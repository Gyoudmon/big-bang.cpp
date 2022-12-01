#include "plane.hpp"
#include "matter.hpp"

#include "graphics/image.hpp"
#include "graphics/geometry.hpp"
#include "graphics/colorspace.hpp"

#include "physics/mathematics.hpp"

#include "datum/string.hpp"
#include "datum/flonum.hpp"

using namespace WarGrey::STEM;

/** NOTE
 *   C-Style casting tries all C++ style casting except dynamic_cast;
 *   `static_cast` almost costs nothing for subtype casting;
 *   `reinterpret_cast` might be harmful for multi-inheritances.
 */
#define MATTER_INFO(m) (static_cast<MatterInfo*>(m->info))

namespace {
    struct AsyncInfo {
        float x0;
        float y0;
        float fx0;
        float fy0;
        float dx0;
        float dy0;
    };

    struct MatterInfo : public WarGrey::STEM::IMatterInfo {
        MatterInfo(WarGrey::STEM::IPlane* master, unsigned int mode) : IMatterInfo(master), mode(mode) {};

        float x = 0.0F;
        float y = 0.0F;
        bool selected = false;
        unsigned int mode = 0U;

        // for asynchronously loaded matters
        AsyncInfo* async = nullptr;

        IMatter* next = nullptr;
        IMatter* prev = nullptr;
    };
}

static inline MatterInfo* bind_matter_owership(WarGrey::STEM::IPlane* master, unsigned int mode, IMatter* m) {
    auto info = new MatterInfo(master, mode);
    
    m->info = info;

    return info;
}

static inline MatterInfo* plane_matter_info(WarGrey::STEM::IPlane* master, IMatter* m) {
    MatterInfo* info = nullptr;

    if ((m != nullptr) && (m->info != nullptr)) {
        if (m->info->master == master) {
            info = MATTER_INFO(m);
        }
    }
    
    return info;
}

static inline bool unsafe_matter_unmasked(MatterInfo* info, unsigned int mode) {
    return ((info->mode & mode) == info->mode);
}

static void unsafe_feed_matter_bound(IMatter* m, MatterInfo* info, float* x, float* y, float* width, float* height) {
    m->feed_extent(info->x, info->y, width, height);

    (*x) = info->x;
    (*y) = info->y;
}

static inline void unsafe_add_selected(WarGrey::STEM::IPlane* master, IMatter* m, MatterInfo* info) {
    master->before_select(m, true);
    info->selected = true;
    master->after_select(m, true);
    master->notify_updated();
}

static inline void unsafe_set_selected(WarGrey::STEM::IPlane* master, IMatter* m, MatterInfo* info) {
    master->begin_update_sequence();
    master->no_selected();
    unsafe_add_selected(master, m, info);
    master->end_update_sequence();
}

static void matter_anchor_fraction(MatterAnchor& a, float* ofx, float* ofy) {
    float fx = 0.0F;
    float fy = 0.0F;

    switch (a) {
        case MatterAnchor::LT:                       break;
        case MatterAnchor::LC:            fy = 0.5F; break;
        case MatterAnchor::LB:            fy = 1.0F; break;
        case MatterAnchor::CT: fx = 0.5F;            break;
        case MatterAnchor::CC: fx = 0.5F; fy = 0.5F; break;
        case MatterAnchor::CB: fx = 0.5F; fy = 1.0F; break;
        case MatterAnchor::RT: fx = 1.0F;            break;
        case MatterAnchor::RC: fx = 1.0F; fy = 0.5F; break;
        case MatterAnchor::RB: fx = 1.0F; fy = 1.0F; break;
    }

    (*ofx) = fx;
    (*ofy) = fy;
}

static bool unsafe_move_matter_via_info(Plane* master, MatterInfo* info, float x, float y, bool absolute) {
    bool moved = false;
    
    if (!absolute) {
        x += info->x;
        y += info->y;
    }

    if ((info->x != x) || (info->y != y)) {
        info->x = x;
        info->y = y;

        master->size_cache_invalid();
        moved = true;
    }

    return moved;
}

static bool unsafe_move_matter_via_info(Plane* master, IMatter* m, MatterInfo* info
    , float x, float y, float fx, float fy, float dx, float dy) {
    float ax = 0.0F;
    float ay = 0.0F;
    
    if (m->ready()) {
        float sx, sy, sw, sh;
        
        unsafe_feed_matter_bound(m, info, &sx, &sy, &sw, &sh);
        ax = (sw * fx);
        ay = (sh * fy);
    } else {
        info->async = new AsyncInfo();

        info->async->x0 = x;
        info->async->y0 = y;
        info->async->fx0 = fx;
        info->async->fy0 = fy;
        info->async->dx0 = dx;
        info->async->dy0 = dy;
    }
    
    return unsafe_move_matter_via_info(master, info, x - ax + dx, y - ay + dy, true);
}

static IMatter* do_search_selected_matter(IMatter* start, unsigned int mode, IMatter* terminator) {
    IMatter* found = nullptr;
    IMatter* child = start;

    do {
        MatterInfo* info = MATTER_INFO(child);

        if (info->selected && (unsafe_matter_unmasked(info, mode))) {
            found = child;
            break;
        }

        child = info->next;
    } while (child != terminator);
    
    return found;
}

static void do_resize(Plane* master, IMatter* m, MatterInfo* info, float scale_x, float scale_y, float prev_scale_x = 1.0F, float prev_scale_y = 1.0F) {
    MatterAnchor resize_anchor;

    // TODO: the theory or implementation seems incorrect. 

    if (m->resizable(&resize_anchor)) {
        float sx, sy, sw, sh, fx, fy, nx, ny, nw, nh;

        unsafe_feed_matter_bound(m, info, &sx, &sy, &sw, &sh);
        matter_anchor_fraction(resize_anchor, &fx, &fy);

        m->resize((sw / prev_scale_x) * scale_x, (sh / prev_scale_y) * scale_y);
        m->feed_extent(sx, sy, &nw, &nh);

        nx = sx + (sw - nw) * fx;
        ny = sy + (sh - nh) * fy;

        unsafe_move_matter_via_info(master, info, nx, ny, true);
    }
}

/*************************************************************************************************/
Plane::Plane(const char* name, unsigned int initial_mode)
    : IPlane(name), head_matter(nullptr), mode(initial_mode) {}

Plane::Plane(const std::string& name, unsigned int initial_mode)
    : Plane(name.c_str(), initial_mode) {}

Plane::~Plane() {
    this->erase();
}

void WarGrey::STEM::Plane::change_mode(unsigned int mode) {
    if (mode != this->mode) {
        this->no_selected();
        this->mode = mode;
        this->size_cache_invalid();
        this->notify_updated();
    }
}

unsigned int WarGrey::STEM::Plane::current_mode() {
    return this->mode;
}

bool WarGrey::STEM::Plane::matter_unmasked(IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);

    return ((info != nullptr) && unsafe_matter_unmasked(info, this->mode));
}

void WarGrey::STEM::Plane::notify_matter_ready(IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        if (info->async != nullptr) {
            this->size_cache_invalid();
            this->begin_update_sequence();

            unsafe_move_matter_via_info(this, m, info,
                info->async->x0, info->async->y0,
                info->async->fx0, info->async->fy0,
                info->async->dx0, info->async->dy0);

            if ((this->scale_x != 1.0F) || (this->scale_y != 1.0F)) {
                do_resize(this, m, info, this->scale_x, this->scale_y);
            }

            delete info->async;
            info->async = nullptr;

            this->notify_updated();
            this->on_matter_ready(m);
            this->end_update_sequence();
        }
    }
}

void WarGrey::STEM::Plane::insert_at(IMatter* m, float x, float y, float fx, float fy, float dx, float dy) {
    if (m->info == nullptr) {
        MatterInfo* info = bind_matter_owership(this, this->mode, m);

        if (this->head_matter == nullptr) {
            this->head_matter = m;
            info->prev = this->head_matter;
        } else {
            MatterInfo* head_info = MATTER_INFO(this->head_matter);
            MatterInfo* prev_info = MATTER_INFO(head_info->prev);
            
            info->prev = head_info->prev;
            prev_info->next = m;
            head_info->prev = m;
        }
        info->next = this->head_matter;

        this->begin_update_sequence();
        m->pre_construct();
        m->construct();
        m->post_construct();
        unsafe_move_matter_via_info(this, m, info, x, y, fx, fy, dx, dy);

        if (m->ready()) {
            if ((this->scale_x != 1.0F) || (this->scale_y != 1.0F)) {
                do_resize(this, m, info, this->scale_x, this->scale_y);
            }

            this->notify_updated();
            this->on_matter_ready(m);
            this->end_update_sequence();
        } else {
            this->notify_updated(); // is it necessary?
            this->end_update_sequence();
        }
    }
}

void WarGrey::STEM::Plane::insert_at(IMatter* m, IMatter* target, float tfx, float tfy, float fx, float fy, float dx, float dy) {
    if (m->info == nullptr) {
        MatterInfo* tinfo = plane_matter_info(this, target);
        float x = 0.0F;
        float y = 0.0F;

        // TODO: what if the target matter is not ready?

        if ((tinfo != nullptr) && unsafe_matter_unmasked(tinfo, this->mode)) {
            float tsx, tsy, tsw, tsh;

            unsafe_feed_matter_bound(target, tinfo, &tsx, &tsy, &tsw, &tsh);
            x = tsx + tsw * tfx;
            y = tsy + tsh * tfy;
        }

        this->insert_at(m, x, y, fx, fy, dx, dy);
    }
}

void WarGrey::STEM::Plane::insert_at(IMatter* m, IMatter* xtarget, float xfx, IMatter* ytarget, float yfy, float fx, float fy, float dx, float dy) {
    if (m->info == nullptr) {
        MatterInfo* xinfo = plane_matter_info(this, xtarget);
        MatterInfo* yinfo = plane_matter_info(this, ytarget);
        float x = 0.0F;
        float y = 0.0F;

        // TODO: what if the target matter is not ready?

        if ((xinfo != nullptr) && unsafe_matter_unmasked(xinfo, this->mode)
            && (yinfo != nullptr) && unsafe_matter_unmasked(yinfo, this->mode)) {
            float xsx, xsy, xsw, xsh, ysx, ysy, ysw, ysh;

            unsafe_feed_matter_bound(xtarget, xinfo, &xsx, &xsy, &xsw, &xsh);
            unsafe_feed_matter_bound(ytarget, yinfo, &ysx, &ysy, &ysw, &ysh);
            x = xsx + xsw * xfx;
            y = ysy + ysh * yfy;
        }

        this->insert_at(m, x, y, fx, fy, dx, dy);
    }
}

void WarGrey::STEM::Plane::remove(IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);

    if ((info != nullptr) && unsafe_matter_unmasked(info, this->mode)) {
        MatterInfo* prev_info = MATTER_INFO(info->prev);
        MatterInfo* next_info = MATTER_INFO(info->next);

        prev_info->next = info->next;
        next_info->prev = info->prev;

        if (this->head_matter == m) {
            if (this->head_matter == info->next) {
                this->head_matter = nullptr;
            } else {
                this->head_matter = info->next;
            }
        }

        if (this->hovering_matter == m) {
            this->hovering_matter = nullptr;
        }
        
        delete m; // m's destructor will delete the associated info object
        this->notify_updated();
        this->size_cache_invalid();
    }
}

void WarGrey::STEM::Plane::erase() {
    if (this->head_matter != nullptr) {
        IMatter* temp_head = this->head_matter;
        MatterInfo* temp_info = MATTER_INFO(temp_head);
        MatterInfo* prev_info = MATTER_INFO(temp_info->prev);

        this->head_matter = nullptr;
        prev_info->next = nullptr;

        do {
            IMatter* child = temp_head;

            temp_head = MATTER_INFO(temp_head)->next;

            delete child; // child's destructor will delete the associated info object
        } while (temp_head != nullptr);

        this->size_cache_invalid();
    }
}

void WarGrey::STEM::Plane::move_to(IMatter* m, float x, float y, float fx, float fy, float dx, float dy) {
    MatterInfo* info = plane_matter_info(this, m);
    
    if ((info != nullptr) && unsafe_matter_unmasked(info, this->mode)) {
        if (unsafe_move_matter_via_info(this, m, info, x, y, fx, fy, dx, dy)) {
            this->notify_updated();
        }
    }
}

void WarGrey::STEM::Plane::move_to(IMatter* m, IMatter* target, float tfx, float tfy, float fx, float fy, float dx, float dy) {
    MatterInfo* tinfo = plane_matter_info(this, target);
    float x = 0.0F;
    float y = 0.0F;

    if ((tinfo != nullptr) && unsafe_matter_unmasked(tinfo, this->mode)) {
        float tsx, tsy, tsw, tsh;

        unsafe_feed_matter_bound(target, tinfo, &tsx, &tsy, &tsw, &tsh);
        x = tsx + tsw * tfx;
        y = tsy + tsh * tfy;
    }
        
    this->move_to(m, x, y, fx, fy, dx, dy);
}

void WarGrey::STEM::Plane::move_to(IMatter* m, IMatter* xtarget, float xfx, IMatter* ytarget, float yfy, float fx, float fy, float dx, float dy) {
    MatterInfo* xinfo = plane_matter_info(this, xtarget);
    MatterInfo* yinfo = plane_matter_info(this, ytarget);
    float x = 0.0F;
    float y = 0.0F;

    if ((xinfo != nullptr) && unsafe_matter_unmasked(xinfo, this->mode)
        && (yinfo != nullptr) && unsafe_matter_unmasked(yinfo, this->mode)) {
        float xsx, xsy, xsw, xsh, ysx, ysy, ysw, ysh;

        unsafe_feed_matter_bound(xtarget, xinfo, &xsx, &xsy, &xsw, &xsh);
        unsafe_feed_matter_bound(ytarget, yinfo, &ysx, &ysy, &ysw, &ysh);
        x = xsx + xsw * xfx;
        y = ysy + ysh * yfy;
    }

    this->move_to(m, x, y, fx, fy, dx, dy);
}

void WarGrey::STEM::Plane::move(IMatter* m, float x, float y) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        if (unsafe_matter_unmasked(info, this->mode)) {
            if (unsafe_move_matter_via_info(this, info, x, y, false)) {
                this->notify_updated();
            }
        }
    } else if (this->head_matter != nullptr) {
        IMatter* child = this->head_matter;

        do {
            info = MATTER_INFO(child);

            if (info->selected && unsafe_matter_unmasked(info, this->mode)) {
                unsafe_move_matter_via_info(this, info, x, y, false);
            }

            child = info->next;
        } while (child != this->head_matter);

        this->notify_updated();
    }
}

IMatter* WarGrey::STEM::Plane::find_matter(float x, float y) {
    IMatter* found = nullptr;

    if (this->head_matter != nullptr) {
        MatterInfo* head_info = MATTER_INFO(this->head_matter);
        IMatter* child = head_info->prev;

        do {
            MatterInfo* info = MATTER_INFO(child);

            if (unsafe_matter_unmasked(info, this->mode)) {
                if (!child->concealled()) {
                    float sx, sy, sw, sh;

                    unsafe_feed_matter_bound(child, info, &sx, &sy, &sw, &sh);

                    sx += (this->translate_x * this->scale_x);
                    sy += (this->translate_y * this->scale_y);

                    if (flin(sx, x, (sx + sw)) && flin(sy, y, (sy + sh))) {
                        if (child->is_colliding_with_mouse(x - sx, y - sy)) {
                            found = child;
                            break;
                        }
                    }
                }
            }

            child = info->prev;
        } while (child != head_info->prev);
    }

    return found;
}

IMatter* WarGrey::STEM::Plane::find_next_selected_matter(IMatter* start) {
    IMatter* found = nullptr;
    
    if (start == nullptr) {
        if (this->head_matter != nullptr) {
            found = do_search_selected_matter(this->head_matter, this->mode, this->head_matter);
        }
    } else {
        MatterInfo* info = plane_matter_info(this, start);

        if ((info != nullptr) && unsafe_matter_unmasked(info, this->mode)) {
            found = do_search_selected_matter(info->next, this->mode, this->head_matter);
        }
    }

    return found;
}

bool WarGrey::STEM::Plane::feed_matter_location(IMatter* m, float* x, float* y, float fx, float fy) {
    MatterInfo* info = plane_matter_info(this, m);
    bool okay = false;
    
    if ((info != nullptr) && unsafe_matter_unmasked(info, this->mode)) {
        float sx, sy, sw, sh;

        unsafe_feed_matter_bound(m, info, &sx, &sy, &sw, &sh);
        if (x != nullptr) (*x) = sx + sw * fx;
        if (y != nullptr) (*y) = sy + sh * fy;

        okay = true;
    }

    return okay;
}

bool WarGrey::STEM::Plane::feed_matter_boundary(IMatter* m, float* x, float* y, float* width, float* height) {
    MatterInfo* info = plane_matter_info(this, m);
    bool okay = false;
    
    if ((info != nullptr) && unsafe_matter_unmasked(info, this->mode)) {
        float sx, sy, sw, sh;
            
        unsafe_feed_matter_bound(m, info, &sx, &sy, &sw, &sh);
        if (x != nullptr) (*x) = sx;
        if (y != nullptr) (*y) = sy;
        if (width != nullptr) (*width) = sw;
        if (height != nullptr) (*height) = sh;

        okay = true;
    }

    return okay;
}

void WarGrey::STEM::Plane::feed_matters_boundary(float* x, float* y, float* width, float* height) {
    this->recalculate_matters_extent_when_invalid();

    if (x != nullptr) (*x) = this->matters_left;
    if (y != nullptr) (*y) = this->matters_top;
    if (width != nullptr) (*width) = this->matters_right - this->matters_left;
    if (height != nullptr) (*height) = this->matters_bottom - this->matters_top;
}

void WarGrey::STEM::Plane::size_cache_invalid() {
    this->matters_right = this->matters_left - 1.0F;
}

void WarGrey::STEM::Plane::recalculate_matters_extent_when_invalid() {
    if (this->matters_right < this->matters_left) {
        float rx, ry, width, height;

        if (this->head_matter == nullptr) {
            this->matters_left = 0.0F;
            this->matters_top = 0.0F;
            this->matters_right = 0.0F;
            this->matters_bottom = 0.0F;
        } else {
            IMatter* child = this->head_matter;

            this->matters_left = infinity_f;
            this->matters_top = infinity_f;
            this->matters_right = -infinity_f;
            this->matters_bottom = -infinity_f;

            do {
                MatterInfo* info = MATTER_INFO(child);

                if (unsafe_matter_unmasked(info, this->mode)) {
                    unsafe_feed_matter_bound(child, info, &rx, &ry, &width, &height);
                    this->matters_left = flmin(this->matters_left, rx);
                    this->matters_top = flmin(this->matters_top, ry);
                    this->matters_right = flmax(this->matters_right, rx + width);
                    this->matters_bottom = flmax(this->matters_bottom, ry + height);
                }

                child = info->next;
            } while (child != this->head_matter);
        }
    }
}

void WarGrey::STEM::Plane::add_selected(IMatter* m) {
    if (this->can_select_multiple()) {
        MatterInfo* info = plane_matter_info(this, m);

        if ((info != nullptr) && (!info->selected)) {
            if (unsafe_matter_unmasked(info, this->mode) && this->can_select(m)) {
                unsafe_add_selected(this, m, info);
            }
        }
    }
}

void WarGrey::STEM::Plane::set_selected(IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);

    if ((info != nullptr) && (!info->selected)) {
        if (unsafe_matter_unmasked(info, this->mode) && (this->can_select(m))) {
            unsafe_set_selected(this, m, info);
        }
    }
}

void WarGrey::STEM::Plane::no_selected() {
    if (this->head_matter != nullptr) {
        IMatter* child = this->head_matter;

        this->begin_update_sequence();

        do {
            MatterInfo* info = MATTER_INFO(child);

            if (info->selected && unsafe_matter_unmasked(info, this->mode)) {
                this->before_select(child, false);
                info->selected = false;
                this->after_select(child, false);
                this->notify_updated();
            }

            child = info->next;
        } while (child != this->head_matter);

        this->end_update_sequence();
    }
}

bool WarGrey::STEM::Plane::is_selected(IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);
    bool selected = false;

    if ((info != nullptr) && unsafe_matter_unmasked(info, this->mode)) {
        selected = info->selected;
    }

    return selected;
}

unsigned int WarGrey::STEM::Plane::count_selected() {
    unsigned int n = 0U;

    if (this->head_matter != nullptr) {
        IMatter* child = this->head_matter;

        do {
            MatterInfo* info = MATTER_INFO(child);

            if (info->selected && unsafe_matter_unmasked(info, this->mode)) {
                n += 1U;
            }

            child = info->next;
        } while (child != this->head_matter);
    }

    return n;
}

IMatter* WarGrey::STEM::Plane::get_focused_matter() {
    return (this->matter_unmasked(this->focused_matter) ? this->focused_matter : nullptr);
}

void WarGrey::STEM::Plane::set_caret_owner(IMatter* m) {
    if (this->focused_matter != m) {
        if ((m != nullptr) && (m->events_allowed())) {
            MatterInfo* info = plane_matter_info(this, m);

            if ((info != nullptr) && unsafe_matter_unmasked(info, this->mode)) {
                if (this->focused_matter != nullptr) {
                    this->focused_matter->own_caret(false);
                    this->on_focus(this->focused_matter, false);
                }

                this->focused_matter = m;
                m->own_caret(true);

                this->on_focus(m, true);
            }
        } else if (this->focused_matter != nullptr) {
            this->focused_matter->own_caret(false);
            this->on_focus(this->focused_matter, false);
            this->focused_matter = nullptr;
        }
    } else if (m != nullptr) {
        this->on_focus(m, true);
    }
}

/************************************************************************************************/
void WarGrey::STEM::Plane::on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) {
    if (this->focused_matter != nullptr) {
        this->focused_matter->on_char(key, modifiers, repeats, pressed);
    }
}

void WarGrey::STEM::Plane::on_text(const char* text, size_t size, bool entire) {
    if (this->focused_matter != nullptr) {
        this->focused_matter->on_text(text, size, entire);
    }
}

void WarGrey::STEM::Plane::on_editing_text(const char* text, int pos, int span) {
    if (this->focused_matter != nullptr) {
        this->focused_matter->on_editing_text(text, pos, span);
    }
}

void WarGrey::STEM::Plane::on_tap(IMatter* m, float local_x, float local_y) {
    if (m != nullptr) {
        MatterInfo* info = MATTER_INFO(m);

        if (!info->selected) {
            if (this->can_select(m)) {
                unsafe_set_selected(this, m, info);

                if (m->events_allowed()) {
                    this->set_caret_owner(m);
                }
            } else {
                this->no_selected();
            }
        }
    }
}

bool WarGrey::STEM::Plane::on_pointer_pressed(uint8_t button, float x, float y, uint8_t clicks, bool touch) {
    bool handled = false;

    switch (clicks) {
        case 1: {
            switch (button) {
                case SDL_BUTTON_LEFT: {
                    IMatter* unmasked_matter = this->find_matter(x, y);

                    this->set_caret_owner(unmasked_matter);
                    this->no_selected();
                    
                    if ((unmasked_matter != nullptr) && (unmasked_matter->low_level_events_allowed())) {
                        MatterInfo* info = MATTER_INFO(unmasked_matter);
                        float local_x = x - info->x;
                        float local_y = y - info->y;

                        handled = unmasked_matter->on_pointer_pressed(button, local_x, local_y, clicks);
                    }
                }; break;
            }
        }; break;
    }

    return handled;
}

bool WarGrey::STEM::Plane::on_pointer_move(uint32_t state, float x, float y, float dx, float dy, bool touch) {
    bool handled = false;

    if (state == 0) {
        IMatter* unmasked_matter = this->find_matter(x, y);

        if (unmasked_matter != this->hovering_matter) {
            this->say_goodbye_to_hover_matter(state, x, y, dx, dy);
        }

        if (unmasked_matter != nullptr) {
            MatterInfo* info = MATTER_INFO(unmasked_matter);
            float local_x = x - info->x;
            float local_y = y - info->y;

            this->hovering_matter = unmasked_matter;

            if (unmasked_matter->events_allowed()) {
                unmasked_matter->on_hover(local_x, local_y);

                if (unmasked_matter->low_level_events_allowed()) {
                    unmasked_matter->on_pointer_move(state, local_x, local_y, dx, dy, false);
                }
            }

            this->on_hover(this->hovering_matter, local_x, local_y);

            handled = true;
        }
    }

    return handled;
}

bool WarGrey::STEM::Plane::on_pointer_released(uint8_t button, float x, float y, uint8_t clicks, bool touch) {
    bool handled = false;

    switch (clicks) {
        case 1: {
            switch (button) {
                case SDL_BUTTON_LEFT: {
                    IMatter* unmasked_matter = this->find_matter(x, y);
        
                    if (unmasked_matter != nullptr) {
                        MatterInfo* info = MATTER_INFO(unmasked_matter);
                        float local_x = x - info->x;
                        float local_y = y - info->y;

                        if (unmasked_matter->events_allowed()) {
                            unmasked_matter->on_tap(local_x, local_y);

                            if (unmasked_matter->low_level_events_allowed()) {
                                unmasked_matter->on_pointer_released(button, local_x, local_y, clicks);
                            }
                        }

                        this->on_tap(unmasked_matter, local_x, local_y);

                        if (info->selected) {
                            this->on_tap_selected(unmasked_matter, local_x, local_y);
                        }

                        handled = info->selected;
                    }
                }; break;
            }
        }; break;
    }

    return handled;
}

bool WarGrey::STEM::Plane::on_scroll(int horizon, int vertical, float hprecise, float vprecise) {
    bool handled = false;

    return handled;
}

bool WarGrey::STEM::Plane::say_goodbye_to_hover_matter(uint32_t state, float x, float y, float dx, float dy) {
    bool done = false;

    if (this->hovering_matter != nullptr) {
        MatterInfo* info = MATTER_INFO(this->hovering_matter);
        float local_x = x - info->x;
        float local_y = y - info->y;

        if (this->hovering_matter->events_allowed()) {
            done |= this->hovering_matter->on_goodbye(local_x, local_y);

            if (this->hovering_matter->low_level_events_allowed()) {
                done |= this->hovering_matter->on_pointer_move(state, local_x, local_y, dx, dy, true);
            }
        }

        this->on_goodbye(this->hovering_matter, local_x, local_y);
        this->hovering_matter = nullptr;
    }

    return done;
}

/************************************************************************************************/
void WarGrey::STEM::Plane::on_elapse(uint32_t count, uint32_t interval, uint32_t uptime) {
    if (this->head_matter != nullptr) {
        IMatter* child = this->head_matter;
        float cwidth, cheight, dwidth, dheight;

        do {
            MatterInfo* info = MATTER_INFO(child);
            this->info->master->feed_extent(&dwidth, &dheight);

            if (unsafe_matter_unmasked(info, this->mode)) {
                IMovable* sprite = child->as_sprite();

                child->update(count, interval, uptime);

                if (sprite != nullptr) {
                    float xspd = sprite->x_speed();
                    float yspd = sprite->y_speed();
                    float hdist = 0.0F;
                    float vdist = 0.0F;

                    if ((xspd != 0.0F) || (yspd != 0.0F)) {
                        info->x += xspd;
                        info->y += yspd;

                        child->feed_extent(info->x, info->y, &cwidth, &cheight);

                        if (info->x < 0) {
                            hdist = info->x;
                        } else if (info->x + cwidth > dwidth) {
                            hdist = info->x + cwidth - dwidth;
                        }

                        if (info->y < 0) {
                            vdist = info->y;
                        } else if (info->y + cheight > dheight) {
                            vdist = info->y + cheight - dheight;
                        }

                        if ((hdist != 0.0F) || (vdist != 0.0F)) {
                            sprite->on_border(hdist, vdist);
                            xspd = sprite->x_speed();
                            yspd = sprite->y_speed();
                        
                            if ((xspd == 0.0F) || (yspd == 0.0F)) {
                                if (info->x < 0.0F) {
                                    info->x = 0.0F;
                                } else if (info->x + cwidth > dwidth) {
                                    info->x = dwidth - cwidth;
                                }

                                if (info->y < 0.0F) {
                                    info->y = 0.0F;
                                } else if (info->y + cheight > dheight) {
                                    info->y = dheight - cheight;
                                }
                            }
                        }
                        
                        this->notify_updated();
                    }
                }
            }
            
            child = info->next;
        } while (child != this->head_matter);
    }

    this->update(count, interval, uptime);
}

void WarGrey::STEM::Plane::draw(SDL_Renderer* renderer, float X, float Y, float Width, float Height) {
    float dsX = flmax(0.0F, X);
    float dsY = flmax(0.0F, Y);
    float dsWidth = X + Width;
    float dsHeight = Y + Height;
    
    if (this->bg_alpha > 0.0F) {
        game_fill_rect(renderer, dsX, dsY, dsWidth, dsHeight, this->background, this->bg_alpha);
    }

    if (this->head_matter != nullptr) {
        IMatter* child = this->head_matter;
        float gx, gy, gwidth, gheight;
        SDL_Rect clip;
        
        do {
            MatterInfo* info = MATTER_INFO(child);

            if (unsafe_matter_unmasked(info, this->mode)) {
                child->feed_extent(info->x, info->y, &gwidth, &gheight);

                gx = (info->x + this->translate_x) * this->scale_x + X;
                gy = (info->y + this->translate_y) * this->scale_y + Y;
                
                if (rectangle_overlay(gx, gy, gx + gwidth, gy + gheight, dsX, dsY, dsWidth, dsHeight)) {
                    clip.x = fl2fxi(flfloor(gx));
                    clip.y = fl2fxi(flfloor(gy));
                    clip.w = fl2fxi(flceiling(gwidth));
                    clip.h = fl2fxi(flceiling(gheight));

                    SDL_RenderSetClipRect(renderer, &clip);
                    child->draw(renderer, gx, gy, gwidth, gheight);

                    if (info->selected) {
                        SDL_RenderSetClipRect(renderer, nullptr);
                        this->draw_visible_selection(renderer, gx, gy, gwidth, gheight);
                    }
                }
            }

            child = info->next;
        } while (child != this->head_matter);
                    
        SDL_RenderSetClipRect(renderer, nullptr);
    }
}

void WarGrey::STEM::Plane::draw_visible_selection(SDL_Renderer* renderer, float x, float y, float width, float height) {
    game_draw_rect(renderer, x, y, width, height, 0x00FFFFU);
}

/*************************************************************************************************/
WarGrey::STEM::IPlane::IPlane(const char* name) : caption(name) {}
WarGrey::STEM::IPlane::IPlane(const std::string& name) : IPlane(name.c_str()) {}

WarGrey::STEM::IPlane::~IPlane() {
    if (this->info != nullptr) {
        delete this->info;
        this->info = nullptr;
    }
}

const char* WarGrey::STEM::IPlane::name() {
    return this->caption.c_str();
}

IScreen* WarGrey::STEM::IPlane::master() {
    IScreen* screen = nullptr;

    if (this->info != nullptr) {
        screen = this->info->master;
    }

    return screen;
}

void WarGrey::STEM::IPlane::feed_background(SDL_Color* c) {
    RGB_FillColor(c, this->background, this->bg_alpha);
}

void WarGrey::STEM::IPlane::start_input_text(const char* fmt, ...) {
    if (this->info != nullptr) {
        VSNPRINT(prompt, fmt);
        this->start_input_text(prompt);
    }
}

void WarGrey::STEM::IPlane::start_input_text(const std::string& prompt) {
    if (this->info != nullptr) {
        this->info->master->start_input_text(prompt);
    }
}

void WarGrey::STEM::IPlane::log_message(const char* fmt, ...) {
    if (this->info != nullptr) {
        VSNPRINT(msg, fmt);
        this->log_message(-1, msg);
    }
}

void WarGrey::STEM::IPlane::log_message(int fgc, const char* fmt, ...) {
    if (this->info != nullptr) {
        VSNPRINT(msg, fmt);
        this->log_message(fgc, msg);
    }
}

void WarGrey::STEM::IPlane::log_message(int fgc, const std::string& msg) {
    if (this->info != nullptr) {
        this->info->master->log_message(fgc, msg);
    }
}

void WarGrey::STEM::IPlane::begin_update_sequence() {
    if (this->info != nullptr) {
        this->info->master->begin_update_sequence();
    }
}

bool WarGrey::STEM::IPlane::is_in_update_sequence() {
    return ((this->info != nullptr) && this->info->master->is_in_update_sequence());
}

void WarGrey::STEM::IPlane::end_update_sequence() {
    if (this->info != nullptr) {
        this->info->master->end_update_sequence();
    }
}

bool WarGrey::STEM::IPlane::should_update() {
    return ((this->info != nullptr) && this->info->master->should_update());
}

void WarGrey::STEM::IPlane::notify_updated() {
    if (this->info != nullptr) {
        this->info->master->notify_updated();
    }
}

SDL_Surface* WarGrey::STEM::IPlane::snapshot(float width, float height, uint32_t bgcolor, float alpha) {
    return this->snapshot(0.0F, 0.0F, width, height, bgcolor, alpha);
}

SDL_Surface* WarGrey::STEM::IPlane::snapshot(float x, float y, float width, float height, uint32_t bgcolor, float alpha) {
    SDL_Surface* photograph = nullptr;
    int saved_bgc = this->background;
    float saved_alpha = this->bg_alpha;

    if (x != 0.0F) width += x;
    if (y != 0.0F) height += y;

    photograph = game_blank_image(width, height);

    if (photograph != nullptr) {
        SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(photograph);
        
        if (renderer != nullptr) {
            this->background = bgcolor;
            this->bg_alpha = alpha;
            
            this->draw(renderer, -x, -y, width, height);
            SDL_RenderPresent(renderer);
            SDL_DestroyRenderer(renderer);

            this->background = saved_bgc;
            this->bg_alpha = saved_alpha;
        }
    }

    return photograph;
}

bool WarGrey::STEM::IPlane::save_snapshot(const std::string& png, float width, float height, uint32_t bgcolor, float alpha) {
    return this->save_snapshot(png.c_str(), 0.0F, 0.0F, width, height, bgcolor, alpha);
}

bool WarGrey::STEM::IPlane::save_snapshot(const char* png, float width, float height, uint32_t bgcolor, float alpha) {
    return this->save_snapshot(png, 0.0F, 0.0F, width, height, bgcolor, alpha);
}

bool WarGrey::STEM::IPlane::save_snapshot(const std::string& png, float x, float y, float width, float height, uint32_t bgcolor, float alpha) {
    return this->save_snapshot(png.c_str(), x, y, width, height, bgcolor, alpha);
}

bool WarGrey::STEM::IPlane::save_snapshot(const char* png, float x, float y, float width, float height, uint32_t bgcolor, float alpha) {
    SDL_Surface* photograph = this->snapshot(x, y, width, height, bgcolor, alpha);
    bool okay = game_save_image(photograph, png);
    
    SDL_FreeSurface(photograph);

    return okay;
}

bool WarGrey::STEM::IPlane::feed_matter_location(IMatter* m, float* x, float* y, MatterAnchor a) {
    float fx, fy;

    matter_anchor_fraction(a, &fx, &fy);

    return this->feed_matter_location(m, x, y, fx, fy);
}

void WarGrey::STEM::IPlane::insert_at(IMatter* m, float x, float y, MatterAnchor a, float dx, float dy) {
    float fx, fy;

    matter_anchor_fraction(a, &fx, &fy);

    this->insert_at(m, x, y, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlane::insert_at(IMatter* m, IMatter* target, MatterAnchor ta, MatterAnchor a, float dx, float dy) {
    float tfx, tfy, fx, fy;

    matter_anchor_fraction(ta, &tfx, &tfy);
    matter_anchor_fraction(a, &fx, &fy);

    this->insert_at(m, target, tfx, tfy, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlane::insert_at(IMatter* m, IMatter* target, float tfx, float tfy, MatterAnchor a, float dx, float dy) {
    float fx, fy;

    matter_anchor_fraction(a, &fx, &fy);

    this->insert_at(m, target, tfx, tfy, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlane::insert_at(IMatter* m, IMatter* target, MatterAnchor ta, float fx, float fy, float dx, float dy) {
    float tfx, tfy;

    matter_anchor_fraction(ta, &tfx, &tfy);
    
    this->insert_at(m, target, tfx, tfy, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlane::insert_at(IMatter* m, IMatter* xtarget, float xfx, IMatter* ytarget, float yfy, MatterAnchor a, float dx, float dy) {
    float fx, fy;

    matter_anchor_fraction(a, &fx, &fy);

    this->insert_at(m, xtarget, xfx, ytarget, yfy, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlane::move_to(IMatter* m, float x, float y, MatterAnchor a, float dx, float dy) {
    float fx, fy;

    matter_anchor_fraction(a, &fx, &fy);

    this->move_to(m, x, y, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlane::move_to(IMatter* m, IMatter* target, MatterAnchor ta, MatterAnchor a, float dx, float dy) {
    float tfx, tfy, fx, fy;

    matter_anchor_fraction(ta, &tfx, &tfy);
    matter_anchor_fraction(a, &fx, &fy);

    this->move_to(m, target, tfx, tfy, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlane::move_to(IMatter* m, IMatter* target, float tfx, float tfy, MatterAnchor a, float dx, float dy) {
    float fx, fy;

    matter_anchor_fraction(a, &fx, &fy);

    this->move_to(m, target, tfx, tfy, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlane::move_to(IMatter* m, IMatter* target, MatterAnchor ta, float fx, float fy, float dx, float dy) {
    float tfx, tfy;

    matter_anchor_fraction(ta, &tfx, &tfy);

    this->move_to(m, target, tfx, tfy, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlane::move_to(IMatter* m, IMatter* xtarget, float xfx, IMatter* ytarget, float yfy, MatterAnchor a, float dx, float dy) {
    float fx, fy;
    
    matter_anchor_fraction(a, &fx, &fy);

    this->move_to(m, xtarget, xfx, ytarget, yfy, fx, fy, dx, dy);
}

