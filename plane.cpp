#include "plane.hpp"
#include "matter.hpp"
#include "misc.hpp"

#include "graphics/image.hpp"
#include "graphics/geometry.hpp"
#include "graphics/colorspace.hpp"

#include "matter/sprite.hpp"
#include "physics/mathematics.hpp"
#include "physics/random.hpp"

#include "datum/string.hpp"
#include "datum/flonum.hpp"
#include "datum/fixnum.hpp"
#include "datum/box.hpp"
#include "datum/time.hpp"

#include <deque>

using namespace WarGrey::STEM;

/** NOTE
 *   C-Style casting tries all C++ style casting except dynamic_cast;
 *   `static_cast` almost costs nothing for subtype casting;
 *   `reinterpret_cast` might be harmful for multi-inheritances.
 */
#define MATTER_INFO(m) (static_cast<MatterInfo*>(m->info))

namespace WarGrey::STEM {
    struct AsyncInfo {
        double second;
        float x0;
        float y0;
        float fx0;
        float fy0;
        float dx0;
        float dy0;
    };

    struct GlidingMotion {
        float target_x;
        float target_y;
        double second;
        double sec_delta;
        bool absolute;
    };

    struct MatterInfo : public WarGrey::STEM::IMatterInfo {
        MatterInfo(WarGrey::STEM::IPlane* master, unsigned int mode) : IMatterInfo(master), mode(mode) {};

        float x = 0.0F;
        float y = 0.0F;
        bool selected = false;
        unsigned int mode = 0U;

        uint32_t local_frame_delta = 0U;
        uint32_t local_frame_count = 0U;
        uint32_t local_elapse = 0U;
        int duration = 0;

        // for queued motions
        bool gliding = false;
        float gliding_tx = 0.0F;
        float gliding_ty = 0.0F;
        std::deque<GlidingMotion> motion_queues;

        // progressbar
        double current_step = 1.0;
        double progress_total = 1.0;

        // for asynchronously loaded matters
        AsyncInfo* async = nullptr;

        IMatter* next = nullptr;
        IMatter* prev = nullptr;
    };
}

static inline bool over_stepped(float tx, float cx, double spd) {
    return flsign(double(tx - cx)) != flsign(spd);
}

static inline void reset_timeline(uint32_t& frame_count, uint32_t& elapse, uint32_t count0) {
    elapse = 0U;
    frame_count = count0;
}

static inline void unsafe_set_local_fps(int fps, bool restart, uint32_t& frame_delta, uint32_t& frame_count, uint32_t& elapse) {
    frame_delta = (fps > 0) ? (1000U / fps) : 0U;

    if (restart) {
        reset_timeline(frame_count, elapse, 0U);
    }
}

static inline void unsafe_set_matter_fps(MatterInfo* info, int fps, bool restart) {
    unsafe_set_local_fps(fps, restart, info->local_frame_delta, info->local_frame_count, info->local_elapse);
}

static uint32_t local_timeline_elapse(uint32_t global_interval, uint32_t local_frame_delta, uint32_t& local_elapse, int duration) {
    uint32_t interval = 0;

    if ((local_frame_delta > 0) || (duration > 0)) {
        if (local_elapse < ((duration > 0) ? duration : local_frame_delta)) {
            local_elapse += global_interval;
        } else {
            interval = local_elapse;
            local_elapse = 0U;
        }
    } else {
        interval = global_interval;
    }

    return interval;
}

static inline MatterInfo* bind_matter_owership(IPlane* master, unsigned int mode, IMatter* m) {
    auto info = new MatterInfo(master, mode);
    
    unsafe_set_matter_fps(info, m->preferred_local_fps(), true);
    m->info = info;

    return info;
}

static inline MatterInfo* plane_matter_info(IPlane* master, IMatter* m) {
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

/*************************************************************************************************/
Plane::Plane(const char* name, unsigned int initial_mode)
    : IPlane(name), head_matter(nullptr), mode(initial_mode) {}

Plane::Plane(const std::string& name, unsigned int initial_mode)
    : Plane(name.c_str(), initial_mode) {}

Plane::~Plane() {
    this->erase();
}

void WarGrey::STEM::Plane::shift_to_mode(unsigned int mode) {
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

            this->glide_matter_via_info(m, info,
                info->async->second,
                info->async->x0, info->async->y0,
                info->async->fx0, info->async->fy0,
                info->async->dx0, info->async->dy0);

            if ((this->scale_x != 1.0F) || (this->scale_y != 1.0F)) {
                this->do_resize(m, info, info->async->fx0, info->async->fy0, this->scale_x, this->scale_y);
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
        SDL_Renderer* master_renderer = this->master_renderer();

        if (this->head_matter == nullptr) {
            this->head_matter = m;
            info->prev = this->head_matter;
            info->next = this->head_matter;
        } else if (this->tooltip == nullptr) {
            MatterInfo* head_info = MATTER_INFO(this->head_matter);
            MatterInfo* prev_info = MATTER_INFO(head_info->prev);
            
            info->prev = head_info->prev;
            info->next = this->head_matter;
            prev_info->next = m;
            head_info->prev = m;
        } else {
            MatterInfo* tool_info = MATTER_INFO(this->tooltip);
            MatterInfo* prev_info = MATTER_INFO(tool_info->prev);

            info->prev = tool_info->prev;
            info->next = this->tooltip;
            prev_info->next = m;
            tool_info->prev = m;
        }

        this->begin_update_sequence();
        m->construct(master_renderer);
        this->move_matter_via_info(m, info, x, y, fx, fy, dx, dy);

        if (m->ready()) {
            if ((this->scale_x != 1.0F) || (this->scale_y != 1.0F)) {
                this->do_resize(m, info, fx, fy, this->scale_x, this->scale_y);
            }

            this->notify_updated();
            this->on_matter_ready(m);
        } else {
            this->notify_updated(); // is it necessary?
        }
        this->end_update_sequence();
    }
}

void WarGrey::STEM::Plane::remove(IMatter* m, bool needs_delete) {
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
        
        if (needs_delete) {
            delete m; // m's destructor will delete the associated info object
        }

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

void WarGrey::STEM::Plane::move(IMatter* m, float x, float y, bool ignore_gliding) {
    if (m != nullptr) {
        MatterInfo* info = plane_matter_info(this, m);

        if ((info != nullptr) && unsafe_matter_unmasked(info, this->mode)) {
            if (this->move_matter_via_info(m, info, x, y, false, ignore_gliding)) {
                this->notify_updated();
            }
        }
    } else if (this->head_matter != nullptr) {
        IMatter* child = this->head_matter;

        do {
            MatterInfo* info = MATTER_INFO(child);

            if (info->selected && unsafe_matter_unmasked(info, this->mode)) {
                this->move_matter_via_info(m, info, x, y, false, ignore_gliding);
            }

            child = info->next;
        } while (child != this->head_matter);

        this->notify_updated();
    }
}

void WarGrey::STEM::Plane::move_to(IMatter* m, float x, float y, float fx, float fy, float dx, float dy) {
    MatterInfo* info = plane_matter_info(this, m);
    
    if ((info != nullptr) && unsafe_matter_unmasked(info, this->mode)) {
        if (this->move_matter_via_info(m, info, x, y, fx, fy, dx, dy)) {
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

void WarGrey::STEM::Plane::glide(double sec, IMatter* m, float x, float y) {
    if (m != nullptr) {
        MatterInfo* info = plane_matter_info(this, m);

        if ((info != nullptr) && unsafe_matter_unmasked(info, this->mode)) {
            this->glide_matter_via_info(m, info, sec, x, y, false);
        }
    } else if (this->head_matter != nullptr) {
        IMatter* child = this->head_matter;

        do {
            MatterInfo* info = MATTER_INFO(child);

            if (info->selected && unsafe_matter_unmasked(info, this->mode)) {
                this->glide_matter_via_info(m, info, sec, x, y, false);
            }

            child = info->next;
        } while (child != this->head_matter);
    }
}

void WarGrey::STEM::Plane::glide_to(double sec, IMatter* m, float x, float y, float fx, float fy, float dx, float dy) {
    MatterInfo* info = plane_matter_info(this, m);
    
    if ((info != nullptr) && unsafe_matter_unmasked(info, this->mode)) {
        this->glide_matter_via_info(m, info, sec, x, y, fx, fy, dx, dy);
    }
}

void WarGrey::STEM::Plane::glide_to(double sec, IMatter* m, IMatter* target, float tfx, float tfy, float fx, float fy, float dx, float dy) {
    MatterInfo* tinfo = plane_matter_info(this, target);
    float x = 0.0F;
    float y = 0.0F;

    if ((tinfo != nullptr) && unsafe_matter_unmasked(tinfo, this->mode)) {
        float tsx, tsy, tsw, tsh;

        unsafe_feed_matter_bound(target, tinfo, &tsx, &tsy, &tsw, &tsh);
        x = tsx + tsw * tfx;
        y = tsy + tsh * tfy;
    }
        
    this->glide_to(sec, m, x, y, fx, fy, dx, dy);
}

void WarGrey::STEM::Plane::glide_to(double sec, IMatter* m, IMatter* xtarget, float xfx, IMatter* ytarget, float yfy, float fx, float fy, float dx, float dy) {
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

    this->glide_to(sec, m, x, y, fx, fy, dx, dy);
}

IMatter* WarGrey::STEM::Plane::find_matter_including_camouflaged_ones(float x, float y) {
    IMatter* found = nullptr;

    if (this->head_matter != nullptr) {
        MatterInfo* head_info = MATTER_INFO(this->head_matter);
        IMatter* child = head_info->prev;

        do {
            MatterInfo* info = MATTER_INFO(child);

            if (unsafe_matter_unmasked(info, this->mode)) {
                if (child->visible()) {
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

IMatter* WarGrey::STEM::Plane::find_matter(float x, float y) {
    IMatter* found = this->find_matter_including_camouflaged_ones(x, y);

    return ((found == nullptr) || found->concealled()) ? nullptr : found;
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
        SET_VALUES(x, sx, y, sy);
        SET_VALUES(width, sw, height, sh);

        okay = true;
    }

    return okay;
}

void WarGrey::STEM::Plane::feed_matters_boundary(float* x, float* y, float* width, float* height) {
    this->recalculate_matters_extent_when_invalid();

    SET_VALUES(x, this->matters_left, y, this->matters_top);
    SET_BOX(width, this->matters_right - this->matters_left);
    SET_BOX(height, this->matters_bottom - this->matters_top);
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

size_t WarGrey::STEM::Plane::count_selected() {
    size_t n = 0U;

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

                if ((this->tooltip != nullptr) && (this->tooltip->visible())) {
                    this->update_tooltip(m, local_x, local_y, local_x + info->x, local_y + info->y);
                    this->place_tooltip(m);
                }
            } else {
                this->no_selected();
            }
        }
    }
}

bool WarGrey::STEM::Plane::on_pointer_pressed(uint8_t button, float x, float y, uint8_t clicks) {
    bool handled = false;

    switch (button) {
        case SDL_BUTTON_LEFT: {
            IMatter* unmasked_matter = this->find_matter(x, y);

            if (unmasked_matter != nullptr) {
                MatterInfo* info = MATTER_INFO(unmasked_matter);

                if (!info->selected) {
                    this->set_caret_owner(unmasked_matter);
                    this->no_selected();
                }
                
                if (unmasked_matter->low_level_events_allowed()) {
                    float local_x = x - info->x;
                    float local_y = y - info->y;

                    handled = unmasked_matter->on_pointer_pressed(button, local_x, local_y, clicks);
                }
            } else {
                this->set_caret_owner(nullptr);
                this->no_selected();
            }
        }; break;
    }

    return handled;
}

bool WarGrey::STEM::Plane::on_pointer_move(uint32_t state, float x, float y, float dx, float dy) {
    bool handled = false;

    if (state == 0) {
        IMatter* unmasked_matter = this->find_matter_including_camouflaged_ones(x, y);

        if ((unmasked_matter == nullptr) || (unmasked_matter != this->hovering_matter)) {
            if ((unmasked_matter != nullptr) && !unmasked_matter->concealled()) {
                this->say_goodbye_to_hover_matter(state, x, y, dx, dy);
            }

            if ((this->tooltip != nullptr) && (this->tooltip != unmasked_matter) && this->tooltip->visible()) {
                this->tooltip->show(false);
            }
        }

        if (unmasked_matter != nullptr) {
            MatterInfo* info = MATTER_INFO(unmasked_matter);
            float local_x = x - info->x;
            float local_y = y - info->y;

            if (!unmasked_matter->concealled()) {
                this->hovering_matter = unmasked_matter;
                this->hovering_mgx = x;
                this->hovering_mgy = y;
                this->hovering_mlx = local_x;
                this->hovering_mly = local_y;

                if (unmasked_matter->events_allowed()) {
                    unmasked_matter->on_hover(local_x, local_y);

                    if (unmasked_matter->low_level_events_allowed()) {
                        unmasked_matter->on_pointer_move(state, local_x, local_y, dx, dy, false);
                    }
                }

                this->on_hover(this->hovering_matter, local_x, local_y);
                handled = true;
            }

            if (this->tooltip != nullptr) {
                if (this->update_tooltip(unmasked_matter, local_x, local_y, x, y)) {
                    if (!this->tooltip->visible()) {
                        this->tooltip->show(true);
                    }

                    this->place_tooltip(unmasked_matter);
                }
            }
        }
    }

    return handled;
}

bool WarGrey::STEM::Plane::on_pointer_released(uint8_t button, float x, float y, uint8_t clicks) {
    bool handled = false;

    switch (button) {
        case SDL_BUTTON_LEFT: {
            IMatter* unmasked_matter = this->find_matter(x, y);

            if (unmasked_matter != nullptr) {
                MatterInfo* info = MATTER_INFO(unmasked_matter);
                float local_x = x - info->x;
                float local_y = y - info->y;

                if (unmasked_matter->events_allowed()) {
                    if (clicks == 1) {
                        unmasked_matter->on_tap(local_x, local_y);
                    }

                    if (unmasked_matter->low_level_events_allowed()) {
                        unmasked_matter->on_pointer_released(button, local_x, local_y, clicks);
                    }
                }

                if (clicks == 1) {
                    if (info->selected) {
                        this->on_tap_selected(unmasked_matter, local_x, local_y);
                    } else {
                        this->on_tap(unmasked_matter, local_x, local_y);
                    }

                    handled = info->selected;
                } else {
                    if ((unmasked_matter == this->sentry) && (this->can_select(unmasked_matter))) {
                        this->on_double_tap_sentry_sprite(this->sentry);
                    }
                }
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

void WarGrey::STEM::Plane::on_enter(IPlane* from) {
    this->mission_done = false;

    if (this->sentry != nullptr) {
        this->sentry->greetings(1);
    }

    IPlane::on_enter(from);
}

void WarGrey::STEM::Plane::mission_complete() {
    if (this->sentry != nullptr) {
        this->sentry->goodbye(1);
        this->sentry->stop(1);
    }

    this->on_mission_complete();
    this->mission_done = true;
}

bool WarGrey::STEM::Plane::has_mission_completed() {
    return this->mission_done &&
            ((this->sentry == nullptr) || !this->sentry->in_playing());
}

bool WarGrey::STEM::Plane::can_select(IMatter* m) {
    return this->sentry == m;
}

void WarGrey::STEM::Plane::set_tooltip_matter(IMatter* m, float dx, float dy) {
    this->begin_update_sequence();

    if ((this->tooltip != nullptr) && !this->tooltip->visible()) {
        this->tooltip->show(true);
    }

    this->tooltip = m;
    this->tooltip->show(false);
    this->tooltip_dx = dx;
    this->tooltip_dy = dy;

    this->end_update_sequence();
}

void WarGrey::STEM::Plane::place_tooltip(WarGrey::STEM::IMatter* target) {
    float ttx, tty, width, height;

    this->move_to(this->tooltip, target,
        MatterAnchor::LB, MatterAnchor::LT,
        this->tooltip_dx, this->tooltip_dy);

    this->master()->feed_client_extent(&width, &height);
    this->feed_matter_location(this->tooltip, &ttx, &tty, MatterAnchor::LB);

    if (tty > height) {
        this->move_to(this->tooltip, target,
            MatterAnchor::LT, MatterAnchor::LB,
            this->tooltip_dx, this->tooltip_dy);
    }

    if (ttx < 0.0F) {
        this->move(this->tooltip, -ttx, 0.0F);
    } else {
        this->feed_matter_location(this->tooltip, &ttx, &tty, MatterAnchor::RB);

        if (ttx > width) {
            this->move(this->tooltip, width - ttx, 0.0F);
        }
    }
}

/************************************************************************************************/
void WarGrey::STEM::Plane::set_matter_fps(IMatter* m, int fps, bool restart) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        unsafe_set_matter_fps(info, fps, restart);
    }
}

void WarGrey::STEM::Plane::set_local_fps(int fps, bool restart) {
    unsafe_set_local_fps(fps, restart, this->local_frame_delta, this->local_frame_count, this->local_elapse);
}


void WarGrey::STEM::Plane::notify_matter_timeline_restart(IMatter* m, uint32_t count0, int duration) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        info->duration = duration;
        reset_timeline(info->local_frame_count, info->local_elapse, count0);
    }
}

void WarGrey::STEM::Plane::on_elapse(uint64_t count, uint32_t interval, uint64_t uptime) {
    uint32_t elapse = 0U;

    if (this->head_matter != nullptr) {
        IMatter* child = this->head_matter;
        float dwidth, dheight;

        this->info->master->feed_client_extent(&dwidth, &dheight);

        do {
            MatterInfo* info = MATTER_INFO(child);
            
            if (unsafe_matter_unmasked(info, this->mode)) {
                elapse = local_timeline_elapse(interval, info->local_frame_delta, info->local_elapse, info->duration);
                
                if (elapse > 0U) {
                    info->duration = child->update(info->local_frame_count ++, elapse, uptime);
                }

                /* controlling motion via global timeline makes it more smooth */
                this->do_motion_moving(child, info, dwidth, dheight);
            }
            
            child = info->next;
        } while (child != this->head_matter);
    }

    elapse = local_timeline_elapse(interval, this->local_frame_delta, this->local_elapse, 0);
    if (elapse > 0U) {
        this->update(this->local_frame_count ++, elapse, uptime);

        if ((this->tooltip != nullptr) && this->tooltip->visible()) {
            if (this->hovering_matter != nullptr) {
                this->update_tooltip(this->hovering_matter, this->hovering_mlx, this->hovering_mly, this->hovering_mgx, this->hovering_mgy);
                this->place_tooltip(this->hovering_matter);
            }
        }
    }
}

void WarGrey::STEM::Plane::draw(SDL_Renderer* renderer, float X, float Y, float Width, float Height) {
    float dsX = flmax(0.0F, X);
    float dsY = flmax(0.0F, Y);
    float dsWidth = X + Width;
    float dsHeight = Y + Height;
    
    if (this->bg_alpha > 0.0F) {
        game_fill_rect(renderer, dsX, dsY, dsWidth, dsHeight, this->background, this->bg_alpha);
    }

    if ((this->grid_alpha > 0.0F)
            && (this->column > 0) && (this->row > 0)
            && (this->cell_width > 0.0F) && (this->cell_height > 0.0F)) {
        RGB_SetRenderDrawColor(renderer, this->grid_color, this->grid_alpha);
        game_draw_grid(renderer, this->row, this->column,
                        this->cell_width, this->cell_height,
                        this->grid_x, this->grid_y);
    }

    if (this->head_matter != nullptr) {
        IMatter* child = this->head_matter;
        float mx, my, mwidth, mheight;
        SDL_Rect clip;
        
        do {
            MatterInfo* info = MATTER_INFO(child);

            if (unsafe_matter_unmasked(info, this->mode) && child->visible()) {
                child->feed_extent(info->x, info->y, &mwidth, &mheight);

                mx = (info->x + this->translate_x) * this->scale_x + X;
                my = (info->y + this->translate_y) * this->scale_y + Y;
                
                if (rectangle_overlay(mx, my, mx + mwidth, my + mheight, dsX, dsY, dsWidth, dsHeight)) {
                    clip.x = fl2fxi(flfloor(mx));
                    clip.y = fl2fxi(flfloor(my));
                    clip.w = fl2fxi(flceiling(mwidth));
                    clip.h = fl2fxi(flceiling(mheight));

                    SDL_RenderSetClipRect(renderer, &clip);

                    if (child->ready()) {
                        child->draw(renderer, mx, my, mwidth, mheight);
                    } else {
                        child->draw_in_progress(renderer, mx, my, mwidth, mheight);
                    }

                    if (info->selected) {
                        SDL_RenderSetClipRect(renderer, nullptr);
                        this->draw_visible_selection(renderer, mx, my, mwidth, mheight);
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

void WarGrey::STEM::Plane::do_resize(IMatter* m, MatterInfo* info, float fx, float fy, float scale_x, float scale_y, float prev_scale_x, float prev_scale_y) {
    // TODO: the theory or implementation seems incorrect.
    if (m->resizable()) {
        float sx, sy, sw, sh, nx, ny, nw, nh;

        unsafe_feed_matter_bound(m, info, &sx, &sy, &sw, &sh);

        m->resize((sw / prev_scale_x) * scale_x, (sh / prev_scale_y) * scale_y);
        m->feed_extent(sx, sy, &nw, &nh);

        nx = sx + (sw - nw) * fx;
        ny = sy + (sh - nh) * fy;

        this->do_moving_via_info(m, info, nx, ny, true);
    }
}

/*************************************************************************************************/
bool WarGrey::STEM::Plane::do_moving_via_info(IMatter* m, MatterInfo* info, float x, float y, bool absolute) {
    bool moved = false;
    
    if (!absolute) {
        x += info->x;
        y += info->y;
    }    
    
    if ((info->x != x) || (info->y != y)) {
        float ox = info->x;
        float oy = info->y;

        info->x = x;
        info->y = y;

        m->on_location_changed(info->x, info->y, ox, oy);
        this->size_cache_invalid();
        moved = true;
    }

    return moved;
}

bool WarGrey::STEM::Plane::do_gliding_via_info(IMatter* m, MatterInfo* info, float x, float y, double sec, double sec_delta, bool absolute) {
    bool moved = false;
    
    if (!absolute) {
        x += info->x;
        y += info->y;
    }    

    if ((info->x != x) || (info->y != y)) {
        /** WARNING
         * Meanwhile the gliding time is not accurate
         * `flfloor` makes it more accurate than `flceiling`
         **/
        double n = flfloor(sec / sec_delta);
        float dx = x - info->x;
        float dy = y - info->y;
        double xspd = dx / n;
        double yspd = dy / n;

        m->set_delta_speed(0.0, 0.0);
        m->set_speed(xspd, yspd);
                
        info->gliding = true;
        info->gliding_tx = x;
        info->gliding_ty = y;
        info->current_step = 1.0;
        info->progress_total = n;

        this->on_motion_start(m, sec, info->x, info->y, xspd, yspd);
        m->step(&info->x, &info->y);
        this->on_motion_step(m, info->x, info->y, xspd, yspd, info->current_step / info->progress_total);
        m->on_location_changed(info->x, info->y, x - dx, y - dy);
        this->size_cache_invalid();
        moved = true;
    }

    return moved;
}

bool WarGrey::STEM::Plane::move_matter_via_info(IMatter* m, MatterInfo* info, float x, float y, bool absolute, bool ignore_gliding) {
    bool moved = false;

    if ((!info->gliding) || (m == this->tooltip) || ignore_gliding) {
        moved = this->do_moving_via_info(m, info, x, y, absolute);
    } else {
        if (info->motion_queues.empty()) {
            info->motion_queues.push_back( { x, y, 0.0F, 0.0F, absolute } );
        } else {
            auto back = info->motion_queues.back();

            if (back.second == 0.0F) {
                back.target_x = x;
                back.target_y = y;
                back.absolute = absolute;
            } else {
                info->motion_queues.push_back( { x, y, 0.0F, 0.0F, absolute } );
            }
        }
    }

    return moved;
}

bool WarGrey::STEM::Plane::glide_matter_via_info(IMatter* m, MatterInfo* info, double sec, float x, float y, bool absolute) {
    bool moved = false;
    
    if (sec <= 0.0F) {
        moved = this->move_matter_via_info(m, info, x, y, absolute, false);
    } else {
        IScreen* screen = this->master();
        double sec_delta = (screen != nullptr) ? (1.0 / double(screen->frame_rate())) : 0.0;

        if ((sec <= sec_delta) || (sec_delta == 0.0)) {
            moved = this->move_matter_via_info(m, info, x, y, absolute, false);
        } else {
            if (m->motion_stopped()) {
                info->motion_queues.clear();
                moved = this->do_gliding_via_info(m, info, x, y, sec, sec_delta, absolute);
            } else if (!info->gliding) {
                moved = this->do_gliding_via_info(m, info, x, y, sec, sec_delta, absolute);
            } else {
                info->motion_queues.push_back({ x, y, sec, sec_delta, absolute });
            }
        }
    }

    return moved;
}

bool WarGrey::STEM::Plane::glide_matter_via_info(IMatter* m, MatterInfo* info, double sec, float x, float y, float fx, float fy, float dx, float dy) {
    float ax = 0.0F;
    float ay = 0.0F;
    
    if (m->ready()) {
        float sx, sy, sw, sh;
        
        unsafe_feed_matter_bound(m, info, &sx, &sy, &sw, &sh);
        ax = (sw * fx);
        ay = (sh * fy);
    } else {
        info->async = new AsyncInfo();

        info->async->second = sec;
        info->async->x0 = x;
        info->async->y0 = y;
        info->async->fx0 = fx;
        info->async->fy0 = fy;
        info->async->dx0 = dx;
        info->async->dy0 = dy;
    }
    
    return this->glide_matter_via_info(m, info, sec, x - ax + dx, y - ay + dy, true);
}

bool WarGrey::STEM::Plane::move_matter_via_info(IMatter* m, MatterInfo* info, float x, float y, float fx, float fy, float dx, float dy) {
    return this->glide_matter_via_info(m, info, 0.0F, x, y, fx, fy, dx, dy);
}

void WarGrey::STEM::Plane::do_motion_moving(IMatter* m, MatterInfo* info, float dwidth, float dheight) {
    if (!m->motion_stopped()) {
        float cwidth, cheight, hdist, vdist;
        double xspd = m->x_speed();
        double yspd = m->y_speed();
        float ox = info->x;
        float oy = info->y;
        
        m->step(&info->x, &info->y);
        
        if (info->gliding) {
            if (over_stepped(info->gliding_tx, info->x, xspd)
                    || over_stepped(info->gliding_ty, info->y, yspd)) {
                info->x = info->gliding_tx;
                info->y = info->gliding_ty;
                this->on_motion_step(m, info->x, info->y, xspd, yspd, 1.0);
                m->motion_stop();
                info->gliding = false;
                this->on_motion_complete(m, info->x, info->y, xspd, yspd);
            } else {
                info->current_step += 1.0F;
                this->on_motion_step(m, info->x, info->y, xspd, yspd, info->current_step / info->progress_total);
            }
        }

        m->feed_extent(info->x, info->y, &cwidth, &cheight);

        if (info->x < 0.0F) {
            hdist = info->x;
        } else if (info->x + cwidth > dwidth) {
            hdist = info->x + cwidth - dwidth;
        } else {
            hdist = 0.0F;
        }

        if (info->y < 0.0F) {
            vdist = info->y;
        } else if (info->y + cheight > dheight) {
            vdist = info->y + cheight - dheight;
        } else {
            vdist = 0.0F;
        }

        if ((hdist != 0.0F) || (vdist != 0.0F)) {
            m->on_border(hdist, vdist);
                        
            if (m->x_stopped()) {
                if (info->x < 0.0F) {
                    info->x = 0.0F;
                } else if (info->x + cwidth > dwidth) {
                    info->x = dwidth - cwidth;
                }
            }

            if (m->y_stopped()) {
                if (info->y < 0.0F) {
                    info->y = 0.0F;
                } else if (info->y + cheight > dheight) {
                    info->y = dheight - cheight;
                }
            }
        }

        // TODO: dealing with bounce and glide
        if (info->gliding && m->motion_stopped()) {
            info->gliding = false;
        }

        if ((info->x != ox) || (info->y != oy)) {
            m->on_location_changed(info->x, info->y, ox, oy);
            this->size_cache_invalid();
            this->notify_updated();
        }
    } else {
        while (!info->motion_queues.empty()) {
            GlidingMotion gm = info->motion_queues.front();

            info->motion_queues.pop_front();

            if (gm.second > 0.0F) {
                if (this->do_gliding_via_info(m, info, gm.target_x, gm.target_y, gm.second, gm.sec_delta, gm.absolute)) {
                    this->notify_updated();
                    break;
                }
            } else if (this->do_moving_via_info(m, info, gm.target_x, gm.target_y, gm.absolute)) {
                this->notify_updated();
            }
        }
    }
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

SDL_Renderer* WarGrey::STEM::IPlane::master_renderer() {
    SDL_Renderer* renderer = nullptr;
    IScreen* screen = this->master();
    
    if (screen != nullptr) {
        renderer = screen->display()->master_renderer();
    }

    return renderer;
}

void WarGrey::STEM::IPlane::on_enter(IPlane* from) {
    /* It's good to tell the mission the size of the stage */
    float width, height;

    this->master()->feed_client_extent(&width, &height);
    this->on_mission_start(width, height);
}

void WarGrey::STEM::IPlane::set_background(double hue, double saturation, double brightness, double alpha) {
    this->set_background(Hexadecimal_From_HSV(hue, saturation, brightness), alpha);
}

uint32_t WarGrey::STEM::IPlane::get_background(double* alpha) {
    SET_BOX(alpha, this->bg_alpha);
    
    return this->background;
}

void WarGrey::STEM::IPlane::start_input_text(const char* fmt, ...) {
    if (this->info != nullptr) {
        VSNPRINT(prompt, fmt);
        this->info->master->start_input_text(prompt);
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

void WarGrey::STEM::IPlane::notify_updated(IMatter* m) {
    if (this->info != nullptr) {
        this->info->master->notify_updated();
    }
}

bool WarGrey::STEM::IPlane::is_colliding(IMatter* m, IMatter* target) {
    float slx, sty, sw, sh, tlx, tty, tw, th;
    bool sokay = this->feed_matter_boundary(m, &slx, &sty, &sw, &sh);
    bool tokay = this->feed_matter_boundary(target, &tlx, &tty, &tw, &th);

    return sokay && tokay
        && rectangle_overlay(slx, sty, slx + sw, sty + sh,
                             tlx, tty, tlx + tw, tty + th);
}

bool WarGrey::STEM::IPlane::is_colliding(IMatter* m, IMatter* target, float fx, float fy) {
    float slx, sty, sw, sh, tx, ty;
    bool sokay = this->feed_matter_boundary(m, &slx, &sty, &sw, &sh);
    bool tokay = this->feed_matter_location(target, &tx, &ty, fx, fy);

    return sokay && tokay
        && rectangle_contain(slx, sty, slx + sw, sty + sh, tx, ty);
}

bool WarGrey::STEM::IPlane::is_colliding(IMatter* m, IMatter* target, MatterAnchor a) {
    float fx, fy;

    matter_anchor_fraction(a, &fx, &fy);
    
    return this->is_colliding(m, target, fx, fy);
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

void WarGrey::STEM::IPlane::glide_to(double sec, IMatter* m, float x, float y, MatterAnchor a, float dx, float dy) {
    float fx, fy;

    matter_anchor_fraction(a, &fx, &fy);
    this->glide_to(sec, m, x, y, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlane::glide_to(double sec, IMatter* m, IMatter* target, MatterAnchor ta, MatterAnchor a, float dx, float dy) {
    float tfx, tfy, fx, fy;

    matter_anchor_fraction(ta, &tfx, &tfy);
    matter_anchor_fraction(a, &fx, &fy);
    this->glide_to(sec, m, target, tfx, tfy, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlane::glide_to(double sec, IMatter* m, IMatter* target, float tfx, float tfy, MatterAnchor a, float dx, float dy) {
    float fx, fy;

    matter_anchor_fraction(a, &fx, &fy);
    this->glide_to(sec, m, target, tfx, tfy, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlane::glide_to(double sec, IMatter* m, IMatter* target, MatterAnchor ta, float fx, float fy, float dx, float dy) {
    float tfx, tfy;

    matter_anchor_fraction(ta, &tfx, &tfy);
    this->glide_to(sec, m, target, tfx, tfy, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlane::glide_to(double sec, IMatter* m, IMatter* xtarget, float xfx, IMatter* ytarget, float yfy, MatterAnchor a, float dx, float dy) {
    float fx, fy;
    
    matter_anchor_fraction(a, &fx, &fy);
    this->glide_to(sec, m, xtarget, xfx, ytarget, yfy, fx, fy, dx, dy);
}

void WarGrey::STEM::IPlane::glide_to_random_location(double sec, IMatter* m) {
    IScreen* screen = this->master();
    float hinset, vinset, width, height;
    
    if (screen != nullptr) {
        m->feed_extent(0.0F, 0.0F, &hinset, &vinset);
        screen->feed_client_extent(&width, &height);

        hinset *= 0.5F;
        vinset *= 0.5F;

        this->glide_to(sec, m,
            float(random_uniform(int(hinset), int(width - hinset))),
            float(random_uniform(int(vinset), int(height - vinset))),
            MatterAnchor::CC);
    }
}

void WarGrey::STEM::IPlane::glide_to_mouse(double sec, IMatter* m, MatterAnchor a, float dx, float dy) {
    float mx, my;

    feed_current_mouse_location(&mx, &my);
    this->glide_to(sec, m, mx, my, a, dx, dy);
}

/*************************************************************************************************/
void WarGrey::STEM::IPlane::create_grid(int col, float x, float y, float width) {
    IScreen* master = this->master();
    float height;

    this->column = col;

    this->grid_x = x;
    this->grid_y = y;

    if (master != nullptr) {
        float Width;
        
        master->feed_client_extent(&Width, &height);

        if (width <= 0.0F) {
            width = Width - this->grid_x;
        }

        height -= this->grid_y;
    }
    
    if (this->column > 0) {
        this->cell_width = width / float(this->column);
        this->cell_height = this->cell_width;
        this->row = int(flfloor(height / this->cell_height));
    } else {
        this->cell_width = 0.0F;
        this->cell_height = 0.0F;
        this->row = col;
    }
}

void WarGrey::STEM::IPlane::create_grid(int row, int col, float x, float y, float width, float height) {
    this->row = row;
    this->column = col;

    this->grid_x = x;
    this->grid_y = y;

    if ((width <= 0.0F) || (height <= 0.0F)) {
        IScreen* master = this->master();

        if (master != nullptr) {
            float Width, Height;
        
            master->feed_client_extent(&Width, &Height);
            
            if (width <= 0.0F) {
                width = Width - this->grid_x;
            }

            if (height <= 0.0F) {
                height = Height - this->grid_y;
            }
        }
    }
    
    if (this->column > 0) {
        this->cell_width = width / float(this->column);
    }

    if (this->row > 0) {
        this->cell_height = height / float(this->row);
    }
}

void WarGrey::STEM::IPlane::create_grid(int row, int col, IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        float x = info->x;
        float y = info->y;
        float width, height;

        m->feed_extent(x, y, &width, &height);
        this->create_grid(row, col, x, y, width, height);
    }
}

void WarGrey::STEM::IPlane::create_grid(float cell_width, float x, float y, int col) {
    IScreen* master = this->master();
    float height;

    this->column = col;
    this->row = col;

    this->grid_x = x;
    this->grid_y = y;
    this->cell_width = cell_width;
    this->cell_height = cell_width;

    if (master != nullptr) {
        float Width;
        
        master->feed_client_extent(&Width, &height);
            
        if ((this->column <= 0) && (this->cell_width > 0.0F)) {
            this->column = int(flfloor((Width - this->grid_x) / this->cell_width));
            this->row = int(flfloor((height - this->grid_y) / this->cell_height));
        }
    }
}

void WarGrey::STEM::IPlane::create_grid(float cell_width, float cell_height, float x, float y, int row, int col) {
    this->column = col;
    this->row = row;

    this->grid_x = x;
    this->grid_y = y;
    this->cell_width = cell_width;
    this->cell_height = cell_height;

    if ((this->row <= 0) || (this->column <= 0)) {
        IScreen* master = this->master();

        if (master != nullptr) {
            float width, height;
        
            master->feed_client_extent(&width, &height);
            
            width -= x;
            height -= y;
            
            if ((this->column <= 0) && (this->cell_width > 0.0F)) {
                this->column = int(flfloor(width / this->cell_width));
            }

            if ((this->row <= 0) && (this->cell_height > 0.0F)) {
                this->row = int(flfloor(height / this->cell_height));
            }
        }
    }
}

int WarGrey::STEM::IPlane::grid_cell_index(float x, float y, int* r, int* c) {
    int row = int(flfloor((y - this->grid_y) / this->cell_height));
    int col = int(flfloor((x - this->grid_x) / this->cell_width));
    
    SET_VALUES(r, row, c, col);

    return row * this->column + col;
}

int WarGrey::STEM::IPlane::grid_cell_index(IMatter* m, int* r, int* c, MatterAnchor a) {
    float x, y;

    this->feed_matter_location(m, &x, &y, a);
    
    return this->grid_cell_index(x, y, r, c);    
}

void WarGrey::STEM::IPlane::feed_grid_cell_extent(float* width, float* height) {
    SET_BOX(width, this->cell_width);
    SET_BOX(height, this->cell_height);
}

void WarGrey::STEM::IPlane::feed_grid_cell_location(int idx, float* x, float* y, MatterAnchor a) {
    if (idx < 0) {
        idx += this->column * this->row;
    }

    if (this->column > 0) {
        int c = idx % this->column;
        int r = idx / this->column;

        this->feed_grid_cell_location(r, c, x, y, a);
    } else {
        this->feed_grid_cell_location(idx, 0, x, y, a);
    }
}

void WarGrey::STEM::IPlane::feed_grid_cell_location(int row, int col, float* x, float* y, MatterAnchor a) {
    float fx, fy;

    /** NOTE
     * Both `row` and `col` are just hints,
     *   and they are therefore allowed to be outside the grid,
     *   since the grid itself might be just a small port of the whole plane.
     */

    if (row < 0) {
        row += this->row;
    }

    if (col < 0) {
        col += this->column;
    }

    matter_anchor_fraction(a, &fx, &fy);
    
    SET_BOX(x, this->grid_x + this->cell_width * (float(col) + fx));
    SET_BOX(y, this->grid_y + this->cell_height * (float(row) + fy));
}

void WarGrey::STEM::IPlane::insert_at_grid(IMatter* m, int idx, MatterAnchor a, float dx, float dy) {
    float x, y;

    this->feed_grid_cell_location(idx, &x, &y, a);
    this->insert_at(m, x, y, a, dx, dy);
}

void WarGrey::STEM::IPlane::insert_at_grid(IMatter* m, int row, int col, MatterAnchor a, float dx, float dy) {
    float x, y;

    this->feed_grid_cell_location(row, col, &x, &y, a);
    this->insert_at(m, x, y, a, dx, dy);
}

void WarGrey::STEM::IPlane::move_to_grid(IMatter* m, int idx, MatterAnchor a, float dx, float dy) {
    float x, y;

    this->feed_grid_cell_location(idx, &x, &y, a);
    this->move_to(m, x, y, a, dx, dy);
}

void WarGrey::STEM::IPlane::move_to_grid(IMatter* m, int row, int col, MatterAnchor a, float dx, float dy) {
    float x, y;

    this->feed_grid_cell_location(row, col, &x, &y, a);
    this->move_to(m, x, y, a, dx, dy);
}

void WarGrey::STEM::IPlane::glide_to_grid(double sec, IMatter* m, int idx, MatterAnchor a, float dx, float dy) {
    float x, y;

    this->feed_grid_cell_location(idx, &x, &y, a);
    this->glide_to(sec, m, x, y, a, dx, dy);
}

void WarGrey::STEM::IPlane::glide_to_grid(double sec, IMatter* m, int row, int col, MatterAnchor a, float dx, float dy) {
    float x, y;

    this->feed_grid_cell_location(row, col, &x, &y, a);
    this->glide_to(sec, m, x, y, a, dx, dy);
}
