#include "planet.hpp"
#include "matter.hpp"

#include "geometry.hpp"
#include "mathematics.hpp"
#include "colorspace.hpp"

#include "datum/string.hpp"
#include "datum/flonum.hpp"

using namespace WarGrey::STEM;

/** NOTE
 *   C-Style casting tries all C++ style casting except dynamic_cast;
 *   reinterpret_cast may cause "Access violation reading location 0xFFFFFFFFFFFFFFFF" even for subtype casting.
 */

#define GRAPHLET_INFO(g) (static_cast<MatterInfo*>(g->info))

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
        MatterInfo(WarGrey::STEM::IPlanet* master, unsigned int mode) : IMatterInfo(master), mode(mode) {};

        float x = 0.0F;
        float y = 0.0F;
        bool selected = false;
        unsigned int mode = 0U;

        // for asynchronously loaded graphlets
        AsyncInfo* async = nullptr;

        IMatter* next = nullptr;
        IMatter* prev = nullptr;
    };
}

static inline MatterInfo* bind_graphlet_owership(WarGrey::STEM::IPlanet* master, unsigned int mode, IMatter* g) {
    auto info = new MatterInfo(master, mode);
    
    g->info = info;

    return info;
}

static inline MatterInfo* planet_graphlet_info(WarGrey::STEM::IPlanet* master, IMatter* g) {
    MatterInfo* info = nullptr;

    if ((g != nullptr) && (g->info != nullptr)) {
        if (g->info->master == master) {
            info = GRAPHLET_INFO(g);
        }
    }
    
    return info;
}

static inline bool unsafe_graphlet_unmasked(MatterInfo* info, unsigned int mode) {
    return ((info->mode & mode) == info->mode);
}

static void unsafe_fill_graphlet_bound(IMatter* g, MatterInfo* info, float* x, float* y, float* width, float* height) {
    g->fill_extent(info->x, info->y, width, height);

    (*x) = info->x;
    (*y) = info->y;
}

static inline void unsafe_add_selected(WarGrey::STEM::IPlanet* master, IMatter* g, MatterInfo* info) {
    master->before_select(g, true);
    info->selected = true;
    master->after_select(g, true);
    master->notify_updated();
}

static inline void unsafe_set_selected(WarGrey::STEM::IPlanet* master, IMatter* g, MatterInfo* info) {
    master->begin_update_sequence();
    master->no_selected();
    unsafe_add_selected(master, g, info);
    master->end_update_sequence();
}

static void graphlet_anchor_fraction(MatterAnchor& a, float* ofx, float* ofy) {
    float fx = 0.0F;
    float fy = 0.0F;

    if (a != MatterAnchor::LT) {
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
    }

    (*ofx) = fx;
    (*ofy) = fy;
}

static bool unsafe_move_graphlet_via_info(Planet* master, MatterInfo* info, float x, float y, bool absolute) {
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

static bool unsafe_move_graphlet_via_info(Planet* master, IMatter* g, MatterInfo* info
    , float x, float y, float fx, float fy, float dx, float dy, bool absolute) {
    float sx, sy, sw, sh;
    float ax = 0.0F;
    float ay = 0.0F;
    
    if (g->ready()) {
        unsafe_fill_graphlet_bound(g, info, &sx, &sy, &sw, &sh);
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
    
    return unsafe_move_graphlet_via_info(master, info, x - ax + dx, y - ay + dy, true);
}

static IMatter* do_search_selected_graphlet(IMatter* start, unsigned int mode, IMatter* terminator) {
    IMatter* found = nullptr;
    IMatter* child = start;

    do {
        MatterInfo* info = GRAPHLET_INFO(child);

        if (info->selected && (unsafe_graphlet_unmasked(info, mode))) {
            found = child;
            break;
        }

        child = info->next;
    } while (child != terminator);
    
    return found;
}

static void do_resize(Planet* master, IMatter* g, MatterInfo* info, float scale_x, float scale_y, float prev_scale_x = 1.0F, float prev_scale_y = 1.0F) {
    MatterAnchor resize_anchor;

    // TODO: the theory or implementation seems incorrect. 

    if (g->resizable(&resize_anchor)) {
        float sx, sy, sw, sh, fx, fy, nx, ny, nw, nh;

        unsafe_fill_graphlet_bound(g, info, &sx, &sy, &sw, &sh);
        graphlet_anchor_fraction(resize_anchor, &fx, &fy);

        g->resize((sw / prev_scale_x) * scale_x, (sh / prev_scale_y) * scale_y);
        g->fill_extent(sx, sy, &nw, &nh);

        nx = sx + (sw - nw) * fx;
        ny = sy + (sh - nh) * fy;

        unsafe_move_graphlet_via_info(master, info, nx, ny, true);
    }
}

/*************************************************************************************************/
Planet::Planet(const char* name, unsigned int initial_mode)
    : IPlanet(name), head_graphlet(nullptr), mode(initial_mode) {}

Planet::Planet(const std::string& name, unsigned int initial_mode)
    : Planet(name.c_str(), initial_mode) {}

Planet::~Planet() {
    this->collapse();
}

void Planet::collapse() {
    this->erase();
}

void WarGrey::STEM::Planet::change_mode(unsigned int mode) {
    if (mode != this->mode) {
        this->no_selected();
        this->mode = mode;
        this->size_cache_invalid();
        this->notify_updated();
    }
}

unsigned int WarGrey::STEM::Planet::current_mode() {
    return this->mode;
}

bool WarGrey::STEM::Planet::graphlet_unmasked(IMatter* g) {
    MatterInfo* info = planet_graphlet_info(this, g);

    return ((info != nullptr) && unsafe_graphlet_unmasked(info, this->mode));
}

void WarGrey::STEM::Planet::notify_graphlet_ready(IMatter* g) {
    MatterInfo* info = planet_graphlet_info(this, g);

    if (info != nullptr) {
        if (info->async != nullptr) {
            this->size_cache_invalid();
            this->begin_update_sequence();

            unsafe_move_graphlet_via_info(this, g, info,
                info->async->x0, info->async->y0, info->async->fx0, info->async->fy0, info->async->dx0, info->async->dy0,
                true);

            if ((this->scale_x != 1.0F) || (this->scale_y != 1.0F)) {
                do_resize(this, g, info, this->scale_x, this->scale_y);
            }

            delete info->async;
            info->async = nullptr;

            this->notify_updated();
            this->on_graphlet_ready(g);
            this->end_update_sequence();
        }
    }
}

void WarGrey::STEM::Planet::insert(IMatter* g, float x, float y, float fx, float fy, float dx, float dy) {
    if (g->info == nullptr) {
        MatterInfo* info = bind_graphlet_owership(this, this->mode, g);

        if (this->head_graphlet == nullptr) {
            this->head_graphlet = g;
            info->prev = this->head_graphlet;
        } else {
            MatterInfo* head_info = GRAPHLET_INFO(this->head_graphlet);
            MatterInfo* prev_info = GRAPHLET_INFO(head_info->prev);
            
            info->prev = head_info->prev;
            prev_info->next = g;
            head_info->prev = g;
        }
        info->next = this->head_graphlet;

        this->begin_update_sequence();
        g->pre_construct();
        g->construct();
        g->post_construct();
        unsafe_move_graphlet_via_info(this, g, info, x, y, fx, fy, dx, dy, true);

        if (g->ready()) {
            if ((this->scale_x != 1.0F) || (this->scale_y != 1.0F)) {
                do_resize(this, g, info, this->scale_x, this->scale_y);
            }

            this->notify_updated();
            this->on_graphlet_ready(g);
            this->end_update_sequence();
        } else {
            this->notify_updated(); // is it necessary?
            this->end_update_sequence();
        }
    }
}

void WarGrey::STEM::Planet::insert(IMatter* g, IMatter* target, float tfx, float tfy, float fx, float fy, float dx, float dy) {
    if (g->info == nullptr) {
        MatterInfo* tinfo = planet_graphlet_info(this, target);
        float x = 0.0F;
        float y = 0.0F;

        // TODO: what if the target graphlet is not ready?

        if ((tinfo != nullptr) && unsafe_graphlet_unmasked(tinfo, this->mode)) {
            float tsx, tsy, tsw, tsh;

            unsafe_fill_graphlet_bound(target, tinfo, &tsx, &tsy, &tsw, &tsh);
            x = tsx + tsw * tfx;
            y = tsy + tsh * tfy;
        }

        this->insert(g, x, y, fx, fy, dx, dy);
    }
}

void WarGrey::STEM::Planet::insert(IMatter* g, IMatter* xtarget, float xfx, IMatter* ytarget, float yfy, float fx, float fy, float dx, float dy) {
    if (g->info == nullptr) {
        MatterInfo* xinfo = planet_graphlet_info(this, xtarget);
        MatterInfo* yinfo = planet_graphlet_info(this, ytarget);
        float x = 0.0F;
        float y = 0.0F;

        // TODO: what if the target graphlet is not ready?

        if ((xinfo != nullptr) && unsafe_graphlet_unmasked(xinfo, this->mode)
            && (yinfo != nullptr) && unsafe_graphlet_unmasked(yinfo, this->mode)) {
            float xsx, xsy, xsw, xsh, ysx, ysy, ysw, ysh;

            unsafe_fill_graphlet_bound(xtarget, xinfo, &xsx, &xsy, &xsw, &xsh);
            unsafe_fill_graphlet_bound(ytarget, yinfo, &ysx, &ysy, &ysw, &ysh);
            x = xsx + xsw * xfx;
            y = ysy + ysh * yfy;
        }

        this->insert(g, x, y, fx, fy, dx, dy);
    }
}

void WarGrey::STEM::Planet::remove(IMatter* g) {
    MatterInfo* info = planet_graphlet_info(this, g);

    if ((info != nullptr) && unsafe_graphlet_unmasked(info, this->mode)) {
        MatterInfo* prev_info = GRAPHLET_INFO(info->prev);
        MatterInfo* next_info = GRAPHLET_INFO(info->next);

        prev_info->next = info->next;
        next_info->prev = info->prev;

        if (this->head_graphlet == g) {
            if (this->head_graphlet == info->next) {
                this->head_graphlet = nullptr;
            } else {
                this->head_graphlet = info->next;
            }
        }

        if (this->hovering_graphlet == g) {
            this->hovering_graphlet = nullptr;
        }
        
        delete g; // g's destructor will delete the associated info object
        this->notify_updated();
        this->size_cache_invalid();
    }
}

void WarGrey::STEM::Planet::erase() {
    if (this->head_graphlet != nullptr) {
        IMatter* temp_head = this->head_graphlet;
        MatterInfo* temp_info = GRAPHLET_INFO(temp_head);
        MatterInfo* prev_info = GRAPHLET_INFO(temp_info->prev);

        this->head_graphlet = nullptr;
        prev_info->next = nullptr;

        do {
            IMatter* child = temp_head;

            temp_head = GRAPHLET_INFO(temp_head)->next;

            delete child; // child's destructor will delete the associated info object
        } while (temp_head != nullptr);

        this->head_graphlet = nullptr;
        this->size_cache_invalid();
    }
}

void WarGrey::STEM::Planet::move_to(IMatter* g, float x, float y, float fx, float fy, float dx, float dy) {
    MatterInfo* info = planet_graphlet_info(this, g);
    
    if ((info != nullptr) && unsafe_graphlet_unmasked(info, this->mode)) {
        if (unsafe_move_graphlet_via_info(this, g, info, x, y, fx, fy, dx, dy, true)) {
            this->notify_updated();
        }
    }
}

void WarGrey::STEM::Planet::move_to(IMatter* g, IMatter* target, float tfx, float tfy, float fx, float fy, float dx, float dy) {
    MatterInfo* tinfo = planet_graphlet_info(this, target);
    float x = 0.0F;
    float y = 0.0F;

    if ((tinfo != nullptr) && unsafe_graphlet_unmasked(tinfo, this->mode)) {
        float tsx, tsy, tsw, tsh;

        unsafe_fill_graphlet_bound(target, tinfo, &tsx, &tsy, &tsw, &tsh);
        x = tsx + tsw * tfx;
        y = tsy + tsh * tfy;
    }
        
    this->move_to(g, x, y, fx, fy, dx, dy);
}

void WarGrey::STEM::Planet::move_to(IMatter* g, IMatter* xtarget, float xfx, IMatter* ytarget, float yfy, float fx, float fy, float dx, float dy) {
    MatterInfo* xinfo = planet_graphlet_info(this, xtarget);
    MatterInfo* yinfo = planet_graphlet_info(this, ytarget);
    float x = 0.0F;
    float y = 0.0F;

    if ((xinfo != nullptr) && unsafe_graphlet_unmasked(xinfo, this->mode)
        && (yinfo != nullptr) && unsafe_graphlet_unmasked(yinfo, this->mode)) {
        float xsx, xsy, xsw, xsh, ysx, ysy, ysw, ysh;

        unsafe_fill_graphlet_bound(xtarget, xinfo, &xsx, &xsy, &xsw, &xsh);
        unsafe_fill_graphlet_bound(ytarget, yinfo, &ysx, &ysy, &ysw, &ysh);
        x = xsx + xsw * xfx;
        y = ysy + ysh * yfy;
    }

    this->move_to(g, x, y, fx, fy, dx, dy);
}

void WarGrey::STEM::Planet::move(IMatter* g, float x, float y) {
    MatterInfo* info = planet_graphlet_info(this, g);

    if (info != nullptr) {
        if (unsafe_graphlet_unmasked(info, this->mode)) {
            if (unsafe_move_graphlet_via_info(this, info, x, y, false)) {
                this->notify_updated();
            }
        }
    } else if (this->head_graphlet != nullptr) {
        IMatter* child = this->head_graphlet;

        do {
            info = GRAPHLET_INFO(child);

            if (info->selected && unsafe_graphlet_unmasked(info, this->mode)) {
                unsafe_move_graphlet_via_info(this, info, x, y, false);
            }

            child = info->next;
        } while (child != this->head_graphlet);

        this->notify_updated();
    }
}

IMatter* WarGrey::STEM::Planet::find_graphlet(float x, float y) {
    IMatter* found = nullptr;

    if (this->head_graphlet != nullptr) {
        MatterInfo* head_info = GRAPHLET_INFO(this->head_graphlet);
        IMatter* child = head_info->prev;

        do {
            MatterInfo* info = GRAPHLET_INFO(child);

            if (unsafe_graphlet_unmasked(info, this->mode)) {
                if (!child->concealled()) {
                    float sx, sy, sw, sh;

                    unsafe_fill_graphlet_bound(child, info, &sx, &sy, &sw, &sh);

                    sx += (this->translate_x * this->scale_x);
                    sy += (this->translate_y * this->scale_y);

                    if ((sx < x) && (x < (sx + sw)) && (sy < y) && (y < (sy + sh))) {
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

IMatter* WarGrey::STEM::Planet::find_next_selected_graphlet(IMatter* start) {
    IMatter* found = nullptr;
    
    if (start == nullptr) {
        if (this->head_graphlet != nullptr) {
            found = do_search_selected_graphlet(this->head_graphlet, this->mode, this->head_graphlet);
        }
    } else {
        MatterInfo* info = planet_graphlet_info(this, start);

        if ((info != nullptr) && unsafe_graphlet_unmasked(info, this->mode)) {
            found = do_search_selected_graphlet(info->next, this->mode, this->head_graphlet);
        }
    }

    return found;
}

bool WarGrey::STEM::Planet::fill_graphlet_location(IMatter* g, float* x, float* y, float fx, float fy) {
    bool okay = false;
    MatterInfo* info = planet_graphlet_info(this, g);
    
    if ((info != nullptr) && unsafe_graphlet_unmasked(info, this->mode)) {
        float sx, sy, sw, sh;

        unsafe_fill_graphlet_bound(g, info, &sx, &sy, &sw, &sh);
        if (x != nullptr) (*x) = sx + sw * fx;
        if (y != nullptr) (*y) = sy + sh * fy;

        okay = true;
    }

    return okay;
}

bool WarGrey::STEM::Planet::fill_graphlet_boundary(IMatter* g, float* x, float* y, float* width, float* height) {
    bool okay = false;
    MatterInfo* info = planet_graphlet_info(this, g);

    if ((info != nullptr) && unsafe_graphlet_unmasked(info, this->mode)) {
        float sx, sy, sw, sh;
            
        unsafe_fill_graphlet_bound(g, info, &sx, &sy, &sw, &sh);
        if (x != nullptr) (*x) = sx;
        if (y != nullptr) (*y) = sy;
        if (width != nullptr) (*width) = sw;
        if (height != nullptr) (*height) = sh;

        okay = true;
    }

    return okay;
}

void WarGrey::STEM::Planet::fill_graphlets_boundary(float* x, float* y, float* width, float* height) {
    this->recalculate_graphlets_extent_when_invalid();

    if (x != nullptr) (*x) = this->graphlets_left;
    if (y != nullptr) (*y) = this->graphlets_top;
    if (width != nullptr) (*width) = this->graphlets_right - this->graphlets_left;
    if (height != nullptr) (*height) = this->graphlets_bottom - this->graphlets_top;
}

void WarGrey::STEM::Planet::size_cache_invalid() {
    this->graphlets_right = this->graphlets_left - 1.0F;
}

void WarGrey::STEM::Planet::recalculate_graphlets_extent_when_invalid() {
    if (this->graphlets_right < this->graphlets_left) {
        float rx, ry, width, height;

        if (this->head_graphlet == nullptr) {
            this->graphlets_left = 0.0F;
            this->graphlets_top = 0.0F;
            this->graphlets_right = 0.0F;
            this->graphlets_bottom = 0.0F;
        } else {
            IMatter* child = this->head_graphlet;

            this->graphlets_left = infinity_f;
            this->graphlets_top = infinity_f;
            this->graphlets_right = -infinity_f;
            this->graphlets_bottom = -infinity_f;

            do {
                MatterInfo* info = GRAPHLET_INFO(child);

                if (unsafe_graphlet_unmasked(info, this->mode)) {
                    unsafe_fill_graphlet_bound(child, info, &rx, &ry, &width, &height);
                    this->graphlets_left = flmin(this->graphlets_left, rx);
                    this->graphlets_top = flmin(this->graphlets_top, ry);
                    this->graphlets_right = flmax(this->graphlets_right, rx + width);
                    this->graphlets_bottom = flmax(this->graphlets_bottom, ry + height);
                }

                child = info->next;
            } while (child != this->head_graphlet);
        }
    }
}

void WarGrey::STEM::Planet::add_selected(IMatter* g) {
    if (this->can_select_multiple()) {
        MatterInfo* info = planet_graphlet_info(this, g);

        if ((info != nullptr) && (!info->selected)) {
            if (unsafe_graphlet_unmasked(info, this->mode) && this->can_select(g)) {
                unsafe_add_selected(this, g, info);
            }
        }
    }
}

void WarGrey::STEM::Planet::set_selected(IMatter* g) {
    MatterInfo* info = planet_graphlet_info(this, g);

    if ((info != nullptr) && (!info->selected)) {
        if (unsafe_graphlet_unmasked(info, this->mode) && (this->can_select(g))) {
            unsafe_set_selected(this, g, info);
        }
    }
}

void WarGrey::STEM::Planet::no_selected() {
    if (this->head_graphlet != nullptr) {
        IMatter* child = this->head_graphlet;

        this->begin_update_sequence();

        do {
            MatterInfo* info = GRAPHLET_INFO(child);

            if (info->selected && unsafe_graphlet_unmasked(info, this->mode)) {
                this->before_select(child, false);
                info->selected = false;
                this->after_select(child, false);
                this->notify_updated();
            }

            child = info->next;
        } while (child != this->head_graphlet);

        this->end_update_sequence();
    }
}

bool WarGrey::STEM::Planet::is_selected(IMatter* g) {
    MatterInfo* info = planet_graphlet_info(this, g);
    bool selected = false;

    if ((info != nullptr) && unsafe_graphlet_unmasked(info, this->mode)) {
        selected = info->selected;
    }

    return selected;
}

unsigned int WarGrey::STEM::Planet::count_selected() {
    unsigned int n = 0U;

    if (this->head_graphlet != nullptr) {
        IMatter* child = this->head_graphlet;

        do {
            MatterInfo* info = GRAPHLET_INFO(child);

            if (info->selected && unsafe_graphlet_unmasked(info, this->mode)) {
                n += 1U;
            }

            child = info->next;
        } while (child != this->head_graphlet);
    }

    return n;
}

IMatter* WarGrey::STEM::Planet::get_focus_graphlet() {
    return (this->graphlet_unmasked(this->focused_graphlet) ? this->focused_graphlet : nullptr);
}

void WarGrey::STEM::Planet::set_caret_owner(IMatter* g) {
    if (this->focused_graphlet != g) {
        if ((g != nullptr) && (g->handle_events())) {
            MatterInfo* info = planet_graphlet_info(this, g);

            if ((info != nullptr) && unsafe_graphlet_unmasked(info, this->mode)) {
                if (this->focused_graphlet != nullptr) {
                    this->focused_graphlet->own_caret(false);
                    this->on_focus(this->focused_graphlet, false);
                }

                this->focused_graphlet = g;
                g->own_caret(true);

                this->on_focus(g, true);
            }
        } else if (this->focused_graphlet != nullptr) {
            this->focused_graphlet->own_caret(false);
            this->on_focus(this->focused_graphlet, false);
            this->focused_graphlet = nullptr;
        }
    } else if (g != nullptr) {
        this->on_focus(g, true);
    }
}

/************************************************************************************************/
void WarGrey::STEM::Planet::on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) {
    if (this->focused_graphlet != nullptr) {
        this->focused_graphlet->on_char(key, modifiers, repeats, pressed);
    }
}

void WarGrey::STEM::Planet::on_text(const char* text, size_t size, bool entire) {
    if (this->focused_graphlet != nullptr) {
        this->focused_graphlet->on_text(text, size, entire);
    }
}

void WarGrey::STEM::Planet::on_editing_text(const char* text, int pos, int span) {
    if (this->focused_graphlet != nullptr) {
        this->focused_graphlet->on_editing_text(text, pos, span);
    }
}

void WarGrey::STEM::Planet::on_tap(IMatter* g, float local_x, float local_y) {
    if (g != nullptr) {
        MatterInfo* info = GRAPHLET_INFO(g);

        if (!info->selected) {
            if (this->can_select(g)) {
                unsafe_set_selected(this, g, info);

                if (g->handle_events()) {
                    this->set_caret_owner(g);
                }
            } else {
                this->no_selected();
            }
        }
    }
}

bool WarGrey::STEM::Planet::on_pointer_pressed(uint8_t button, float x, float y, uint8_t clicks, bool touch) {
    bool handled = false;

    switch (clicks) {
        case 1: {
            switch (button) {
                case SDL_BUTTON_LEFT: {
                    IMatter* unmasked_graphlet = this->find_graphlet(x, y);

                    this->set_caret_owner(unmasked_graphlet);
                    this->no_selected();
                    
                    if ((unmasked_graphlet != nullptr) && (unmasked_graphlet->handle_low_level_events())) {
                        MatterInfo* info = GRAPHLET_INFO(unmasked_graphlet);
                        float local_x = x - info->x;
                        float local_y = y - info->y;

                        handled = unmasked_graphlet->on_pointer_pressed(button, local_x, local_y, clicks);
                    }
                }; break;
            }
        }; break;
    }

    return handled;
}

bool WarGrey::STEM::Planet::on_pointer_move(uint32_t state, float x, float y, float dx, float dy, bool touch) {
    bool handled = false;

    if (state == 0) {
        IMatter* unmasked_graphlet = this->find_graphlet(x, y);

        if (unmasked_graphlet != this->hovering_graphlet) {
            this->say_goodbye_to_hover_graphlet(state, x, y, dx, dy);
        }

        if (unmasked_graphlet != nullptr) {
            MatterInfo* info = GRAPHLET_INFO(unmasked_graphlet);
            float local_x = x - info->x;
            float local_y = y - info->y;

            this->hovering_graphlet = unmasked_graphlet;

            if (unmasked_graphlet->handle_events()) {
                unmasked_graphlet->on_hover(local_x, local_y);

                if (unmasked_graphlet->handle_low_level_events()) {
                    unmasked_graphlet->on_pointer_move(state, local_x, local_y, dx, dy, false);
                }
            }

            this->on_hover(this->hovering_graphlet, local_x, local_y);

            handled = true;
        }
    }

    return handled;
}

bool WarGrey::STEM::Planet::on_pointer_released(uint8_t button, float x, float y, uint8_t clicks, bool touch) {
    bool handled = false;

    switch (clicks) {
        case 1: {
            switch (button) {
                case SDL_BUTTON_LEFT: {
                    IMatter* unmasked_graphlet = this->find_graphlet(x, y);
        
                    if (unmasked_graphlet != nullptr) {
                        MatterInfo* info = GRAPHLET_INFO(unmasked_graphlet);
                        float local_x = x - info->x;
                        float local_y = y - info->y;

                        if (unmasked_graphlet->handle_events()) {
                            unmasked_graphlet->on_tap(local_x, local_y);

                            if (unmasked_graphlet->handle_low_level_events()) {
                                unmasked_graphlet->on_pointer_released(button, local_x, local_y, clicks);
                            }
                        }

                        this->on_tap(unmasked_graphlet, local_x, local_y);

                        if (info->selected) {
                            this->on_tap_selected(unmasked_graphlet, local_x, local_y);
                        }

                        handled = info->selected;
                    }
                }; break;
            }
        }; break;
    }

    return handled;
}

bool WarGrey::STEM::Planet::on_scroll(int horizon, int vertical, float hprecise, float vprecise) {
    bool handled = false;

    return handled;
}

bool WarGrey::STEM::Planet::say_goodbye_to_hover_graphlet(uint32_t state, float x, float y, float dx, float dy) {
    bool done = false;

    if (this->hovering_graphlet != nullptr) {
        MatterInfo* info = GRAPHLET_INFO(this->hovering_graphlet);
        float local_x = x - info->x;
        float local_y = y - info->y;

        if (this->hovering_graphlet->handle_events()) {
            done |= this->hovering_graphlet->on_goodbye(local_x, local_y);

            if (this->hovering_graphlet->handle_low_level_events()) {
                done |= this->hovering_graphlet->on_pointer_move(state, local_x, local_y, dx, dy, true);
            }
        }

        this->on_goodbye(this->hovering_graphlet, local_x, local_y);
        this->hovering_graphlet = nullptr;
    }

    return done;
}

/************************************************************************************************/
void WarGrey::STEM::Planet::on_elapse(uint32_t count, uint32_t interval, uint32_t uptime) {
    if (this->head_graphlet != nullptr) {
        IMatter* child = this->head_graphlet;
        float cwidth, cheight, dwidth, dheight;

        do {
            MatterInfo* info = GRAPHLET_INFO(child);
            this->info->master->fill_extent(&dwidth, &dheight);

            if (unsafe_graphlet_unmasked(info, this->mode)) {
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

                        child->fill_extent(info->x, info->y, &cwidth, &cheight);

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
        } while (child != this->head_graphlet);
    }

    this->update(count, interval, uptime);
}

void WarGrey::STEM::Planet::draw(SDL_Renderer* renderer, float X, float Y, float Width, float Height) {
    float dsX = flmax(0.0F, X);
    float dsY = flmax(0.0F, Y);
    float dsWidth = X + Width;
    float dsHeight = Y + Height;
    
    if (this->bg_alpha > 0.0F) {
        game_fill_rect(renderer, X, Y, Width, Height, this->background, this->bg_alpha);
    }

    if (this->head_graphlet != nullptr) {
        IMatter* child = this->head_graphlet;
        float gx, gy, gwidth, gheight;
        SDL_Rect clip;
        
        do {
            MatterInfo* info = GRAPHLET_INFO(child);

            if (unsafe_graphlet_unmasked(info, this->mode)) {
                child->fill_extent(info->x, info->y, &gwidth, &gheight);

                gx = (info->x + this->translate_x) * this->scale_x + X;
                gy = (info->y + this->translate_y) * this->scale_y + Y;
                
                if (rectangle_overlay(gx, gy, gx + gwidth, gy + gheight, dsX, dsY, dsWidth, dsHeight)) {
                    clip.x = flfloor(gx);
                    clip.y = flfloor(gy);
                    clip.w = flceiling(gwidth);
                    clip.h = flceiling(gheight);

                    SDL_RenderSetClipRect(renderer, &clip);
                    child->draw(renderer, gx, gy, gwidth, gheight);

                    if (info->selected) {
                        SDL_RenderSetClipRect(renderer, nullptr);
                        this->draw_visible_selection(renderer, gx, gy, gwidth, gheight);
                    }
                }
            }

            child = info->next;
        } while (child != this->head_graphlet);
                    
        SDL_RenderSetClipRect(renderer, nullptr);
    }
}

void WarGrey::STEM::Planet::draw_visible_selection(SDL_Renderer* renderer, float x, float y, float width, float height) {
    game_draw_rect(renderer, x, y, width, height, 0x00FFFFU);
}

/*************************************************************************************************/
WarGrey::STEM::IPlanet::IPlanet(const char* name) : caption(name) {}
WarGrey::STEM::IPlanet::IPlanet(const std::string& name) : IPlanet(name.c_str()) {}

WarGrey::STEM::IPlanet::~IPlanet() {
    if (this->info != nullptr) {
        delete this->info;
        this->info = nullptr;
    }
}

const char* WarGrey::STEM::IPlanet::name() {
    return this->caption.c_str();
}

IScreen* WarGrey::STEM::IPlanet::master() {
    IScreen* screen = nullptr;

    if (this->info != nullptr) {
        screen = this->info->master;
    }

    return screen;
}

void WarGrey::STEM::IPlanet::fill_background(SDL_Color* c) {
    RGB_FillColor(c, this->background, this->bg_alpha);
}

void WarGrey::STEM::IPlanet::start_input_text(const char* fmt, ...) {
    if (this->info != nullptr) {
        VSNPRINT(prompt, fmt);
        this->start_input_text(prompt);
    }
}

void WarGrey::STEM::IPlanet::start_input_text(const std::string& prompt) {
    if (this->info != nullptr) {
        this->info->master->start_input_text(prompt);
    }
}

void WarGrey::STEM::IPlanet::log_message(const char* fmt, ...) {
    if (this->info != nullptr) {
        VSNPRINT(msg, fmt);
        this->log_message(-1, msg);
    }
}

void WarGrey::STEM::IPlanet::log_message(int fgc, const char* fmt, ...) {
    if (this->info != nullptr) {
        VSNPRINT(msg, fmt);
        this->log_message(fgc, msg);
    }
}

void WarGrey::STEM::IPlanet::log_message(int fgc, const std::string& msg) {
    if (this->info != nullptr) {
        this->info->master->log_message(fgc, msg);
    }
}

void WarGrey::STEM::IPlanet::begin_update_sequence() {
    if (this->info != nullptr) {
        this->info->master->begin_update_sequence();
    }
}

bool WarGrey::STEM::IPlanet::in_update_sequence() {
    return ((this->info != nullptr) && this->info->master->in_update_sequence());
}

void WarGrey::STEM::IPlanet::end_update_sequence() {
    if (this->info != nullptr) {
        this->info->master->end_update_sequence();
    }
}

bool WarGrey::STEM::IPlanet::needs_update() {
    return ((this->info != nullptr) && this->info->master->needs_update());
}

void WarGrey::STEM::IPlanet::notify_updated() {
    if (this->info != nullptr) {
        this->info->master->notify_updated();
    }
}

SDL_Surface* WarGrey::STEM::IPlanet::snapshot(float width, float height, uint32_t bgcolor, float alpha) {
    return this->snapshot(0.0F, 0.0F, width, height, bgcolor, alpha);
}

SDL_Surface* WarGrey::STEM::IPlanet::snapshot(float x, float y, float width, float height, uint32_t bgcolor, float alpha) {
    static SDL_Surface* photograph = nullptr;
    SDL_Renderer* renderer = nullptr;
    int saved_bgc = this->background;
    float saved_alpha = this->bg_alpha;

    if (photograph != nullptr) {
        SDL_FreeSurface(photograph);
    }

    if (x != 0.0F) width += x;
    if (y != 0.0F) height += y;

    photograph = game_blank_image(width, height);

    if (photograph != nullptr) {
        renderer = SDL_CreateSoftwareRenderer(photograph);
        
        if (renderer != nullptr) {
            this->background = bgcolor;
            this->bg_alpha = alpha;
            this->draw(renderer, -x, -y, width, height);
            SDL_RenderPresent(renderer);
            SDL_DestroyRenderer(renderer);
        }
    }

    this->background = saved_bgc;
    this->bg_alpha = saved_alpha;

    return photograph;
}

bool WarGrey::STEM::IPlanet::save_snapshot(const std::string& png, float width, float height, uint32_t bgcolor, float alpha) {
    return this->save_snapshot(png.c_str(), 0.0F, 0.0F, width, height, bgcolor, alpha);
}

bool WarGrey::STEM::IPlanet::save_snapshot(const char* png, float width, float height, uint32_t bgcolor, float alpha) {
    return this->save_snapshot(png, 0.0F, 0.0F, width, height, bgcolor, alpha);
}

bool WarGrey::STEM::IPlanet::save_snapshot(const std::string& png, float x, float y, float width, float height, uint32_t bgcolor, float alpha) {
    return this->save_snapshot(png.c_str(), x, y, width, height, bgcolor, alpha);
}

bool WarGrey::STEM::IPlanet::save_snapshot(const char* png, float x, float y, float width, float height, uint32_t bgcolor, float alpha) {
    return game_save_image(this->snapshot(x, y, width, height, bgcolor, alpha), png);
}

bool WarGrey::STEM::IPlanet::fill_graphlet_location(IMatter* g, float* x, float* y, MatterAnchor a) {
    float fx, fy;

    graphlet_anchor_fraction(a, &fx, &fy);

    return this->fill_graphlet_location(g, x, y, fx, fy);
}

void WarGrey::STEM::IPlanet::insert(IMatter* g, float x, float y, MatterAnchor a, float dx, float dy) {
    float fx, fy;

    graphlet_anchor_fraction(a, &fx, &fy);

    this->insert(g, x, y, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlanet::insert(IMatter* g, IMatter* target, MatterAnchor ta, MatterAnchor a, float dx, float dy) {
    float tfx, tfy, fx, fy;

    graphlet_anchor_fraction(ta, &tfx, &tfy);
    graphlet_anchor_fraction(a, &fx, &fy);

    this->insert(g, target, tfx, tfy, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlanet::insert(IMatter* g, IMatter* target, float tfx, float tfy, MatterAnchor a, float dx, float dy) {
    float fx, fy;

    graphlet_anchor_fraction(a, &fx, &fy);

    this->insert(g, target, tfx, tfy, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlanet::insert(IMatter* g, IMatter* target, MatterAnchor ta, float fx, float fy, float dx, float dy) {
    float tfx, tfy;

    graphlet_anchor_fraction(ta, &tfx, &tfy);
    
    this->insert(g, target, tfx, tfy, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlanet::insert(IMatter* g, IMatter* xtarget, float xfx, IMatter* ytarget, float yfy, MatterAnchor a, float dx, float dy) {
    float fx, fy;

    graphlet_anchor_fraction(a, &fx, &fy);

    this->insert(g, xtarget, xfx, ytarget, yfy, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlanet::move_to(IMatter* g, float x, float y, MatterAnchor a, float dx, float dy) {
    float fx, fy;

    graphlet_anchor_fraction(a, &fx, &fy);

    this->move_to(g, x, y, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlanet::move_to(IMatter* g, IMatter* target, MatterAnchor ta, MatterAnchor a, float dx, float dy) {
    float tfx, tfy, fx, fy;

    graphlet_anchor_fraction(ta, &tfx, &tfy);
    graphlet_anchor_fraction(a, &fx, &fy);

    this->move_to(g, target, tfx, tfy, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlanet::move_to(IMatter* g, IMatter* target, float tfx, float tfy, MatterAnchor a, float dx, float dy) {
    float fx, fy;

    graphlet_anchor_fraction(a, &fx, &fy);

    this->move_to(g, target, tfx, tfy, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlanet::move_to(IMatter* g, IMatter* target, MatterAnchor ta, float fx, float fy, float dx, float dy) {
    float tfx, tfy;

    graphlet_anchor_fraction(ta, &tfx, &tfy);

    this->move_to(g, target, tfx, tfy, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlanet::move_to(IMatter* g, IMatter* xtarget, float xfx, IMatter* ytarget, float yfy, MatterAnchor a, float dx, float dy) {
    float fx, fy;
    
    graphlet_anchor_fraction(a, &fx, &fy);

    this->move_to(g, xtarget, xfx, ytarget, yfy, fx, fy, dx, dy);
}

