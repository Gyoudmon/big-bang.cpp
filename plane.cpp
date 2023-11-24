#include "plane.hpp"
#include "matter.hpp"
#include "misc.hpp"

#include "graphics/pen.hpp"
#include "graphics/ruler.hpp"
#include "graphics/colorspace.hpp"

#include "matter/sprite.hpp"
#include "matter/graphlet/textlet.hpp"
#include "matter/graphlet/tracklet.hpp"

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
#define SPEECH_INFO(m) (static_cast<SpeechInfo*>(m->info))

namespace WarGrey::STEM {
    enum class MotionTargetType { Vector, Location, Anchor };
    enum class MotionActionType { Motion, TrackReset, TrackDrawing, PenColor, PenWidth, Heading, Rotation, Stamp };
    
    struct MTLocation {
        float x;
        float y;
    };
    
    struct MTAnchor {
        IMatter* matter;
        float fx;
        float fy;
    };

    union GMTargetBody {
        double length;
        MTLocation dot;
    };

    struct GMTarget {
        MotionTargetType type;
        GMTargetBody body;
    };

    struct GlidingMotion {
        GMTarget target;
        
        double second;
        double sec_delta; // 0.0 means the target is based on current heading
        bool absolute;
        bool heading; // moving only
    };

    struct PenColor {
        uint32_t hex;
        double alpha;
    };

    union MotionActionBody {
        bool drawing;
        double direction;
        double theta;
        uint8_t pen_width;
        PenColor color;
        GlidingMotion motion;
    };

    struct MotionAction {
        MotionActionType type;
        MotionActionBody body;
    };

    struct MatterInfo : public WarGrey::STEM::IMatterInfo {
        MatterInfo(WarGrey::STEM::IPlane* master) : IMatterInfo(master) {}
        virtual ~MatterInfo() noexcept;

        float x = 0.0F;
        float y = 0.0F;

        // for mouse selection
        bool selected = false;
        uint32_t selection_hit = 0;

        // for speech bubble
        IMatter* bubble = nullptr;
        SpeechBubble bubble_type = SpeechBubble::Default;
        long long bubble_expiration_time = 0;
        
        // for animation
        uint32_t local_frame_delta = 0U;
        uint32_t local_frame_count = 0U;
        uint32_t local_elapse = 0U;
        int duration = 0;

        // for queued motions
        bool gliding = false;
        float gliding_tx = 0.0F;
        float gliding_ty = 0.0F;
        std::deque<MotionAction> motion_actions;

        // for track
        Tracklet* canvas = nullptr;
        bool shared_canvas = false;
        bool is_drawing = false;
        float draw_fx = 0.5F;
        float draw_fy = 0.5F;
        uint32_t draw_color = 0x0U;
        double draw_alpha = 1.0;
        uint8_t draw_width = 1U;

        // gliding progressbar
        double current_step = 1.0;
        double progress_total = 1.0;

        // as linked list
        IMatter* next = nullptr;
        IMatter* prev = nullptr;
    };

    class SpeechInfo : public WarGrey::STEM::IMatterInfo {
    public:
        SpeechInfo(WarGrey::STEM::IPlane* master) : IMatterInfo(master) {}
        virtual ~SpeechInfo() {}

    public:
        void counter_increase() {
            this->refcount ++;
        }

        void counter_decrease(IMatter* master) {
            this->refcount --;

            if (this->refcount <= 0) {
                delete master;
            }
        }

    public:
        // as linked list
        IMatter* next = nullptr;

    private:
        uint32_t refcount = 0;
    };

    WarGrey::STEM::MatterInfo::~MatterInfo() noexcept {
        if (this->bubble != nullptr) {
            auto speech_info = dynamic_cast<SpeechInfo*>(this->bubble->info);

            if (speech_info != nullptr) {
                speech_info->counter_decrease(this->bubble);
            }

            this->bubble = nullptr;
        }
    }
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

static inline void unsafe_canvas_info_reset(MatterInfo* info) {
    info->is_drawing = false;
    info->draw_color = 0x0U;
    info->draw_alpha = 1.0;
    info->draw_width = 1U;
}

static inline void unsafe_canvas_sync_settings(MatterInfo* info) {
    info->canvas->set_drawing(info->is_drawing);
    info->canvas->set_pen_width(info->draw_width);
    info->canvas->set_pen_color(info->draw_color, info->draw_alpha);
}

static void unsafe_canvas_info_do_setting(IPlane* self, IMatter* m, MatterInfo* info, const MotionAction& op) {
    switch (op.type) {
    case MotionActionType::PenColor: {
        info->draw_color = op.body.color.hex;
        info->draw_alpha = op.body.color.alpha;
    }; break;
    case MotionActionType::TrackDrawing: info->is_drawing = op.body.drawing; break;
    case MotionActionType::PenWidth: info->draw_width = op.body.pen_width; break;
    case MotionActionType::TrackReset: unsafe_canvas_info_reset(info); break;
    case MotionActionType::Rotation: m->add_heading(op.body.theta, true); break;
    case MotionActionType::Heading: m->set_heading(op.body.direction, true); break;
    case MotionActionType::Stamp: {
        float x, y;

        self->feed_matter_location(m, &x, &y, MatterAnchor::LT);
        info->canvas->stamp(m, x, y);
     }; break;
    default: /* ignored */;
    }
}

static void unsafe_do_canvas_setting(Plane* self, IMatter* m, MatterInfo* info, MotionAction& op) {
    if (info->gliding) {
        info->motion_actions.push_back(op);
    } else {
        unsafe_canvas_info_do_setting(self, m, info, op);

        if (!info->shared_canvas) {
            unsafe_canvas_sync_settings(info);
        }
    }
}

static void unsafe_canvas_do_drawing(IMatter* m, MatterInfo* info, float x1, float y1, float x2, float y2) {
    if ((info->draw_fx != 0.0F) || (info->draw_fy != 0.0F)) {
        float dx, dy, mwidth, mheight;
        
        m->feed_extent(x1, y1, &mwidth, &mheight);
        dx = info->draw_fx * mwidth;
        dy = info->draw_fy * mheight;

        info->canvas->add_line(x1 + dx, y1 + dy, x2 + dx, y2 + dy);
    } else {
        info->canvas->add_line(x1, y1, x2, y2);
    }
}

static inline void unsafe_location_changed(IMatter* m, MatterInfo* info, float ox, float oy, bool ignore_track) {
    m->on_location_changed(info->x, info->y, ox, oy);

    if ((info->canvas != nullptr) && (!ignore_track)) {
        if (info->shared_canvas) {
            unsafe_canvas_sync_settings(info);
        }

        unsafe_canvas_do_drawing(m, info, ox, oy, info->x, info->y);
    }
}

static inline MatterInfo* bind_matter_owership(IPlane* master, IMatter* m) {
    auto info = new MatterInfo(master);
    
    unsafe_set_matter_fps(info, m->preferred_local_fps(), true);
    m->info = info;

    return info;
}

static inline SpeechInfo* bind_speech_owership(IPlane* master, IMatter* m) {
    auto info = new SpeechInfo(master);

    info->counter_increase();
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

static inline void bubble_start(ISprite* m, MatterInfo* info, double sec, SpeechBubble type, double default_duration) {
    double duration = (sec > 0.0) ? sec : default_duration;

    info->bubble_type = type;            
    info->bubble_expiration_time = current_milliseconds() + fl2fx<long long>(duration * 1000.0);

    printf("bubble start\n");
    fflush(stdout);
    
    if (type == SpeechBubble::Default) {
        m->play_speaking(1);
    } else {
        m->play_thinking(1);
    }
}

static inline void bubble_expire(IMatter* m, MatterInfo* info) {
    info->bubble_expiration_time = 0LL;
}

static inline bool is_matter_bubble_showing(IMatter* m, MatterInfo* info) {
    bool yes = false;

    if (info->bubble != nullptr) {
        if (info->bubble_expiration_time > 0LL) {
            yes = true;
        }
    }

    return yes;
}

static inline void unsafe_feed_matter_bound(IMatter* m, MatterInfo* info, float* x, float* y, float* width, float* height) {
    m->feed_extent(info->x, info->y, width, height);

    (*x) = info->x;
    (*y) = info->y;
}

static inline void unsafe_add_selected(WarGrey::STEM::IPlane* master, IMatter* m, MatterInfo* info, bool selected) {
    master->on_select(m, selected);
    info->selected = selected;
    master->after_select(m, selected);
    master->notify_updated();
}

static inline void unsafe_set_selected(WarGrey::STEM::IPlane* master, IMatter* m, MatterInfo* info) {
    master->begin_update_sequence();
    master->no_selected();
    unsafe_add_selected(master, m, info, true);
    master->end_update_sequence();
}

static IMatter* do_search_selected_matter(IMatter* start, IMatter* terminator) {
    IMatter* found = nullptr;
    IMatter* child = start;

    do {
        MatterInfo* info = MATTER_INFO(child);

        if (info->selected) {
            found = child;
            break;
        }

        child = info->next;
    } while (child != terminator);
    
    return found;
}

/*************************************************************************************************/
Plane::Plane(const std::string& name) : Plane(name.c_str()) {}
Plane::Plane(const char* name) : IPlane(name), head_matter(nullptr) {
    this->bubble_font = GameFont::Tooltip(FontSize::medium);
    this->set_bubble_duration();
}

Plane::~Plane() {
    this->erase();
}

void WarGrey::STEM::Plane::notify_matter_ready(IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        this->size_cache_invalid();
        this->begin_update_sequence();
        this->notify_updated();
        this->on_matter_ready(m);
        this->end_update_sequence();
    }
}

void WarGrey::STEM::Plane::bring_to_front(IMatter* m, IMatter* target) {
    MatterInfo* tinfo = plane_matter_info(this, target);

    if (tinfo == nullptr) {
        if (this->head_matter != nullptr) {
            this->bring_to_front(m, MATTER_INFO(this->head_matter)->prev);
        }
    } else {
        MatterInfo* sinfo = plane_matter_info(this, m);
        
        if ((sinfo != nullptr) && (m != target)) {
            if (tinfo->next != m) {
                MATTER_INFO(sinfo->prev)->next = sinfo->next;
                MATTER_INFO(sinfo->next)->prev = sinfo->prev;
                MATTER_INFO(tinfo->next)->prev = m;
                sinfo->prev = target;
                sinfo->next = tinfo->next;
                tinfo->next = m;
            }

            if (this->head_matter == m) {
                this->head_matter = sinfo->next;
            }
            
            this->notify_updated();
        }
    }
}

void WarGrey::STEM::Plane::bring_forward(IMatter* m, int n) {
    MatterInfo* sinfo = plane_matter_info(this, m);
    
    if (sinfo != nullptr) {
        IMatter* sentry = MATTER_INFO(this->head_matter)->prev;
        IMatter* target = m;

        while ((target != sentry) && (n > 0)) {
            n --;
            target = MATTER_INFO(target)->next;
        }

        this->bring_to_front(m, target);
    }
}

void WarGrey::STEM::Plane::send_to_back(IMatter* m, IMatter* target) {
    MatterInfo* tinfo = plane_matter_info(this, target);

    if (tinfo == nullptr) {
        if (this->head_matter != nullptr) {
            this->send_to_back(m, this->head_matter);
        }
    } else {
        MatterInfo* sinfo = plane_matter_info(this, m);
        
        if ((sinfo != nullptr) && (m != target)) {
            if (tinfo->prev != m) {
                MATTER_INFO(sinfo->prev)->next = sinfo->next;
                MATTER_INFO(sinfo->next)->prev = sinfo->prev;
                MATTER_INFO(tinfo->prev)->next = m;
                sinfo->next = target;
                sinfo->prev = tinfo->prev;
                tinfo->prev = m;
            }

            if (this->head_matter == target) {
                this->head_matter = m;
            }

            this->notify_updated();
        }
    }
}

void WarGrey::STEM::Plane::send_backward(IMatter* m, int n) {
    MatterInfo* sinfo = plane_matter_info(this, m);
    
    if (sinfo != nullptr) {
        IMatter* target = m;

        while ((target != this->head_matter) && (n > 0)) {
            n --;
            target = MATTER_INFO(target)->prev;
        }

        this->send_to_back(m, target);
    }
}

void WarGrey::STEM::Plane::insert_at(IMatter* m, float x, float y, float fx, float fy, float dx, float dy) {
    if (m->info == nullptr) {
        MatterInfo* info = bind_matter_owership(this, m);
        
        if (this->head_matter == nullptr) {
            this->head_matter = m;
            info->prev = this->head_matter;
            info->next = this->head_matter;
        } else {
            MatterInfo* head_info = MATTER_INFO(this->head_matter);
            MatterInfo* prev_info = MATTER_INFO(head_info->prev);
            
            info->prev = head_info->prev;
            info->next = this->head_matter;
            prev_info->next = m;
            head_info->prev = m;
        }

        this->handle_new_matter(m, info, x, y, fx, fy, dx, dy);
    }
}

void WarGrey::STEM::Plane::insert_as_speech_bubble(IMatter* m) {
    if (m->info == nullptr) {
        SpeechInfo* info = bind_speech_owership(this, m);

        info->next = this->head_speech;
        this->head_speech = m;

        this->handle_new_matter(m, info);
    }
}

void WarGrey::STEM::Plane::handle_new_matter(IMatter* m, MatterInfo* info, float x, float y, float fx, float fy, float dx, float dy) {
    this->begin_update_sequence();
    m->construct(this->master_renderer());
    this->move_matter_to_location_via_info(m, info, x, y, fx, fy, dx, dy);
    
    if (m->ready()) {
        this->on_matter_ready(m);
    }
    
    this->notify_updated();
    this->end_update_sequence();
}


void WarGrey::STEM::Plane::handle_new_matter(IMatter* m, SpeechInfo* info) {
    this->begin_update_sequence();
    m->construct(this->master_renderer());
    this->notify_updated();
    this->end_update_sequence();
}

void WarGrey::STEM::Plane::remove(IMatter* m, bool needs_delete) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        MATTER_INFO(info->prev)->next = info->next;
        MATTER_INFO(info->next)->prev = info->prev;

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
            this->delete_matter(m);
        }

        this->notify_updated();
        this->size_cache_invalid();
    }
}

void WarGrey::STEM::Plane::erase() {
    IMatter* temp_head = this->head_matter;
        
    if (this->head_matter != nullptr) {
        MatterInfo* temp_info = MATTER_INFO(temp_head);
        MatterInfo* prev_info = MATTER_INFO(temp_info->prev);

        this->head_matter = nullptr;
        prev_info->next = nullptr;

        do {
            IMatter* child = temp_head;

            temp_head = MATTER_INFO(temp_head)->next;
            this->delete_matter(child);
        } while (temp_head != nullptr);

        this->size_cache_invalid();
    }

    while (this->head_speech != nullptr) {
        temp_head = this->head_speech;
        this->head_speech = SPEECH_INFO(this->head_speech)->next;
        this->delete_matter(temp_head);
    }
}

void WarGrey::STEM::Plane::move(IMatter* m, double length, bool ignore_gliding) {
    if (m != nullptr) {
        MatterInfo* info = plane_matter_info(this, m);

        if (info != nullptr) {
            if (this->move_matter_via_info(m, info, length, ignore_gliding, false)) {
                this->notify_updated();
            }
        }
    } else if (this->head_matter != nullptr) {
        IMatter* child = this->head_matter;

        do {
            MatterInfo* info = MATTER_INFO(child);

            if (info->selected) {
                this->move_matter_via_info(child, info, length, ignore_gliding, false);
            }

            child = info->next;
        } while (child != this->head_matter);

        this->notify_updated();
    }
}

void WarGrey::STEM::Plane::move(IMatter* m, float x, float y, bool ignore_gliding) {
    if (m != nullptr) {
        MatterInfo* info = plane_matter_info(this, m);

        if (info != nullptr) {
            if (this->move_matter_via_info(m, info, x, y, false, ignore_gliding, false)) {
                this->notify_updated();
            }
        }
    } else if (this->head_matter != nullptr) {
        IMatter* child = this->head_matter;

        do {
            MatterInfo* info = MATTER_INFO(child);

            if (info->selected) {
                this->move_matter_via_info(child, info, x, y, false, ignore_gliding, false);
            }

            child = info->next;
        } while (child != this->head_matter);

        this->notify_updated();
    }
}

void WarGrey::STEM::Plane::move_to(IMatter* m, float x, float y, float fx, float fy, float dx, float dy) {
    MatterInfo* info = plane_matter_info(this, m);
    
    if (info != nullptr) {
        if (this->move_matter_to_location_via_info(m, info, x, y, fx, fy, dx, dy)) {
            this->notify_updated();
        }
    }
}

void WarGrey::STEM::Plane::move_to(IMatter* m, IMatter* target, float tfx, float tfy, float fx, float fy, float dx, float dy) {
    MatterInfo* tinfo = plane_matter_info(this, target);
    float x = 0.0F;
    float y = 0.0F;

    if (tinfo != nullptr) {
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

    if ((xinfo != nullptr) && (yinfo != nullptr)) {
        float xsx, xsy, xsw, xsh, ysx, ysy, ysw, ysh;

        unsafe_feed_matter_bound(xtarget, xinfo, &xsx, &xsy, &xsw, &xsh);
        unsafe_feed_matter_bound(ytarget, yinfo, &ysx, &ysy, &ysw, &ysh);
        x = xsx + xsw * xfx;
        y = ysy + ysh * yfy;
    }

    this->move_to(m, x, y, fx, fy, dx, dy);
}

void WarGrey::STEM::Plane::glide(double sec, IMatter* m, double length) {
    if (m != nullptr) {
        MatterInfo* info = plane_matter_info(this, m);

        if (info != nullptr) {
            this->glide_matter_via_info(m, info, sec, length);
        }
    } else if (this->head_matter != nullptr) {
        IMatter* child = this->head_matter;

        do {
            MatterInfo* info = MATTER_INFO(child);

            if (info->selected) {
                this->glide_matter_via_info(child, info, sec, length);
            }

            child = info->next;
        } while (child != this->head_matter);
    }
}

void WarGrey::STEM::Plane::glide(double sec, IMatter* m, float x, float y) {
    if (m != nullptr) {
        MatterInfo* info = plane_matter_info(this, m);

        if (info != nullptr) {
            this->glide_matter_via_info(m, info, sec, x, y, false, true);
        }
    } else if (this->head_matter != nullptr) {
        IMatter* child = this->head_matter;

        do {
            MatterInfo* info = MATTER_INFO(child);

            if (info->selected) {
                this->glide_matter_via_info(child, info, sec, x, y, false, true);
            }

            child = info->next;
        } while (child != this->head_matter);
    }
}

void WarGrey::STEM::Plane::glide_to(double sec, IMatter* m, float x, float y, float fx, float fy, float dx, float dy) {
    MatterInfo* info = plane_matter_info(this, m);
    
    if (info != nullptr) {
        this->glide_matter_to_location_via_info(m, info, sec, x, y, fx, fy, dx, dy, true);
    }
}

void WarGrey::STEM::Plane::glide_to(double sec, IMatter* m, IMatter* target, float tfx, float tfy, float fx, float fy, float dx, float dy) {
    MatterInfo* tinfo = plane_matter_info(this, target);
    float x = 0.0F;
    float y = 0.0F;

    if (tinfo != nullptr) {
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

    if ((xinfo != nullptr) && (yinfo != nullptr)) {
        float xsx, xsy, xsw, xsh, ysx, ysy, ysw, ysh;

        unsafe_feed_matter_bound(xtarget, xinfo, &xsx, &xsy, &xsw, &xsh);
        unsafe_feed_matter_bound(ytarget, yinfo, &ysx, &ysy, &ysw, &ysh);
        x = xsx + xsw * xfx;
        y = ysy + ysh * yfy;
    }

    this->glide_to(sec, m, x, y, fx, fy, dx, dy);
}

void WarGrey::STEM::Plane::clear_motion_actions(IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        info->motion_actions.clear();
    }
}

IMatter* WarGrey::STEM::Plane::find_matter(float x, float y, IMatter* after) {
    IMatter* found = nullptr;

    if (this->head_matter != nullptr) {
        MatterInfo* head_info = MATTER_INFO(this->head_matter);
        MatterInfo* aftr_info = plane_matter_info(this, after);
        IMatter* child = (aftr_info == nullptr) ? head_info->prev : aftr_info->prev;

        do {
            MatterInfo* info = MATTER_INFO(child);

            if (child->visible() && !child->concealled()) {
                if (this->is_matter_found(child, info, x, y)) {
                    found = child;
                    break;
                }
            }

            child = info->prev;
        } while (child != head_info->prev);
    }

    return found;
}

IMatter* WarGrey::STEM::Plane::find_least_recent_matter(float x, float y) {
    IMatter* found = nullptr;
    uint32_t found_hit = 0xFFFFFFFFU;

    if (this->head_matter != nullptr) {
        MatterInfo* head_info = MATTER_INFO(this->head_matter);
        IMatter* child = head_info->prev;

        do {
            MatterInfo* info = MATTER_INFO(child);

            if (child->visible() && !child->concealled()) {
                if (this->is_matter_found(child, info, x, y)) {
                    if (info->selection_hit < found_hit) {
                        found = child;
                        found_hit = info->selection_hit;
                    }
                } else {
                    info->selection_hit = 0U;
                }
            } else {
                info->selection_hit = 0U;
            }

            child = info->prev;
        } while (child != head_info->prev);
    }

    if (found != nullptr) {
        MATTER_INFO(found)->selection_hit ++;
    }

    return found;
}

/**
 * TODO: if we need to check selected matters first? 
 */
IMatter* WarGrey::STEM::Plane::find_matter_for_tooltip(float x, float y) {
    IMatter* found = nullptr;

    if (this->head_matter != nullptr) {
        MatterInfo* head_info = MATTER_INFO(this->head_matter);
        IMatter* child = head_info->prev;

        do {
            MatterInfo* info = MATTER_INFO(child);

            if (child->visible()) {
                if (this->is_matter_found(child, info, x, y)) {
                    found = child;
                    break;
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
            found = do_search_selected_matter(this->head_matter, this->head_matter);
        }
    } else {
        MatterInfo* info = plane_matter_info(this, start);

        if (info != nullptr) {
            found = do_search_selected_matter(info->next, this->head_matter);
        }
    }

    return found;
}

bool WarGrey::STEM::Plane::feed_matter_location(IMatter* m, float* x, float* y, float fx, float fy) {
    MatterInfo* info = plane_matter_info(this, m);
    bool okay = false;
    
    if (info != nullptr) {
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
    
    if (info != nullptr) {
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

                unsafe_feed_matter_bound(child, info, &rx, &ry, &width, &height);
                this->matters_left = flmin(this->matters_left, rx);
                this->matters_top = flmin(this->matters_top, ry);
                this->matters_right = flmax(this->matters_right, rx + width);
                this->matters_bottom = flmax(this->matters_bottom, ry + height);

                child = info->next;
            } while (child != this->head_matter);
        }
    }
}

void WarGrey::STEM::Plane::add_selected(IMatter* m) {
    if (this->can_select_multiple()) {
        MatterInfo* info = plane_matter_info(this, m);

        if ((info != nullptr) && (!info->selected)) {
            if (this->can_select(m)) {
                unsafe_add_selected(this, m, info, true);
            }
        }
    }
}

void WarGrey::STEM::Plane::remove_selected(IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);

    if ((info != nullptr) && (info->selected)) {
        unsafe_add_selected(this, m, info, false);
    }
}

void WarGrey::STEM::Plane::set_selected(IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);

    if ((info != nullptr) && (!info->selected)) {
        if (this->can_select(m)) {
            unsafe_set_selected(this, m, info);
        }
    }
}

void WarGrey::STEM::Plane::no_selected() {
    this->no_selected_except(nullptr);
}

void WarGrey::STEM::Plane::no_selected_except(IMatter* m) {
    if (this->head_matter != nullptr) {
        IMatter* child = this->head_matter;

        this->begin_update_sequence();

        do {
            MatterInfo* info = MATTER_INFO(child);

            if (info->selected && (child != m)) {
                unsafe_add_selected(this, child, info, false);
            }

            child = info->next;
        } while (child != this->head_matter);

        this->end_update_sequence();
    }
}

bool WarGrey::STEM::Plane::is_selected(IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);
    bool selected = false;

    if (info != nullptr) {
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

            if (info->selected) {
                n += 1U;
            }

            child = info->next;
        } while (child != this->head_matter);
    }

    return n;
}

IMatter* WarGrey::STEM::Plane::get_focused_matter() {
    return this->focused_matter;
}

void WarGrey::STEM::Plane::set_caret_owner(IMatter* m) {
    if (this->focused_matter != m) {
        if ((m != nullptr) && (m->events_allowed())) {
            MatterInfo* info = plane_matter_info(this, m);

            if (info != nullptr) {
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
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        if (!info->selected) {
            if (this->can_select(m)) {
                if (this->can_select_multiple()) {
                    unsafe_add_selected(this, m, info, true);
                } else {
                    unsafe_set_selected(this, m, info);
                    this->set_caret_owner(m);
                }

                if ((this->tooltip != nullptr) && (this->tooltip->visible())) {
                    this->update_tooltip(m, local_x, local_y, local_x + info->x, local_y + info->y);
                    this->place_tooltip(m);
                }
            } else if (!this->can_select_multiple()) {
                this->no_selected();
            }
        }
    }
}

void WarGrey::STEM::Plane::on_tap_selected(IMatter* m, float local_x, float local_y) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        unsafe_add_selected(this, m, info, false);
    }
}

bool WarGrey::STEM::Plane::on_pointer_pressed(uint8_t button, float x, float y, uint8_t clicks) {
    bool handled = false;

    switch (button) {
    case SDL_BUTTON_LEFT: {
        IMatter* self_matter = this->find_matter(x, y, static_cast<IMatter*>(nullptr));

        if (self_matter != nullptr) {
            if (self_matter->low_level_events_allowed()) {
                MatterInfo* info = MATTER_INFO(self_matter);

                float local_x = x - info->x;
                float local_y = y - info->y;

                handled = self_matter->on_pointer_pressed(button, local_x, local_y, clicks);
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
        IMatter* self_matter = this->find_matter_for_tooltip(x, y);

        if ((self_matter == nullptr) || (self_matter != this->hovering_matter)) {
            if ((self_matter != nullptr) && !self_matter->concealled()) {
                this->say_goodbye_to_hover_matter(state, x, y, dx, dy);
            }

            if ((this->tooltip != nullptr) && (this->tooltip != self_matter) && this->tooltip->visible()) {
                this->tooltip->show(false);
            }
        }

        if (self_matter != nullptr) {
            MatterInfo* info = MATTER_INFO(self_matter);
            float local_x = x - info->x;
            float local_y = y - info->y;

            if (!self_matter->concealled()) {
                this->hovering_matter = self_matter;
                this->hovering_mgx = x;
                this->hovering_mgy = y;
                this->hovering_mlx = local_x;
                this->hovering_mly = local_y;

                if (self_matter->events_allowed()) {
                    self_matter->on_hover(local_x, local_y);

                    if (self_matter->low_level_events_allowed()) {
                        self_matter->on_pointer_move(state, local_x, local_y, dx, dy, false);
                    }
                }

                this->on_hover(this->hovering_matter, local_x, local_y);
                handled = true;
            }

            if (this->tooltip != nullptr) {
                if (this->update_tooltip(self_matter, local_x, local_y, x, y)) {
                    if (!this->tooltip->visible()) {
                        this->tooltip->show(true);
                    }

                    this->place_tooltip(self_matter);
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
        IMatter* self_matter = this->find_least_recent_matter(x, y);

        if (self_matter != nullptr) {
            MatterInfo* info = MATTER_INFO(self_matter);
            float local_x = x - info->x;
            float local_y = y - info->y;

            if (self_matter->events_allowed()) {
                if (clicks == 1) {
                    self_matter->on_tap(local_x, local_y);
                } else if (clicks == 2) {
                    self_matter->on_double_tap(local_x, local_y);
                }

                if (self_matter->low_level_events_allowed()) {
                    self_matter->on_pointer_released(button, local_x, local_y, clicks);
                }
            }

            if (self_matter != this->sentry) {
                if (info->selected) {
                    this->on_tap_selected(self_matter, local_x, local_y);
                    handled = !info->selected;
                } else {
                    this->on_tap(self_matter, local_x, local_y);
                    handled = info->selected;
                }
            } else {
                if ((clicks == 2) && (this->can_select(self_matter))) {
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
        this->sentry->play_greeting(1);
    }

    IPlane::on_enter(from);
}

void WarGrey::STEM::Plane::mission_complete() {
    if (this->sentry != nullptr) {
        this->sentry->play_goodbye(1);
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
            
            elapse = local_timeline_elapse(interval, info->local_frame_delta, info->local_elapse, info->duration);
                
            if (elapse > 0U) {
                info->duration = child->update(info->local_frame_count ++, elapse, uptime);
            }

            /* controlling motion via global timeline makes it more smooth */
            this->handle_queued_motion(child, info, dwidth, dheight);

            if (is_matter_bubble_showing(child, info)) {
                if (current_milliseconds() >= info->bubble_expiration_time) {
                    bubble_expire(child, info);
                    this->notify_updated(child);
                }
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
        Pen::fill_rect(renderer, dsX, dsY, dsWidth, dsHeight, this->background, this->bg_alpha);
    }

    if ((this->grid_alpha > 0.0F)
            && (this->column > 0) && (this->row > 0)
            && (this->cell_width > 0.0F) && (this->cell_height > 0.0F)) {
        RGB_SetRenderDrawColor(renderer, this->grid_color, this->grid_alpha);
        Pen::draw_grid(renderer, this->row, this->column,
                        this->cell_width, this->cell_height,
                        this->grid_x, this->grid_y);
    }

    if (this->head_matter != nullptr) {
        IMatter* speech_head = nullptr;
        IMatter* speech_nil = nullptr;
        IMatter* child = this->head_matter;
        MatterInfo* info = nullptr;
        
        do {
            info = MATTER_INFO(child);

            if (this->tooltip != child) {
                if (info->bubble != nullptr) {
                    if (speech_head == nullptr) {
                        speech_head = child;
                    }

                    speech_nil = info->next;
                }

                this->draw_matter(renderer, child, info, X, Y, dsX, dsY, dsWidth, dsHeight);
            }

            child = info->next;
        } while (child != this->head_matter);

        if (speech_head != nullptr) {
            child = speech_head;

            do {
                info = MATTER_INFO(child);
                this->draw_speech(renderer, child, info, Width, Height, X, Y, dsX, dsY, dsWidth, dsHeight);
                child = info->next;
            } while (child != speech_nil);
        }

        if (this->tooltip != nullptr) {
            this->draw_matter(renderer, this->tooltip, MATTER_INFO(this->tooltip), X, Y, dsX, dsY, dsWidth, dsHeight);
        }

        SDL_RenderSetClipRect(renderer, nullptr);
    }

    // Ruler::draw_ht_hatchmark(renderer, 300.0F, 300.0F, 150.0F, 0.0, 100.0, 10, ROYALBLUE);
    // Ruler::draw_hb_hatchmark(renderer, 300.0F, 300.0F, 150.0F, 0.0, 100.0, 10, CRIMSON);
}

void WarGrey::STEM::Plane::draw_visible_selection(SDL_Renderer* renderer, float x, float y, float width, float height) {
    Pen::draw_rect(renderer, x, y, width, height, 0x00FFFFU);
}

void WarGrey::STEM::Plane::draw_matter(SDL_Renderer* renderer, IMatter* child, MatterInfo* info, float X, float Y, float dsX, float dsY, float dsWidth, float dsHeight) {
    float mx, my, mwidth, mheight;
    SDL_Rect clip;
    
    if (child->visible()) {
        child->feed_extent(info->x, info->y, &mwidth, &mheight);

        mx = (info->x + this->translate_x) + X;
        my = (info->y + this->translate_y) + Y;
                
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
}

void WarGrey::STEM::Plane::draw_speech(SDL_Renderer* renderer, IMatter* child, MatterInfo* info, float Width, float Height, float X, float Y, float dsX, float dsY, float dsWidth, float dsHeight) {
    if (is_matter_bubble_showing(child, info)) {
        float ix, iy, iwidth, iheight, bx, by, bwidth, bheight;
        float bfx, bfy, mfx, mfy, dx, dy;
        float mwidth, mheight;
        SDL_Rect clip;
    
        child->feed_extent(info->x, info->y, &mwidth, &mheight);
        info->bubble->feed_extent(0.0F, 0.0F, &iwidth, &iheight);

        bwidth =  iwidth +  (this->bubble_left_margin + this->bubble_right_margin);
        bheight = iheight + (this->bubble_top_margin + this->bubble_bottom_margin);
        this->place_speech_bubble(child, bwidth, bheight, Width, Height, &mfx, &mfy, &bfx, &bfy, &dx, &dy);
        bx = info->x + mwidth  * mfx - bwidth  * bfx + dx;
        by = info->y + mheight * mfy - bheight * bfy + dy;
        bx = (bx + this->translate_x) + X;
        by = (by + this->translate_y) + Y;

        if (rectangle_overlay(bx, by, bx + bwidth, by + bheight, dsX, dsY, dsWidth, dsHeight)) {
            bx = flmax(bx, this->bubble_left_margin + X);
            by = flmax(by, this->bubble_top_margin  + Y);

            ix = bx + this->bubble_left_margin;
            iy = by + this->bubble_top_margin;
        
            clip.x = fl2fxi(flfloor(ix));
            clip.y = fl2fxi(flfloor(iy));
            clip.w = fl2fxi(flceiling(iwidth));
            clip.h = fl2fxi(flceiling(iheight));

            SDL_RenderSetClipRect(renderer, nullptr);

            Pen::fill_rounded_rect(renderer, bx, by, bwidth, bheight, -0.25F, this->bubble_color, this->bubble_alpha);
            Pen::draw_rounded_rect(renderer, bx, by, bwidth, bheight, -0.25F, this->bubble_border, this->bubble_alpha);

            SDL_RenderSetClipRect(renderer, &clip);
            
            if (info->bubble->ready()) {
                info->bubble->draw(renderer, ix, iy, iwidth, iheight);
            } else {
                info->bubble->draw_in_progress(renderer, ix, iy, iwidth, iheight);
            }
        }
    }
}

/*************************************************************************************************/
bool WarGrey::STEM::Plane::do_moving_via_info(IMatter* m, MatterInfo* info, float x, float y, bool absolute, bool ignore_track, bool heading) {
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

        if (heading) {
            m->set_heading(x - ox, y - oy);
        }

        unsafe_location_changed(m, info, ox, oy, ignore_track);
        this->size_cache_invalid();
        moved = true;
    }

    return moved;
}

bool WarGrey::STEM::Plane::do_gliding_via_info(IMatter* m, MatterInfo* info, float x, float y, double sec, double sec_delta, bool absolute, bool ignore_track) {
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
        unsafe_location_changed(m, info, x - dx, y - dy, ignore_track);
        this->size_cache_invalid();
        moved = true;
    }

    return moved;
}

bool WarGrey::STEM::Plane::move_matter_via_info(IMatter* m, MatterInfo* info, double length, bool ignore_gliding, bool heading) {
    bool moved = false;

    if ((!info->gliding) || ignore_gliding) {
        moved = this->do_vector_moving(m, info, length, heading);
    } else {
        double x, y;

        orthogonal_decomposition(length, m->get_heading(), &x, &y);
    
        return this->move_matter_via_info(m, info, float(x), float(y), false, ignore_gliding, heading);
    }

    return moved;
}

bool WarGrey::STEM::Plane::move_matter_via_info(IMatter* m, MatterInfo* info, float x, float y, bool absolute, bool ignore_gliding, bool heading) {
    bool moved = false;

    if ((!info->gliding) || ignore_gliding) {
        moved = this->do_moving_via_info(m, info, x, y, absolute, false, heading);
    } else if (m == this->tooltip) {
        moved = this->do_moving_via_info(m, info, x, y, absolute, true, heading);
    } else {
        GMTarget target;
        MotionAction action;

        target.type = MotionTargetType::Location;
        target.body.dot = { x, y };

        action.type = MotionActionType::Motion;
        action.body.motion = { target, 0.0F, 0.0F, absolute, heading };

        info->motion_actions.push_back(action);
    }

    return moved;
}

bool WarGrey::STEM::Plane::glide_matter_via_info(IMatter* m, MatterInfo* info, double sec, double length) {
    bool moved = false;

    if (!info->gliding) {
        this->do_vector_gliding(m, info, length, sec);
    } else {
        GMTarget target;
        MotionAction action;

        target.type = MotionTargetType::Vector;
        target.body.length = length;

        action.type = MotionActionType::Motion;
        action.body.motion = { target, sec, 0.0F, false, true };

        info->motion_actions.push_back(action);   
    }
    
    return moved;
}

bool WarGrey::STEM::Plane::glide_matter_via_info(IMatter* m, MatterInfo* info, double sec, float x, float y, bool absolute, bool heading) {
    bool moved = false;
    
    if (sec <= 0.0F) {
        moved = this->move_matter_via_info(m, info, x, y, absolute, false, heading);
    } else {
        IScreen* screen = this->master();
        double sec_delta = (screen != nullptr) ? (1.0 / double(screen->frame_rate())) : 0.0;

        if ((sec <= sec_delta) || (sec_delta == 0.0)) {
            moved = this->move_matter_via_info(m, info, x, y, absolute, false, heading);
        } else {
            if (!info->gliding) {
                moved = this->do_gliding_via_info(m, info, x, y, sec, sec_delta, absolute, false);
            } else {
                GMTarget target;
                MotionAction action;

                target.type = MotionTargetType::Location;
                target.body.dot = { x, y };
                
                action.type = MotionActionType::Motion;
                action.body.motion = { target, sec, sec_delta, absolute, heading };

                info->motion_actions.push_back(action);
            }
        }
    }

    return moved;
}

bool WarGrey::STEM::Plane::glide_matter_to_location_via_info(IMatter* m, MatterInfo* info, double sec, float x, float y, float fx, float fy, float dx, float dy, bool heading) {
    float sx, sy, sw, sh;
    float ax = 0.0F;
    float ay = 0.0F;
        
    unsafe_feed_matter_bound(m, info, &sx, &sy, &sw, &sh);
    ax = (sw * fx);
    ay = (sh * fy);
    
    return this->glide_matter_via_info(m, info, sec, x - ax + dx, y - ay + dy, true, heading);
}

bool WarGrey::STEM::Plane::move_matter_to_location_via_info(IMatter* m, MatterInfo* info, float x, float y, float fx, float fy, float dx, float dy) {
    return this->glide_matter_to_location_via_info(m, info, 0.0F, x, y, fx, fy, dx, dy, false);
}

void WarGrey::STEM::Plane::handle_queued_motion(IMatter* m, MatterInfo* info, float dwidth, float dheight) {
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
            unsafe_location_changed(m, info, ox, oy, false);
            this->size_cache_invalid();
            this->notify_updated();
        }
    } else {
        while (!info->motion_actions.empty()) {
            MotionAction next_move = info->motion_actions.front();

            info->motion_actions.pop_front();

            if (next_move.type == MotionActionType::Motion) {
                GlidingMotion gm = next_move.body.motion;

                if (gm.second > 0.0) {
                    if (gm.sec_delta > 0.0) {
                        if (this->do_gliding_via_info(m, info, gm.target.body.dot.x, gm.target.body.dot.y, gm.second, gm.sec_delta, gm.absolute, false)) {
                            this->notify_updated();
                            break;
                        }
                    } else {
                        if (this->do_vector_gliding(m, info, gm.target.body.length, gm.second)) {
                            this->notify_updated();
                            break;
                        }
                    }
                } else if (this->do_moving_via_info(m, info, gm.target.body.dot.x, gm.target.body.dot.y, gm.absolute, false, gm.heading)) {
                    this->notify_updated();
                }
            } else {
                unsafe_canvas_info_do_setting(this, m, info, next_move);

                if (!info->shared_canvas) {
                    unsafe_canvas_sync_settings(info);
                }
            }
        }
    }
}

bool WarGrey::STEM::Plane::do_vector_moving(IMatter* m, MatterInfo* info, double length, bool heading) {
    double x, y;

    orthogonal_decomposition(length, m->get_heading(), &x, &y);
    
    return this->move_matter_via_info(m, info, float(x), float(y), false, true, heading);
}

bool WarGrey::STEM::Plane::do_vector_gliding(IMatter* m, MatterInfo* info, double length, double sec) {
    double x, y;

    orthogonal_decomposition(length, m->get_heading(), &x, &y);
    
    return this->glide_matter_via_info(m, info, sec, float(x), float(y), false, true);
}

bool WarGrey::STEM::Plane::is_matter_found(IMatter* m, MatterInfo* info, float x, float y) {
    float sx, sy, sw, sh;
    
    unsafe_feed_matter_bound(m, info, &sx, &sy, &sw, &sh);

    sx += this->translate_x;
    sy += this->translate_y;

    return flin(sx, x, (sx + sw)) && flin(sy, y, (sy + sh))
        && m->is_colliding_with_mouse(x - sx, y - sy);
}

/*************************************************************************************************/
void WarGrey::STEM::Plane::bind_canvas(IMatter* m, WarGrey::STEM::Tracklet* canvas, MatterAnchor anchor, bool shared) {
    float fx, fy;

    matter_anchor_fraction(anchor, &fx, &fy);
    this->bind_canvas(m, canvas, fx, fy, shared);
}

void WarGrey::STEM::Plane::bind_canvas(IMatter* m, WarGrey::STEM::Tracklet* canvas, float fx, float fy, bool shared) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        info->canvas = canvas;
        info->shared_canvas = shared;
        info->draw_fx = fx;
        info->draw_fy = fy;

        if (!info->shared_canvas) {
            unsafe_canvas_sync_settings(info);
        }
    }
}

void WarGrey::STEM::Plane::reset_pen(IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        MotionAction action;

        action.type = MotionActionType::TrackReset;
        unsafe_do_canvas_setting(this, m, info, action);
    }
}

void WarGrey::STEM::Plane::stamp(IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        MotionAction action;

        action.type = MotionActionType::Stamp;
        unsafe_do_canvas_setting(this, m, info, action);
    }
}

void WarGrey::STEM::Plane::set_drawing(IMatter* m, bool yes_or_no) {
    MatterInfo* info = plane_matter_info(this, m);

    if ((info != nullptr) && (info->master != nullptr)) {
        MotionAction action;

        action.type = MotionActionType::TrackDrawing;
        action.body.drawing = yes_or_no;
        unsafe_do_canvas_setting(this, m, info, action);
    }
}

void WarGrey::STEM::Plane::set_pen_width(IMatter* m, uint8_t width) {
    MatterInfo* info = plane_matter_info(this, m);

    if ((info != nullptr) && (info->master != nullptr)) {
        MotionAction action;

        action.type = MotionActionType::PenWidth;
        action.body.pen_width = width;
        unsafe_do_canvas_setting(this, m, info, action);
    }
}

void WarGrey::STEM::Plane::set_pen_color(IMatter* m, uint32_t hex, double alpha) {
    MatterInfo* info = plane_matter_info(this, m);

    if ((info != nullptr) && (info->master != nullptr)) {
        MotionAction action;

        action.type = MotionActionType::PenColor;
        action.body.color = { hex, alpha };
        unsafe_do_canvas_setting(this, m, info, action);
    }
}

void WarGrey::STEM::Plane::set_heading(IMatter* m, double direction, bool is_radian) {
    MatterInfo* info = plane_matter_info(this, m);

    if ((info != nullptr) && (info->master != nullptr)) {
        MotionAction action;

        action.type = MotionActionType::Heading;
        action.body.direction = (is_radian ? direction : degrees_to_radians(direction));
        unsafe_do_canvas_setting(this, m, info, action);
    }
}

void WarGrey::STEM::Plane::turn(IMatter* m, double theta, bool is_radian) {
    MatterInfo* info = plane_matter_info(this, m);

    if ((info != nullptr) && (info->master != nullptr)) {
        MotionAction action;

        action.type = MotionActionType::Rotation;
        action.body.theta = (is_radian ? theta : degrees_to_radians(theta));
        unsafe_do_canvas_setting(this, m, info, action);
    }
}

void WarGrey::STEM::Plane::set_pen_color(IMatter* m, double hue, double saturation, double brightness, double alpha) {
    this->set_pen_color(m, Hexadecimal_From_HSV(hue, saturation, brightness), alpha);
}

/*************************************************************************************************/
void WarGrey::STEM::Plane::log_message(int fgc, const std::string& msg) {
    if (this->sentry != nullptr) {
        this->sentry->say(2.0, msg, uint32_t(fgc));
    } else {
        IPlane::log_message(fgc, msg);
    }
}

void WarGrey::STEM::Plane::say(ISprite* m, double sec, IMatter* message, SpeechBubble type) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        if (message == nullptr) {
            this->shh(m);
        } else {
            if ((info->bubble == nullptr) || (info->bubble != message)) {
                if (message->info == nullptr) {
                    this->handle_new_matter(message, bind_speech_owership(this, message));
                } else {
                    auto sinfo = dynamic_cast<SpeechInfo*>(message->info);

                    if (sinfo != nullptr) {
                        sinfo->counter_increase();
                    }
                }

                if (info->bubble != nullptr) {
                    auto sinfo = dynamic_cast<SpeechInfo*>(info->bubble->info);

                    if (sinfo != nullptr) {
                        sinfo->counter_decrease(info->bubble);
                    }
                }
        
                info->bubble = message;
            }

            bubble_start(m, info, sec, type, this->bubble_second);
        }
    }
}

void WarGrey::STEM::Plane::say(ISprite* m, double sec, const std::string& message, uint32_t color, SpeechBubble type) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        if (message.empty()) {
            this->shh(m);
        } else if (this->merge_bubble_text(info->bubble, message, color, 1.0)) {
            bubble_start(m, info, sec, type, this->bubble_second);
        } else {
            this->say(m, sec, this->make_bubble_text(message, color, 1.0), type);
        }
    }
}

void WarGrey::STEM::Plane::shh(ISprite* m) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        if (this->in_speech(m)) {
            bubble_expire(m, info);
        }
    }
}

IMatter* WarGrey::STEM::Plane::make_bubble_text(const std::string& message, uint32_t color, double alpha) {
    return new Labellet(this->bubble_font, color, alpha, "%s", message.c_str());
}

bool WarGrey::STEM::Plane::merge_bubble_text(IMatter* bubble, const std::string& message, uint32_t color, double alpha) {
    auto bmsg = dynamic_cast<ITextlet*>(bubble);
    bool okay = (bmsg != nullptr);

    if (okay) {
        bmsg->set_text_color(color);
        bmsg->set_text(message);
    }

    return okay;
}

bool WarGrey::STEM::Plane::is_bubble_showing(IMatter* m, SpeechBubble* type) {
    MatterInfo* info = plane_matter_info(this, m);
    bool yes = is_matter_bubble_showing(m, info);

    if (yes) {
        SET_BOX(type, info->bubble_type);
    }

    return yes;
}

void WarGrey::STEM::Plane::place_speech_bubble(IMatter* m, float bubble_width, float bubble_height, float Width, float Height
        , float* mfx, float* mfy, float* bfx, float* bfy, float* dx, float* dy) {
    SET_BOXES(mfx, mfy, 0.0F);
    SET_VALUES(bfx, 0.618F, bfy, 1.0);
    SET_BOXES(dx, dy, 0.0F);
}

void WarGrey::STEM::Plane::set_bubble_duration(double second) {
    if (second <= 0.0) {
        this->set_bubble_duration();
    } else {
        this->bubble_second = second;
    }
}

void WarGrey::STEM::Plane::set_bubble_margin(float top, float right, float bottom, float left) {
    this->bubble_top_margin = top;
    this->bubble_right_margin = right;
    this->bubble_bottom_margin = bottom;
    this->bubble_left_margin = left;
}

void WarGrey::STEM::Plane::set_bubble_color(uint32_t border, uint32_t background, double alpha) {
    this->bubble_border = border;
    this->bubble_color = background;
    this->bubble_alpha = alpha;
}

void WarGrey::STEM::Plane::delete_matter(IMatter* m) {
    // m's destructor will delete the associated info object
    delete m;
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

bool WarGrey::STEM::IPlane::is_colliding_with_mouse(IMatter* m) {
    float slx, sty, sw, sh, mx, my;
    bool mokay = this->feed_matter_boundary(m, &slx, &sty, &sw, &sh);

    if (mokay) {
        feed_current_mouse_location(&mx, &my);
        mokay = rectangle_contain(slx, sty, slx + sw, sty + sh, mx, my)
                && m->is_colliding_with_mouse(mx - slx, my - sty);
    }
    
    return mokay;
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

/*************************************************************************************************/
bool WarGrey::STEM::IPlane::in_speech(ISprite* m) {
    return this->is_bubble_showing(m, nullptr);
}

bool WarGrey::STEM::IPlane::is_speaking(ISprite* m) {
    SpeechBubble type;
    bool showing = this->is_bubble_showing(m, &type);

    return showing && (type == SpeechBubble::Default);
}

bool WarGrey::STEM::IPlane::is_thinking(ISprite* m) {
    SpeechBubble type;
    bool showing = this->is_bubble_showing(m, &type);

    return showing && (type == SpeechBubble::Thought);
}

void WarGrey::STEM::IPlane::say(ISprite* m, const char* sentence, uint32_t color) {
    if (sentence == nullptr) {
        this->shh(m);
    } else {
        this->say(m, std::string(sentence), color);
    }
}

void WarGrey::STEM::IPlane::say(ISprite* m, const std::string& sentence, uint32_t color) {
    if (sentence.empty()) {
        this->shh(m);
    } else {
        this->say(m, 0.0, sentence, color, SpeechBubble::Default);
    }
}

void WarGrey::STEM::IPlane::say(ISprite* m, uint32_t color, const char* fmt, ...) {
    VSNPRINT(sentence, fmt);
    this->say(m, sentence, color);
}

void WarGrey::STEM::IPlane::say(ISprite* m, double sec, const char* sentence, uint32_t color) {
    if (sentence == nullptr) {
        this->shh(m);
    } else {
        this->say(m, sec, std::string(sentence), color);
    }
}

void WarGrey::STEM::IPlane::say(ISprite* m, double sec, const std::string& sentence, uint32_t color) {
    if (sentence.empty()) {
        this->shh(m);
    } else {
        this->say(m, sec, sentence, color, SpeechBubble::Default);
    }
}

void WarGrey::STEM::IPlane::say(ISprite* m, double sec, uint32_t color, const char* fmt, ...) {
    VSNPRINT(sentence, fmt);
    this->say(m, sec, sentence, color);
}

void WarGrey::STEM::IPlane::think(ISprite* m, const char* sentence, uint32_t color) {
    if (sentence == nullptr) {
        this->shh(m);
    } else {
        this->think(m, std::string(sentence), color);
    }
}

void WarGrey::STEM::IPlane::think(ISprite* m, const std::string& sentence, uint32_t color) {
    if (sentence.empty()) {
        this->shh(m);
    } else {
        this->say(m, 0.0, sentence, color, SpeechBubble::Thought);
    }
}

void WarGrey::STEM::IPlane::think(ISprite* m, uint32_t color, const char* fmt, ...) {
    VSNPRINT(sentence, fmt);
    this->think(m, sentence, color);
}

void WarGrey::STEM::IPlane::think(ISprite* m, double sec, const char* sentence, uint32_t color) {
    if (sentence == nullptr) {
        this->shh(m);
    } else {
        this->think(m, sec, std::string(sentence), color);
    }
}

void WarGrey::STEM::IPlane::think(ISprite* m, double sec, const std::string& sentence, uint32_t color) {
    if (sentence.empty()) {
        this->shh(m);
    } else {
        this->say(m, sec, sentence, color, SpeechBubble::Thought);
    }
}

void WarGrey::STEM::IPlane::think(ISprite* m, double sec, uint32_t color, const char* fmt, ...) {
    VSNPRINT(sentence, fmt);
    this->think(m, sec, sentence, color);
}
