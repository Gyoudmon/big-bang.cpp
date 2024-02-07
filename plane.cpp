#include "plane.hpp"
#include "matter.hpp"
#include "misc.hpp"

#include "graphics/ruler.hpp"

#include "matter/sprite.hpp"
#include "matter/graphlet/textlet.hpp"
#include "matter/graphlet/tracklet.hpp"

#include "physics/color/rgba.hpp"
#include "physics/algebra/point.hpp"
#include "physics/mathematics.hpp"
#include "physics/random.hpp"

#include "datum/string.hpp"
#include "datum/flonum.hpp"
#include "datum/fixnum.hpp"
#include "datum/box.hpp"
#include "datum/time.hpp"

#include <deque>

using namespace GYDM;

/** NOTE
 *   C-Style casting tries all C++ style casting except dynamic_cast;
 *   `static_cast` almost costs nothing for subtype casting;
 *   `reinterpret_cast` might be harmful for multi-inheritances.
 */
#define MATTER_INFO(m) (static_cast<MatterInfo*>(m->info))
#define SPEECH_INFO(m) (static_cast<SpeechInfo*>(m->info))

namespace GYDM {
    enum class MotionActionType { Motion, TrackReset, TrackDrawing, PenColor, PenWidth, Heading, Rotation, Stamp };

    struct GlidingMotion {
        GlidingMotion(double length, double second, bool absolute, bool heading)
            : second(second), sec_delta(0.0), absolute(absolute), heading(heading), length(length) {}

        GlidingMotion(const Position& pos, double second, double delta, bool absolute, bool heading)
            : second(second), sec_delta(delta), absolute(absolute), heading(heading), target(pos) {}

        GlidingMotion(const GlidingMotion& m) { this->operator=(m); }

        GlidingMotion& operator=(const GlidingMotion& m) {
            if (m.sec_delta == 0.0) {
                this->length = m.length;
            } else {
                this->target = m.target;
            }

            this->second = m.second;
            this->sec_delta = m.sec_delta;
            this->absolute = m.absolute;
            this->heading = m.heading;

            return (*this);
        }

        ~GlidingMotion() noexcept {}

        double second;
        double sec_delta; // 0.0 means the target is based on current heading
        bool absolute;
        bool heading;     // moving only

        union {
            Position target;
            double length;
        };
    };

    struct MotionAction {
        MotionAction(const MotionAction& a) : type(a.type) {
            switch (a.type) {
            case MotionActionType::Motion: this->motion = a.motion; break;
            case MotionActionType::Heading: this->direction = a.direction; break;
            case MotionActionType::Rotation: this->theta = a.theta; break;
            case MotionActionType::TrackDrawing: this->drawing = a.drawing; break;
            case MotionActionType::PenWidth: this->pen_width = a.pen_width; break;
            case MotionActionType::PenColor: this->color = a.color; break;
            default: this->unused = true;
            }
        }

        MotionAction(MotionActionType type, double datum, bool is_radian) : type(type) {
            switch (this->type) {
            case MotionActionType::Heading: this->direction = (is_radian ? datum : degrees_to_radians(datum)); break;
            case MotionActionType::Rotation: this->theta = (is_radian ? datum : degrees_to_radians(datum)); break;
            default: this->unused = true;
            }
        }

        MotionAction(MotionActionType type) : type(type), unused(true) {}
        MotionAction(bool drawing) : type(MotionActionType::TrackDrawing), drawing(drawing) {}
        MotionAction(uint8_t width) : type(MotionActionType::PenWidth), pen_width(width) {}
        MotionAction(const RGBA& c) : type(MotionActionType::PenColor), color(c) {}
        MotionAction(const GlidingMotion& m) : type(MotionActionType::Motion), motion(m) {}
        ~MotionAction() noexcept {}

        MotionActionType type;
        
        union {
            GlidingMotion motion;
            bool drawing;
            double direction;
            double theta;
            uint8_t pen_width;
            RGBA color;
            bool unused;
        };
    };

    struct MatterInfo : public GYDM::IMatterInfo {
        MatterInfo(GYDM::IPlane* master) : IMatterInfo(master) {}
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
        Port draw_port = 0.5F;
        RGBA draw_color = 0x0U;
        uint8_t draw_width = 1U;

        // gliding progressbar
        double current_step = 1.0;
        double progress_total = 1.0;

        // as linked list
        IMatter* next = nullptr;
        IMatter* prev = nullptr;
    };

    class SpeechInfo : public GYDM::IMatterInfo {
    public:
        SpeechInfo(GYDM::IPlane* master) : IMatterInfo(master) {}
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

    GYDM::MatterInfo::~MatterInfo() noexcept {
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
    info->draw_width = 1U;
}

static inline void unsafe_canvas_sync_settings(MatterInfo* info) {
    info->canvas->set_drawing(info->is_drawing);
    info->canvas->set_pen_width(info->draw_width);
    info->canvas->set_pen_color(info->draw_color);
}

static void unsafe_canvas_info_do_setting(IPlane* self, IMatter* m, MatterInfo* info, const MotionAction& op) {
    switch (op.type) {
    case MotionActionType::PenColor: info->draw_color = op.color; break;
    case MotionActionType::TrackDrawing: info->is_drawing = op.drawing; break;
    case MotionActionType::PenWidth: info->draw_width = op.pen_width; break;
    case MotionActionType::TrackReset: unsafe_canvas_info_reset(info); break;
    case MotionActionType::Rotation: m->add_heading(op.theta, true); break;
    case MotionActionType::Heading: m->set_heading(op.direction, true); break;
    case MotionActionType::Stamp: {
        Dot dot = self->get_matter_location(m, MatterPort::LT);

        info->canvas->stamp(m, dot.x, dot.y);
     }; break;
    default: /* ignored */;
    }
}

static void unsafe_do_canvas_setting(Plane* self, IMatter* m, MatterInfo* info, const MotionAction& op) {
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
    if (!info->draw_port.is_zero()) {
        Dot dot = m->get_bounding_box().point_at(info->draw_port);

        info->canvas->add_line(x1 + dot.x, y1 + dot.y, x2 + dot.x, y2 + dot.y);
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

static inline MatterInfo* bind_matter_ownership(IPlane* master, IMatter* m) {
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

static inline Box unsafe_get_matter_bound(IMatter* m, MatterInfo* info) {
    return m->get_bounding_box() + Dot(info->x, info->y);
}

static inline void unsafe_add_selected(GYDM::IPlane* master, IMatter* m, MatterInfo* info, bool selected) {
    master->on_select(m, selected);
    info->selected = selected;
    master->after_select(m, selected);
    master->notify_updated();
}

static inline void unsafe_set_selected(GYDM::IPlane* master, IMatter* m, MatterInfo* info) {
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

void GYDM::Plane::notify_matter_ready(IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        this->size_cache_invalid();
        this->begin_update_sequence();
        this->notify_updated();
        this->on_matter_ready(m);
        this->end_update_sequence();
    }
}

void GYDM::Plane::bring_to_front(IMatter* m, IMatter* target) {
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

void GYDM::Plane::bring_forward(IMatter* m, int n) {
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

void GYDM::Plane::send_to_back(IMatter* m, IMatter* target) {
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

void GYDM::Plane::send_backward(IMatter* m, int n) {
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

void GYDM::Plane::insert_at(IMatter* m, const Position& pos, const Port& p, const Vector& vec) {
    if (m->info == nullptr) {
        MatterInfo* info = bind_matter_ownership(this, m);
        
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

        this->handle_new_matter(m, info, pos, p, vec.x, vec.y);
    }
}

void GYDM::Plane::insert_as_speech_bubble(IMatter* m) {
    if (m->info == nullptr) {
        SpeechInfo* info = bind_speech_owership(this, m);

        info->next = this->head_speech;
        this->head_speech = m;

        this->handle_new_matter(m, info);
    }
}

void GYDM::Plane::handle_new_matter(IMatter* m, MatterInfo* info, const Position& pos, const Port& p, float dx, float dy) {
    this->begin_update_sequence();
    m->construct(this->drawing_context());
    this->move_matter_to_location_via_info(m, info, pos, p, dx, dy);
    
    if (m->ready()) {
        this->on_matter_ready(m);
    }
    
    this->notify_updated();
    this->end_update_sequence();
}


void GYDM::Plane::handle_new_matter(IMatter* m, SpeechInfo* info) {
    this->begin_update_sequence();
    m->construct(this->drawing_context());
    this->notify_updated();
    this->end_update_sequence();
}

void GYDM::Plane::remove(IMatter* m, bool needs_delete) {
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

void GYDM::Plane::erase() {
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

void GYDM::Plane::move(IMatter* m, double length, bool ignore_gliding) {
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

void GYDM::Plane::move(IMatter* m, const Vector& vec, bool ignore_gliding) {
    if (m != nullptr) {
        MatterInfo* info = plane_matter_info(this, m);

        if (info != nullptr) {
            if (this->move_matter_via_info(m, info, vec, false, ignore_gliding, false)) {
                this->notify_updated();
            }
        }
    } else if (this->head_matter != nullptr) {
        IMatter* child = this->head_matter;

        do {
            MatterInfo* info = MATTER_INFO(child);

            if (info->selected) {
                this->move_matter_via_info(child, info, vec, false, ignore_gliding, false);
            }

            child = info->next;
        } while (child != this->head_matter);

        this->notify_updated();
    }
}

void GYDM::Plane::move_to(IMatter* m, const Position& pos, const Port& p, const Vector& vec) {
    MatterInfo* info = plane_matter_info(this, m);
    
    if (info != nullptr) {
        if (this->move_matter_to_location_via_info(m, info, pos, p, vec.x, vec.y)) {
            this->notify_updated();
        }
    }
}

void GYDM::Plane::glide(double sec, IMatter* m, double length) {
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

void GYDM::Plane::glide(double sec, IMatter* m, const Vector& vec) {
    if (m != nullptr) {
        MatterInfo* info = plane_matter_info(this, m);

        if (info != nullptr) {
            this->glide_matter_via_info(m, info, sec, vec, false, true);
        }
    } else if (this->head_matter != nullptr) {
        IMatter* child = this->head_matter;

        do {
            MatterInfo* info = MATTER_INFO(child);

            if (info->selected) {
                this->glide_matter_via_info(child, info, sec, vec, false, true);
            }

            child = info->next;
        } while (child != this->head_matter);
    }
}

void GYDM::Plane::glide_to(double sec, IMatter* m, const Position& pos, const Port& p, const Vector& vec) {
    MatterInfo* info = plane_matter_info(this, m);
    
    if (info != nullptr) {
        this->glide_matter_to_location_via_info(m, info, sec, pos, p, vec.x, vec.y, true);
    }
}

void GYDM::Plane::clear_motion_actions(IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        info->motion_actions.clear();
    }
}

IMatter* GYDM::Plane::find_matter(const Position& pos, IMatter* after) {
    IMatter* found = nullptr;

    if (this->head_matter != nullptr) {
        MatterInfo* head_info = MATTER_INFO(this->head_matter);
        MatterInfo* aftr_info = plane_matter_info(this, after);
        IMatter* child = (aftr_info == nullptr) ? head_info->prev : aftr_info->prev;
        Dot dot = pos.calculate_point();

        do {
            MatterInfo* info = MATTER_INFO(child);

            if (child->visible() && !child->concealled()) {
                if (this->is_matter_found(child, info, dot)) {
                    found = child;
                    break;
                }
            }

            child = info->prev;
        } while (child != head_info->prev);
    }

    return found;
}

IMatter* GYDM::Plane::find_matter(IMatter* collided_matter, IMatter* after) {
    IMatter* found = nullptr;
    Box self = this->get_matter_bounding_box(collided_matter);

    if ((!self.is_empty()) && (this->head_matter != nullptr)) {
        MatterInfo* head_info = MATTER_INFO(this->head_matter);
        MatterInfo* aftr_info = plane_matter_info(this, after);
        IMatter* child = (aftr_info == nullptr) ? head_info->prev : aftr_info->prev;

        do {
            MatterInfo* info = MATTER_INFO(child);

            if (child->visible() && !child->concealled()) {
                Box box = unsafe_get_matter_bound(child, info);

                if (self.overlay(box) && (collided_matter != child)) {
                    found = child;
                    break;
                }
            }

            child = info->prev;
        } while (child != head_info->prev);
    }

    return found;
}

IMatter* GYDM::Plane::find_least_recent_matter(const Dot& pos) {
    IMatter* found = nullptr;
    uint32_t found_hit = 0xFFFFFFFFU;

    if (this->head_matter != nullptr) {
        MatterInfo* head_info = MATTER_INFO(this->head_matter);
        IMatter* child = head_info->prev;

        do {
            MatterInfo* info = MATTER_INFO(child);

            if (child->visible() && !child->concealled()) {
                if (this->is_matter_found(child, info, pos)) {
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
IMatter* GYDM::Plane::find_matter_for_tooltip(const Dot& pos) {
    IMatter* found = nullptr;

    if (this->head_matter != nullptr) {
        MatterInfo* head_info = MATTER_INFO(this->head_matter);
        IMatter* child = head_info->prev;

        do {
            MatterInfo* info = MATTER_INFO(child);

            if (child->visible()) {
                if (this->is_matter_found(child, info, pos)) {
                    found = child;
                    break;
                }
            }

            child = info->prev;
        } while (child != head_info->prev);
    }

    return found;
}

IMatter* GYDM::Plane::find_next_selected_matter(IMatter* start) {
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

GYDM::Dot GYDM::Plane::get_matter_location(IMatter* m, const Port& a) {
    MatterInfo* info = plane_matter_info(this, m);
    Dot dot(flnan_f, flnan_f);
    
    if (info != nullptr) {
        dot = unsafe_get_matter_bound(m, info).point_at(a);
    }

    return dot;
}

GYDM::Box GYDM::Plane::get_matter_bounding_box(IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);
    Box box;
    
    if (info != nullptr) {
        box = unsafe_get_matter_bound(m, info);
    }

    return box;
}

GYDM::Box GYDM::Plane::get_bounding_box() {
    this->recalculate_matters_extent_when_invalid();

    return this->extent;
}

void GYDM::Plane::size_cache_invalid() {
    this->extent.invalidate();
}

void GYDM::Plane::recalculate_matters_extent_when_invalid() {
    if (this->extent.is_empty()) {
        if (this->head_matter == nullptr) {
            this->extent *= 0.0F;
        } else {
            IMatter* child = this->head_matter;

            do {
                MatterInfo* info = MATTER_INFO(child);
                
                this->extent += unsafe_get_matter_bound(child, info);
                child = info->next;
            } while (child != this->head_matter);
        }
    }
}

void GYDM::Plane::add_selected(IMatter* m) {
    if (this->can_select_multiple()) {
        MatterInfo* info = plane_matter_info(this, m);

        if ((info != nullptr) && (!info->selected)) {
            if (this->can_select(m)) {
                unsafe_add_selected(this, m, info, true);
            }
        }
    }
}

void GYDM::Plane::remove_selected(IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);

    if ((info != nullptr) && (info->selected)) {
        unsafe_add_selected(this, m, info, false);
    }
}

void GYDM::Plane::set_selected(IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);

    if ((info != nullptr) && (!info->selected)) {
        if (this->can_select(m)) {
            unsafe_set_selected(this, m, info);
        }
    }
}

void GYDM::Plane::no_selected() {
    this->no_selected_except(nullptr);
}

void GYDM::Plane::no_selected_except(IMatter* m) {
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

bool GYDM::Plane::is_selected(IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);
    bool selected = false;

    if (info != nullptr) {
        selected = info->selected;
    }

    return selected;
}

size_t GYDM::Plane::count_selected() {
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

IMatter* GYDM::Plane::get_focused_matter() {
    return this->focused_matter;
}

void GYDM::Plane::set_caret_owner(IMatter* m) {
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
void GYDM::Plane::on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) {
    if (this->focused_matter != nullptr) {
        this->focused_matter->on_char(key, modifiers, repeats, pressed);
    }
}

void GYDM::Plane::on_text(const char* text, size_t size, bool entire) {
    if (this->focused_matter != nullptr) {
        this->focused_matter->on_text(text, size, entire);
    }
}

void GYDM::Plane::on_editing_text(const char* text, int pos, int span) {
    if (this->focused_matter != nullptr) {
        this->focused_matter->on_editing_text(text, pos, span);
    }
}

void GYDM::Plane::on_tap(IMatter* m, float local_x, float local_y) {
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

void GYDM::Plane::on_tap_selected(IMatter* m, float local_x, float local_y) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        unsafe_add_selected(this, m, info, false);
    }
}

bool GYDM::Plane::on_pointer_pressed(uint8_t button, float x, float y, uint8_t clicks) {
    bool handled = false;

    switch (button) {
    case SDL_BUTTON_LEFT: {
        Dot pos = Dot(x, y) - this->translate;
        IMatter* self_matter = this->find_matter(pos, static_cast<IMatter*>(nullptr));

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

bool GYDM::Plane::on_pointer_move(uint32_t state, float x, float y, float dx, float dy) {
    bool handled = false;

    if (state == 0) {
        Dot pos = Dot(x, y) - this->translate;
        IMatter* self_matter = this->find_matter_for_tooltip(pos);

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
                this->hovering_gm = { x,  y };
                this->hovering_lm = { local_x, local_y };

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

bool GYDM::Plane::on_pointer_released(uint8_t button, float x, float y, uint8_t clicks) {
    bool handled = false;

    switch (button) {
    case SDL_BUTTON_LEFT: {
        Dot pos = Dot(x, y) - this->translate;
        IMatter* self_matter = this->find_least_recent_matter(pos);

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

bool GYDM::Plane::on_scroll(int horizon, int vertical, float hprecise, float vprecise) {
    bool handled = false;

    return handled;
}

bool GYDM::Plane::say_goodbye_to_hover_matter(uint32_t state, float x, float y, float dx, float dy) {
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

void GYDM::Plane::on_enter(IPlane* from) {
    this->mission_done = false;

    if (this->sentry != nullptr) {
        this->sentry->play_greeting(1);
    }

    IPlane::on_enter(from);
}

void GYDM::Plane::mission_complete() {
    if (this->sentry != nullptr) {
        this->sentry->play_goodbye(1);
        this->sentry->stop(1);
    }

    this->on_mission_complete();
    this->mission_done = true;
}

bool GYDM::Plane::has_mission_completed() {
    return this->mission_done &&
            ((this->sentry == nullptr) || !this->sentry->in_playing());
}

bool GYDM::Plane::can_select(IMatter* m) {
    return this->sentry == m;
}

void GYDM::Plane::set_tooltip_matter(IMatter* m, const Vector& offset) {
    this->begin_update_sequence();

    if ((this->tooltip != nullptr) && !this->tooltip->visible()) {
        this->tooltip->show(true);
    }

    this->tooltip = m;
    this->tooltip->show(false);
    this->tooltip_offset = offset;

    this->end_update_sequence();
}

void GYDM::Plane::place_tooltip(GYDM::IMatter* target) {
    float width, height;
    Dot tt;

    this->move_to(this->tooltip, Position(target, MatterPort::LB), MatterPort::LT, this->tooltip_offset);

    this->master()->feed_client_extent(&width, &height);
    tt = this->get_matter_location(this->tooltip, MatterPort::LB);

    if (tt.y > height) {
        this->move_to(this->tooltip, Position(target, MatterPort::LT), MatterPort::LB, this->tooltip_offset);
    }

    if (tt.x < 0.0F) {
        this->move(this->tooltip, Vector(-tt.x, 0.0F));
    } else {
        tt = this->get_matter_location(this->tooltip, MatterPort::RB);

        if (tt.x > width) {
            this->move(this->tooltip, Vector(width - tt.x, 0.0F));
        }
    }
}

/************************************************************************************************/
void GYDM::Plane::set_matter_fps(IMatter* m, int fps, bool restart) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        unsafe_set_matter_fps(info, fps, restart);
    }
}

void GYDM::Plane::set_local_fps(int fps, bool restart) {
    unsafe_set_local_fps(fps, restart, this->local_frame_delta, this->local_frame_count, this->local_elapse);
}


void GYDM::Plane::notify_matter_timeline_restart(IMatter* m, uint32_t count0, int duration) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        info->duration = duration;
        reset_timeline(info->local_frame_count, info->local_elapse, count0);
    }
}

void GYDM::Plane::on_elapse(uint64_t count, uint32_t interval, uint64_t uptime) {
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
                this->update_tooltip(this->hovering_matter, this->hovering_lm.x, this->hovering_lm.y, this->hovering_gm.x, this->hovering_gm.y);
                this->place_tooltip(this->hovering_matter);
            }
        }
    }
}

void GYDM::Plane::draw(dc_t* dc, float X, float Y, float Width, float Height) {
    float dsX = flmax(0.0F, X);
    float dsY = flmax(0.0F, Y);
    float dsWidth = X + Width;
    float dsHeight = Y + Height;
    
    if (this->background.is_opacity()) {
        dc->fill_rect(dsX, dsY, dsWidth, dsHeight, this->background);
    }

    if (this->grid_color.is_opacity()
            && (this->column > 0) && (this->row > 0)
            && (this->cell_width > 0.0F) && (this->cell_height > 0.0F)) {
        dc->draw_grid(this->row, this->column,
                        this->cell_width, this->cell_height,
                        this->grid_color,
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

                this->draw_matter(dc, child, info, X, Y, dsX, dsY, dsWidth, dsHeight);
            }

            child = info->next;
        } while (child != this->head_matter);

        if (speech_head != nullptr) {
            child = speech_head;

            do {
                info = MATTER_INFO(child);
                this->draw_speech(dc, child, info, Width, Height, X, Y, dsX, dsY, dsWidth, dsHeight);
                child = info->next;
            } while (child != speech_nil);
        }

        if (this->tooltip != nullptr) {
            this->draw_matter(dc, this->tooltip, MATTER_INFO(this->tooltip), X, Y, dsX, dsY, dsWidth, dsHeight);
        }

        dc->clear_clipping_region();
    }
}

void GYDM::Plane::draw_visible_selection(dc_t* dc, float x, float y, float width, float height) {
    dc->draw_rect(x, y, width, height, 0x00FFFFU);
}

void GYDM::Plane::draw_matter(dc_t* dc, IMatter* child, MatterInfo* info, float X, float Y, float dsX, float dsY, float dsWidth, float dsHeight) {
    float mx, my;
    SDL_Rect clip;
    
    if (child->visible()) {
        Box box = child->get_bounding_box();
        float mwidth = box.width();
        float mheight = box.height();

        mx = (info->x + this->translate.x) + X;
        my = (info->y + this->translate.y) + Y;
                
        if (rectangle_overlay(mx, my, mx + mwidth, my + mheight, dsX, dsY, dsWidth, dsHeight)) {
            clip.x = fl2fxi(flfloor(mx));
            clip.y = fl2fxi(flfloor(my));
            clip.w = fl2fxi(flceiling(mwidth));
            clip.h = fl2fxi(flceiling(mheight));

            dc->set_clipping_region(&clip);

            if (child->ready()) {
                child->draw(dc, mx, my, mwidth, mheight);
            } else {
                child->draw_in_progress(dc, mx, my, mwidth, mheight);
            }

            if (info->selected) {
                dc->clear_clipping_region();
                this->draw_visible_selection(dc, mx, my, mwidth, mheight);
            }
        }
    }
}

void GYDM::Plane::draw_speech(dc_t* dc, IMatter* child, MatterInfo* info, float Width, float Height, float X, float Y, float dsX, float dsY, float dsWidth, float dsHeight) {
    if (is_matter_bubble_showing(child, info)) {
        float ix, iy, bx, by, bwidth, bheight, dx, dy;
        Port mp, bp;
        SDL_Rect clip;

        Box cbox = child->get_bounding_box();
        Box ibox = info->bubble->get_bounding_box();
        float mwidth = cbox.width();
        float mheight = cbox.height();
        float iwidth = ibox.width();
        float iheight = ibox.height();
        
        bwidth =  iwidth +  this->bubble_margin.horizon();
        bheight = iheight + this->bubble_margin.vertical();
        this->place_speech_bubble(child, bwidth, bheight, Width, Height, &mp, &bp, &dx, &dy);
        bx = info->x + mwidth  * mp.fx - bwidth  * bp.fx + dx;
        by = info->y + mheight * mp.fy - bheight * bp.fy + dy;
        bx = (bx + this->translate.x) + X;
        by = (by + this->translate.y) + Y;

        if (rectangle_overlay(bx, by, bx + bwidth, by + bheight, dsX, dsY, dsWidth, dsHeight)) {
            bx = flmax(bx, this->bubble_margin.left + X);
            by = flmax(by, this->bubble_margin.top  + Y);

            ix = bx + this->bubble_margin.left;
            iy = by + this->bubble_margin.top;
        
            clip.x = fl2fxi(flfloor(ix));
            clip.y = fl2fxi(flfloor(iy));
            clip.w = fl2fxi(flceiling(iwidth));
            clip.h = fl2fxi(flceiling(iheight));

            dc->clear_clipping_region();

            dc->fill_rounded_rect(bx, by, bwidth, bheight, -0.25F, this->bubble_color);
            dc->draw_rounded_rect(bx, by, bwidth, bheight, -0.25F, this->bubble_border);

            dc->set_clipping_region(&clip);
            
            if (info->bubble->ready()) {
                info->bubble->draw(dc, ix, iy, iwidth, iheight);
            } else {
                info->bubble->draw_in_progress(dc, ix, iy, iwidth, iheight);
            }
        }
    }
}

/*************************************************************************************************/
bool GYDM::Plane::do_moving_via_info(IMatter* m, MatterInfo* info, const Position& pos, bool absolute, bool ignore_track, bool heading) {
    bool moved = false;
    Dot dot = pos.calculate_point();
    float x = dot.x;
    float y = dot.y;
    
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

bool GYDM::Plane::do_gliding_via_info(IMatter* m, MatterInfo* info, const Position& pos, double sec, double sec_delta, bool absolute, bool ignore_track) {
    bool moved = false;
    Dot dot = pos.calculate_point();
    float x = dot.x;
    float y = dot.y;
    
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

bool GYDM::Plane::move_matter_via_info(IMatter* m, MatterInfo* info, double length, bool ignore_gliding, bool heading) {
    bool moved = false;

    if ((!info->gliding) || ignore_gliding) {
        moved = this->do_vector_moving(m, info, length, heading);
    } else {
        double x, y;

        orthogonal_decompose(length, m->get_heading(), &x, &y);
    
        return this->move_matter_via_info(m, info, Position(x, y), false, ignore_gliding, heading);
    }

    return moved;
}

bool GYDM::Plane::move_matter_via_info(IMatter* m, MatterInfo* info, const Position& pos, bool absolute, bool ignore_gliding, bool heading) {
    bool moved = false;

    if ((!info->gliding) || ignore_gliding) {
        moved = this->do_moving_via_info(m, info, pos, absolute, false, heading);
    } else if (m == this->tooltip) {
        moved = this->do_moving_via_info(m, info, pos, absolute, true, heading);
    } else {
        info->motion_actions.push_back(MotionAction(GlidingMotion(pos, 0.0, 0.0, absolute, heading)));
    }

    return moved;
}

bool GYDM::Plane::glide_matter_via_info(IMatter* m, MatterInfo* info, double sec, double length) {
    bool moved = false;

    if (!info->gliding) {
        this->do_vector_gliding(m, info, length, sec);
    } else {
        info->motion_actions.push_back(MotionAction(GlidingMotion(length, sec, false, true)));
    }
    
    return moved;
}

bool GYDM::Plane::glide_matter_via_info(IMatter* m, MatterInfo* info, double sec, const Position& pos, bool absolute, bool heading) {
    bool moved = false;
    
    if (sec <= 0.0F) {
        moved = this->move_matter_via_info(m, info, pos, absolute, false, heading);
    } else {
        IScreen* screen = this->master();
        double sec_delta = (screen != nullptr) ? (1.0 / double(screen->frame_rate())) : 0.0;

        if ((sec <= sec_delta) || (sec_delta == 0.0)) {
            moved = this->move_matter_via_info(m, info, pos, absolute, false, heading);
        } else {
            if (!info->gliding) {
                moved = this->do_gliding_via_info(m, info, pos, sec, sec_delta, absolute, false);
            } else {
                info->motion_actions.push_back(MotionAction(GlidingMotion(pos, sec, sec_delta, absolute, heading)));
            }
        }
    }

    return moved;
}

bool GYDM::Plane::glide_matter_to_location_via_info(IMatter* m, MatterInfo* info, double sec, const Position& pos, const Port& p, float dx, float dy, bool heading) {
    float xoff = dx;
    float yoff = dy;
    bool moved = false;

    if (!p.is_zero()) {    
        Box box = unsafe_get_matter_bound(m, info);

        xoff -= box.width() * p.fx;
        yoff -= box.height() * p.fy;
    }

    if ((xoff == 0.0F) && (yoff == 0.0F)) {
        moved = this->glide_matter_via_info(m, info, sec, pos, true, heading);
    } else {
        Position position = pos;

        position.set_offset({ xoff, yoff });
        moved = this->glide_matter_via_info(m, info, sec, position, true, heading);
    }

    return moved;
}

bool GYDM::Plane::move_matter_to_location_via_info(IMatter* m, MatterInfo* info, const Position& pos, const Port& p, float dx, float dy) {
    return this->glide_matter_to_location_via_info(m, info, 0.0F, pos, p, dx, dy, false);
}

void GYDM::Plane::handle_queued_motion(IMatter* m, MatterInfo* info, float dwidth, float dheight) {
    if (!m->motion_stopped()) {
        Box box = m->get_bounding_box();
        float cwidth = box.width();
        float cheight = box.height();
        double xspd = m->x_speed();
        double yspd = m->y_speed();
        float ox = info->x;
        float oy = info->y;
        float hdist, vdist;
        
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
                GlidingMotion gm = next_move.motion;

                if (gm.second > 0.0) {
                    if (gm.sec_delta > 0.0) {
                        if (this->do_gliding_via_info(m, info, gm.target, gm.second, gm.sec_delta, gm.absolute, false)) {
                            this->notify_updated();
                            break;
                        }
                    } else {
                        if (this->do_vector_gliding(m, info, gm.length, gm.second)) {
                            this->notify_updated();
                            break;
                        }
                    }
                } else if (this->do_moving_via_info(m, info, gm.target, gm.absolute, false, gm.heading)) {
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

bool GYDM::Plane::do_vector_moving(IMatter* m, MatterInfo* info, double length, bool heading) {
    double x, y;

    orthogonal_decompose(length, m->get_heading(), &x, &y);
    
    return this->move_matter_via_info(m, info, Position(x, y), false, true, heading);
}

bool GYDM::Plane::do_vector_gliding(IMatter* m, MatterInfo* info, double length, double sec) {
    double x, y;

    orthogonal_decompose(length, m->get_heading(), &x, &y);
    
    return this->glide_matter_via_info(m, info, sec, Position(x, y), false, true);
}

bool GYDM::Plane::is_matter_found(IMatter* m, MatterInfo* info, const Dot& dot) {
    Dot lp = Dot(dot.x - info->x, dot.y - info->y);

    /** NOTE:
     * the translation should only affect view poisition(say, mouse position for instance)
     */
    
    return m->is_colliding(lp);
}

/*************************************************************************************************/
void GYDM::Plane::bind_canvas(IMatter* m, GYDM::Tracklet* canvas, const Port& p, bool shared) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        info->canvas = canvas;
        info->shared_canvas = shared;
        info->draw_port = p;

        if (!info->shared_canvas) {
            unsafe_canvas_sync_settings(info);
        }
    }
}

void GYDM::Plane::reset_pen(IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        unsafe_do_canvas_setting(this, m, info, MotionAction(MotionActionType::TrackReset));
    }
}

void GYDM::Plane::stamp(IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        unsafe_do_canvas_setting(this, m, info, MotionAction(MotionActionType::Stamp));
    }
}

void GYDM::Plane::set_drawing(IMatter* m, bool yes_or_no) {
    MatterInfo* info = plane_matter_info(this, m);

    if ((info != nullptr) && (info->master != nullptr)) {
        unsafe_do_canvas_setting(this, m, info, MotionAction(yes_or_no));
    }
}

void GYDM::Plane::set_pen_width(IMatter* m, uint8_t width) {
    MatterInfo* info = plane_matter_info(this, m);

    if ((info != nullptr) && (info->master != nullptr)) {
        unsafe_do_canvas_setting(this, m, info, MotionAction(width));
    }
}

void GYDM::Plane::set_pen_color(IMatter* m, const RGBA& color) {
    MatterInfo* info = plane_matter_info(this, m);

    if ((info != nullptr) && (info->master != nullptr)) {
        unsafe_do_canvas_setting(this, m, info, MotionAction(color));
    }
}

void GYDM::Plane::set_heading(IMatter* m, double direction, bool is_radian) {
    MatterInfo* info = plane_matter_info(this, m);

    if ((info != nullptr) && (info->master != nullptr)) {
        unsafe_do_canvas_setting(this, m, info, MotionAction(MotionActionType::Heading, direction, is_radian));
    }
}

void GYDM::Plane::turn(IMatter* m, double theta, bool is_radian) {
    MatterInfo* info = plane_matter_info(this, m);

    if ((info != nullptr) && (info->master != nullptr)) {
        unsafe_do_canvas_setting(this, m, info, MotionAction(MotionActionType::Rotation, theta, is_radian));
    }
}

/*************************************************************************************************/
void GYDM::Plane::log_message(Log level, const std::string& msg) {
    if (this->sentry != nullptr) {
        RGBA color;

        switch (level) {
        case Log::Info: color = GREEN; break;
        case Log::Warning: color = YELLOW; break;
        case Log::Error: color = RED; break;
        case Log::Fatal: color = CRIMSON; break;
        case Log::Debug: color = SILVER; break;
        default: color = transparent;
        }

        this->sentry->say(2.0, msg, color);
    } else {
        IPlane::log_message(level, msg);
    }
}

void GYDM::Plane::say(ISprite* m, double sec, IMatter* message, SpeechBubble type) {
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

void GYDM::Plane::say(ISprite* m, double sec, const std::string& message, const RGBA& color, SpeechBubble type) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        if (message.empty()) {
            this->shh(m);
        } else if (this->merge_bubble_text(info->bubble, message, color)) {
            bubble_start(m, info, sec, type, this->bubble_second);
        } else {
            this->say(m, sec, this->make_bubble_text(message, color), type);
        }
    }
}

void GYDM::Plane::shh(ISprite* m) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        if (this->in_speech(m)) {
            bubble_expire(m, info);
        }
    }
}

IMatter* GYDM::Plane::make_bubble_text(const std::string& message, const RGBA& color) {
    return new Labellet(this->bubble_font, color, "%s", message.c_str());
}

bool GYDM::Plane::merge_bubble_text(IMatter* bubble, const std::string& message, const RGBA& color) {
    auto bmsg = dynamic_cast<ITextlet*>(bubble);
    bool okay = (bmsg != nullptr);

    if (okay) {
        bmsg->set_text_color(color);
        bmsg->set_text(message);
    }

    return okay;
}

bool GYDM::Plane::is_bubble_showing(IMatter* m, SpeechBubble* type) {
    MatterInfo* info = plane_matter_info(this, m);
    bool yes = is_matter_bubble_showing(m, info);

    if (yes) {
        SET_BOX(type, info->bubble_type);
    }

    return yes;
}

void GYDM::Plane::place_speech_bubble(IMatter* m, float bubble_width, float bubble_height, float Width, float Height, Port* mp, Port* bp, float* dx, float* dy) {
    mp->reset(0.0F);
    bp->reset(0.618F, 1.0);
    SET_BOXES(dx, dy, 0.0F);
}

void GYDM::Plane::set_bubble_duration(double second) {
    if (second <= 0.0) {
        this->set_bubble_duration();
    } else {
        this->bubble_second = second;
    }
}

void GYDM::Plane::set_bubble_color(const RGBA& border, const RGBA& background) {
    this->bubble_border = border;
    this->bubble_color = background;
}

void GYDM::Plane::delete_matter(IMatter* m) {
    // m's destructor will delete the associated info object
    delete m;
}

/*************************************************************************************************/
bool GYDM::Plane::is_colliding_with_mouse(IMatter* m) {
    Dot mdot = this->get_matter_location(m, MatterPort::LT);
    bool okay = false;
    
    if (mdot.okay()) {
        Dot mouse = get_current_mouse_location();

        okay = m->is_colliding(mouse - (mdot + this->translate));
    }
    
    return okay;
}

void GYDM::Plane::glide_to_random_location(double sec, IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);
    
    if (info != nullptr) {
        IScreen* screen = this->master();
        float width, height;
        Dot rpos;
    
        if (screen != nullptr) {
            Box box = m->get_bounding_box();
            float hinset = box.width() * 0.5F;
            float vinset = box.height() * 0.5F;

            screen->feed_client_extent(&width, &height);

            // TODO: deal with translation
            rpos.x = float(random_uniform(int(hinset), int(width - hinset)));
            rpos.y = float(random_uniform(int(vinset), int(height - vinset)));

            this->glide_to(sec, m, rpos, MatterPort::CC, Vector::O);
        }
    }
}

void GYDM::Plane::glide_to_mouse(double sec, IMatter* m, const Port& p, const Vector& vec) {
    MatterInfo* info = plane_matter_info(this, m);
    
    if (info != nullptr) {
        Dot mouse = get_current_mouse_location();

        this->glide_to(sec, m, mouse - this->translate, p, vec);
    }
}

/*************************************************************************************************/
GYDM::IPlane::IPlane(const char* name) : caption(name) {}
GYDM::IPlane::IPlane(const std::string& name) : IPlane(name.c_str()) {}

GYDM::IPlane::~IPlane() {
    if (this->info != nullptr) {
        delete this->info;
        this->info = nullptr;
    }
}

const char* GYDM::IPlane::name() const {
    return this->caption.c_str();
}

IScreen* GYDM::IPlane::master() const {
    IScreen* screen = nullptr;

    if (this->info != nullptr) {
        screen = this->info->master;
    }

    return screen;
}

dc_t* GYDM::IPlane::drawing_context() const {
    IScreen* screen = this->master();
    dc_t* device = nullptr;
    
    if (screen != nullptr) {
        device = screen->display()->drawing_context();
    }

    return device;
}

void GYDM::IPlane::on_enter(IPlane* from) {
    float width, height;

    this->master()->feed_client_extent(&width, &height);
    this->on_mission_start(width, height);
}

void GYDM::IPlane::start_input_text(const char* fmt, ...) {
    if (this->info != nullptr) {
        VSNPRINT(prompt, fmt);
        this->info->master->start_input_text(prompt);
    }
}

void GYDM::IPlane::start_input_text(const std::string& prompt) {
    if (this->info != nullptr) {
        this->info->master->start_input_text(prompt);
    }
}

void GYDM::IPlane::log_message(Log level, const std::string& msg) {
    if (this->info != nullptr) {
        this->info->master->log_message(level, msg);
    }
}

void GYDM::IPlane::begin_update_sequence() {
    if (this->info != nullptr) {
        this->info->master->begin_update_sequence();
    }
}

bool GYDM::IPlane::is_in_update_sequence() {
    return ((this->info != nullptr) && this->info->master->is_in_update_sequence());
}

void GYDM::IPlane::end_update_sequence() {
    if (this->info != nullptr) {
        this->info->master->end_update_sequence();
    }
}

bool GYDM::IPlane::should_update() {
    return ((this->info != nullptr) && this->info->master->should_update());
}

void GYDM::IPlane::notify_updated(IMatter* m) {
    if (this->info != nullptr) {
        this->info->master->notify_updated();
    }
}

bool GYDM::IPlane::is_colliding(IMatter* m, IMatter* target) {
    Box sbox = this->get_matter_bounding_box(m);
    Box tbox = this->get_matter_bounding_box(target);

    return sbox.overlay(tbox);
}

bool GYDM::IPlane::is_colliding(IMatter* m, IMatter* target, const Port& a) {
    Box sbox = this->get_matter_bounding_box(m);
    Dot tdot = this->get_matter_location(target, a);

    return sbox.contain(tdot);
}

/*************************************************************************************************/
void GYDM::IPlane::create_grid(int col, float x, float y, float width) {
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

void GYDM::IPlane::create_grid(int row, int col, float x, float y, float width, float height) {
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

void GYDM::IPlane::create_grid(int row, int col, IMatter* m) {
    MatterInfo* info = plane_matter_info(this, m);

    if (info != nullptr) {
        float x = info->x;
        float y = info->y;
        Box box = m->get_bounding_box();

        this->create_grid(row, col, x, y, box.width(), box.height());
    }
}

void GYDM::IPlane::create_grid(float cell_width, float x, float y, int col) {
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

void GYDM::IPlane::create_grid(float cell_width, float cell_height, float x, float y, int row, int col) {
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

int GYDM::IPlane::grid_cell_index(float x, float y, int* r, int* c) {
    int row = int(flfloor((y - this->grid_y) / this->cell_height));
    int col = int(flfloor((x - this->grid_x) / this->cell_width));
    
    SET_VALUES(r, row, c, col);

    return row * this->column + col;
}

int GYDM::IPlane::grid_cell_index(IMatter* m, int* r, int* c, const Port& a) {
    Dot dot = this->get_matter_location(m, a);
    
    return this->grid_cell_index(dot.x, dot.y, r, c);    
}

GYDM::Box GYDM::IPlane::get_grid_cell_bounding_box() {
    return { this->cell_width, this->cell_height };
}

Dot GYDM::IPlane::get_grid_cell_location(int idx, const Port& a) {
    if (idx < 0) {
        idx += this->column * this->row;
    }

    if (this->column > 0) {
        int c = idx % this->column;
        int r = idx / this->column;

        return this->get_grid_cell_location(r, c, a);
    } else {
        return this->get_grid_cell_location(idx, 0, a);
    }
}

Dot GYDM::IPlane::get_grid_cell_location(int row, int col, const Port& a) {
    Dot dot;
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
    
    dot.x = this->grid_x + this->cell_width * (float(col) + a.fx);
    dot.y = this->grid_y + this->cell_height * (float(row) + a.fy);
    
    return dot;
}

void GYDM::IPlane::insert_at_grid(IMatter* m, int idx, const Port& p, const Vector& vec) {
    this->insert_at(m, this->get_grid_cell_location(idx, p), p, vec);
}

void GYDM::IPlane::insert_at_grid(IMatter* m, int row, int col, const Port& p, const Vector& vec) {
    this->insert_at(m, this->get_grid_cell_location(row, col, p), p, vec);
}

void GYDM::IPlane::move_to_grid(IMatter* m, int idx, const Port& p, const Vector& vec) {
    this->move_to(m, this->get_grid_cell_location(idx, p), p, vec);
}

void GYDM::IPlane::move_to_grid(IMatter* m, int row, int col, const Port& p, const Vector& vec) {
    this->move_to(m, this->get_grid_cell_location(row, col, p), p, vec);
}

void GYDM::IPlane::glide_to_grid(double sec, IMatter* m, int idx, const Port& p, const Vector& vec) {
    this->glide_to(sec, m, this->get_grid_cell_location(idx, p), p, vec);
}

void GYDM::IPlane::glide_to_grid(double sec, IMatter* m, int row, int col, const Port& p, const Vector& vec) {
    this->glide_to(sec, m, this->get_grid_cell_location(row, col, p), p, vec);
}

/*************************************************************************************************/
bool GYDM::IPlane::in_speech(ISprite* m) {
    return this->is_bubble_showing(m, nullptr);
}

bool GYDM::IPlane::is_speaking(ISprite* m) {
    SpeechBubble type;
    bool showing = this->is_bubble_showing(m, &type);

    return showing && (type == SpeechBubble::Default);
}

bool GYDM::IPlane::is_thinking(ISprite* m) {
    SpeechBubble type;
    bool showing = this->is_bubble_showing(m, &type);

    return showing && (type == SpeechBubble::Thought);
}

void GYDM::IPlane::say(ISprite* m, const std::string& sentence, const RGBA& color) {
    if (sentence.empty()) {
        this->shh(m);
    } else {
        this->say(m, 0.0, sentence, color, SpeechBubble::Default);
    }
}

void GYDM::IPlane::say(ISprite* m, double sec, const std::string& sentence, const RGBA& color) {
    if (sentence.empty()) {
        this->shh(m);
    } else {
        this->say(m, sec, sentence, color, SpeechBubble::Default);
    }
}

void GYDM::IPlane::think(ISprite* m, const std::string& sentence, const RGBA& color) {
    if (sentence.empty()) {
        this->shh(m);
    } else {
        this->say(m, 0.0, sentence, color, SpeechBubble::Thought);
    }
}

void GYDM::IPlane::think(ISprite* m, double sec, const std::string& sentence, const RGBA& color) {
    if (sentence.empty()) {
        this->shh(m);
    } else {
        this->say(m, sec, sentence, color, SpeechBubble::Thought);
    }
}
