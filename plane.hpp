#pragma once

#include <type_traits>

#include "graphics/dc.hpp"
#include "graphics/font.hpp"
#include "physics/color/rgba.hpp"
#include "physics/color/names.hpp"
#include "physics/algebra/vector.hpp"
#include "physics/geometry/port.hpp"
#include "physics/geometry/aabox.hpp"
#include "physics/geometry/margin.hpp"

#include "virtualization/screen.hpp"
#include "virtualization/position.hpp"

namespace Plteen {
    class __lambda__ IPlaneInfo {
    public:
        virtual ~IPlaneInfo() {}
        IPlaneInfo(IScreen* master) : master(master) {}
    
    public:
        IScreen* master;
    };

    struct MatterInfo;
    class SpeechInfo;

    /** Note
     * The destruction of `IPlane` is always performed by its `display`
     *  since its instance cannot belong to multiple `display`s.
     *
     *  Do not `delete` it on your own.
     */
    class __lambda__ IPlane {
    friend class Plteen::Cosmos;
    friend class Plteen::Planelet;
    public:
        virtual ~IPlane();
        IPlane(const std::string& name);
        IPlane(const char* name);

    public:
        const char* name() const;
        Plteen::IScreen* master() const;
        Plteen::dc_t* drawing_context() const;

    public:
        virtual bool has_mission_completed() { return false; }
 
    public:
        virtual void construct(float Width, float Height) {}
        virtual void load(float Width, float Height) {}
        virtual void reflow(float width, float height) {}
        virtual void update(uint64_t count, uint32_t interval, uint64_t uptime) {}
        virtual void draw(Plteen::dc_t* dc, float X, float Y, float Width, float Height) {}
        virtual void draw_foreground(Plteen::dc_t* dc, float X, float Y, float Width, float Height) {}
        virtual void draw_background(Plteen::dc_t* dc, float X, float Y, float Width, float Height) {}
    
    public:
        virtual Plteen::IMatter* find_matter(const Position& pos, Plteen::IMatter* after) = 0;
        virtual Plteen::IMatter* find_matter(Plteen::IMatter* collided_matter, Plteen::IMatter* after) = 0;
        virtual Plteen::Dot get_matter_location(Plteen::IMatter* m, const Plteen::Port& a) = 0;
        virtual Plteen::Box get_matter_bounding_box(Plteen::IMatter* m) = 0;
        virtual Plteen::Box get_bounding_box() = 0;
        virtual void insert_at(IMatter* m, const Plteen::Position& pos, const Plteen::Port& p, const Plteen::Vector& vec) = 0;
        virtual void insert_as_speech_bubble(IMatter* m) = 0;
        virtual void bring_to_front(IMatter* m, IMatter* target) = 0;
        virtual void bring_forward(IMatter* m, int n) = 0;
        virtual void send_to_back(IMatter* m, IMatter* target) = 0;
        virtual void send_backward(IMatter* m, int n) = 0;
        virtual void move(IMatter* m, double length, bool ignore_gliding) = 0;
        virtual void move(IMatter* m, const Plteen::Vector& vec, bool ignore_gliding) = 0;
        virtual void move_to(IMatter* m, const Plteen::Position& pos, const Plteen::Port& p, const Plteen::Vector& vec) = 0;
        virtual void glide(double sec, IMatter* m, double length) = 0;
        virtual void glide(double sec, IMatter* m, const Plteen::Vector& vec) = 0;
        virtual void glide_to(double sec, IMatter* m, const Plteen::Position& pos, const Plteen::Port& p, const Plteen::Vector& vec) = 0;
        virtual void remove(IMatter* m, bool needs_delete) = 0;
        virtual void erase() = 0;

    public:
        virtual IMatter* find_next_selected_matter(IMatter* start) = 0;
        virtual void add_selected(IMatter* m) = 0;
        virtual void remove_selected(IMatter* m) = 0;
        virtual void set_selected(IMatter* m) = 0;
        virtual void no_selected() = 0;
        virtual size_t count_selected() = 0;
        virtual bool is_selected(IMatter* m) = 0;

    public:
        virtual bool can_interactive_move(IMatter* m, float local_x, float local_y) { return false; }
        virtual bool can_select(IMatter* m) { return false; }
        virtual bool can_select_multiple() { return false; }
        virtual void on_select(IMatter* m, bool on_or_off) {} // the plane should be locked for writing when invoking this method
        virtual void after_select(IMatter* m, bool on_or_off) {} // use this method if you want to modify the plane
        
    public:
        virtual Plteen::IMatter* get_focused_matter() = 0;
        virtual void set_caret_owner(IMatter* m) = 0;
        virtual void notify_matter_ready(IMatter* m) = 0;
        virtual void notify_matter_timeline_restart(IMatter* m, uint32_t count0, int duration = 0) = 0;

    public:
        virtual void shh(ISprite* m) = 0;
        virtual void say(ISprite* m, double sec, IMatter* message, SpeechBubble type) = 0;
        virtual void say(ISprite* m, double sec, const std::string& message, const Plteen::RGBA& color, SpeechBubble type) = 0;
        
    public:
        void begin_update_sequence();
        bool is_in_update_sequence();
        void end_update_sequence();
        bool should_update();
        void notify_updated(IMatter* m = nullptr);

    public:
        void set_background(const Plteen::RGBA& color) { this->background = color; }
        Plteen::RGBA get_background() { return this->background; }
        void start_input_text(const std::string& prompt);
        void start_input_text(const char* prompt, ...);
        void log_message(Plteen::Log log, const std::string& msg);

    public:
        bool is_colliding(IMatter* m, IMatter* target);
        bool is_colliding(IMatter* m, IMatter* target, const Plteen::Port& a);
        
    public:
        void create_grid(int col, float x = 0.0F, float y = 0.0F, float width = 0.0F);
        void create_grid(int row, int col, float x = 0.0F, float y = 0.0F, float width = 0.0F, float height = 0.0F);
        void create_grid(int row, int col, IMatter* m);
        void create_grid(float cell_width, float x = 0.0F, float y = 0.0F, int col = 0);
        void create_grid(float cell_width, float cell_height, float x = 0.0F, float y = 0.0F, int row = 0, int col = 0);
        void create_centered_grid(int row, int col, float cell_width, float cell_height, const Plteen::Vector& vec = Plteen::Vector::O);
        void create_centered_grid(int row, int col, float cell_size, const Plteen::Vector& vec = Plteen::Vector::O) { this->create_centered_grid(row, col, cell_size, cell_size, vec); }
        void create_centered_grid(int n, float cell_size, const Plteen::Vector& vec = Plteen::Vector::O) { this->create_centered_grid(n, n, cell_size, cell_size, vec); }
        int grid_cell_index(float x, float y, int* r = nullptr, int* c = nullptr);
        int grid_cell_index(IMatter* m, int* r = nullptr, int* c = nullptr, const Plteen::Port& p = 0.5F);
        Plteen::Box get_grid_cell_bounding_box();
        Plteen::Dot get_grid_cell_location(int idx, const Plteen::Port& p = 0.5F);
        Plteen::Dot get_grid_cell_location(int row, int col, const Plteen::Port& p = 0.5F);
        void insert_at_grid(IMatter* m, int idx, const Plteen::Port& p = 0.5F, const Plteen::Vector& vec = Plteen::Vector::O);
        void insert_at_grid(IMatter* m, int row, int col, const Plteen::Port& p = 0.5F, const Plteen::Vector& vec = Plteen::Vector::O);
        void move_to_grid(IMatter* m, int idx, const Plteen::Port& p = 0.5F, const Plteen::Vector& vec = Plteen::Vector::O);
        void move_to_grid(IMatter* m, int row, int col, const Plteen::Port& p = 0.5F, const Plteen::Vector& vec = Plteen::Vector::O);
        void glide_to_grid(double sec, IMatter* m, int idx, const Plteen::Port& p = 0.5F, const Plteen::Vector& vec = Plteen::Vector::O);
        void glide_to_grid(double sec, IMatter* m, int row, int col, const Plteen::Port& p = 0.5F, const Plteen::Vector& vec = Plteen::Vector::O);
        void set_grid_color(const Plteen::RGBA& color) { this->grid_color = color; }

    public:
        void say(ISprite* m, IMatter* message) { this->say(m, 0.0, message, SpeechBubble::Default); }
        void say(ISprite* m, double sec, IMatter* message) { this->say(m, sec, message, SpeechBubble::Default); }
        void think(ISprite* m, IMatter* message) { this->say(m, 0.0, message, SpeechBubble::Thought); }
        void think(ISprite* m, double sec, IMatter* message) { this->say(m, sec, message, SpeechBubble::Thought); }
    
        void say(ISprite* m, const std::string& sentence, const Plteen::RGBA& color = BLACK);
        void say(ISprite* m, double sec, const std::string& sentence, const Plteen::RGBA& color = BLACK);
        void think(ISprite* m, const std::string& sentence, const Plteen::RGBA& color = DIMGRAY);
        void think(ISprite* m, double sec, const std::string& sentence, const Plteen::RGBA& color = DIMGRAY);
        
        bool in_speech(ISprite* m);
        bool is_speaking(ISprite* m);
        bool is_thinking(ISprite* m);
         
    protected:
        virtual bool on_pointer_pressed(uint8_t button, float x, float y, uint8_t clicks) { return false; }
        virtual bool on_pointer_released(uint8_t button, float x, float y, uint8_t clicks) { return false; }
        virtual bool on_pointer_move(uint32_t state, float x, float y, float dx, float dy) { return false; }
        virtual bool on_scroll(int horizon, int vertical, float hprecise, float vprecise) { return false; }

    protected:
        virtual void on_focus(Plteen::IMatter* m, bool on_off) {}
        virtual void on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) {}
        virtual void on_text(const char* text, size_t size, bool entire) {}
        virtual void on_editing_text(const char* text, int pos, int span) {}
        virtual void on_elapse(uint64_t count, uint32_t interval, uint64_t uptime) {}
        virtual void on_hover(Plteen::IMatter* m, float local_x, float local_y) {}
        virtual void on_goodbye(Plteen::IMatter* m, float local_x, float local_y) {}
        virtual void on_tap(Plteen::IMatter* m, float local_x, float local_y) {}
        virtual void on_tap_selected(Plteen::IMatter* m, float local_x, float local_y) {}

    protected:
        virtual void on_motion_start(Plteen::IMatter* m, double sec, float x, float y, double xspd, double yspd) {}
        virtual void on_motion_step(Plteen::IMatter* m, float x, float y, double xspd, double yspd, double percentage) {}
        virtual void on_motion_complete(Plteen::IMatter* m, float x, float y, double xspd, double yspd) {}
        
    protected:
        virtual void on_enter(Plteen::IPlane* from);
        virtual void on_leave(Plteen::IPlane* to) { /* the completion of mission doesn't imply leaving */ }
        virtual void mission_complete() { this->on_mission_complete(); }
        virtual void on_mission_start(float width, float height) {}
        virtual void on_mission_complete() {}

    protected:
        virtual void on_matter_ready(IMatter* m) = 0;

    protected:
        virtual const char* usrdata_extension() { return nullptr; }
        virtual void on_save(const std::string& usrdata_path, std::ofstream& dev_datout) {}

    protected:
        virtual void draw_visible_selection(Plteen::dc_t* renderer, float X, float Y, float width, float height) = 0;

    protected:
        virtual IMatter* make_bubble_text(const std::string& message, const Plteen::RGBA& color) = 0;
        virtual bool merge_bubble_text(IMatter* bubble, const std::string& message, const Plteen::RGBA& color) = 0;
        virtual bool is_bubble_showing(IMatter* m, SpeechBubble* type) = 0;
        virtual void on_bubble_expired(IMatter* m, SpeechBubble type) {}

    public:
        template<class M, typename... Args>
        M* create_for_speech(Args&&... args) {
            M* m = new M(std::forward<Args>(args)...);

            this->insert_as_speech_bubble(m);

            return m;
        }
        
        template<class M>
        M* insert(M* m, const Plteen::Position& pos = {}, const Plteen::Port& p = 0.0F, const Plteen::Vector& vec = Plteen::Vector::O) {
            this->insert_at(m, pos, p, vec);
            
            return m;
        }
        
        template<class M>
        M* insert(M* m, int idx, const Plteen::Port& p = 0.5F, const Plteen::Vector& vec = Plteen::Vector::O) {
            this->insert_at_grid(m, idx, p, vec);

            return m;
        }

        template<class M>
        M* insert(M* m, int row, int col, const Plteen::Port& p = 0.5F, const Plteen::Vector& vec = Plteen::Vector::O) {
            this->insert_at_grid(m, row, col, p, vec);

            return m;
        }

        template<class M, typename... Args>
        M* spawn(Args && ... args) {
            M* self = nullptr;

            static_assert(std::is_base_of_v<Plteen::IMatter, M> || std::is_base_of_v<Plteen::IPlane, M>);

            if constexpr(std::is_base_of_v<Plteen::IMatter, M>) {
                self = new M(std::forward<Args>(args)...);
                this->insert(self);
            } else {
                self = new M(std::forward<Args>(args)...);
                this->insert_planelet(self);
            }

            return self;
        }

    public:
        IPlaneInfo* info = nullptr;

    protected:
        Plteen::RGBA background;

    protected:
        int column = 0;
        int row = 0;
        float grid_x = 0.0F;
        float grid_y = 0.0F;
        float cell_width = 0.0F;
        float cell_height = 0.0F;
        Plteen::RGBA grid_color;

    private:
        void insert_planelet(Plteen::IPlane* child);
        
    private:
        std::string caption;
    };

    class __lambda__ Plane : public Plteen::IPlane {
    public:
        virtual ~Plane();
        Plane(const std::string& caption);
        Plane(const char* caption);

    public:
        bool has_mission_completed() override;
        void set_sentry_sprite(Plteen::ISprite* sentry) { this->sentry = sentry; }
        void set_tooltip_matter(Plteen::IMatter* m, const Plteen::Vector& vec = Plteen::Vector::O);
        void set_bubble_color(const Plteen::RGBA& border, const Plteen::RGBA& background);
        void set_bubble_font(shared_font_t font) { this->bubble_font = font; }
        void set_bubble_duration(double second = 3600.0);
        void set_bubble_margin(const Plteen::Margin& margin) { this->bubble_margin = margin; }
        
    public:
        void draw(Plteen::dc_t* renderer, float X, float Y, float Width, float Height) override;
        
    public:
        bool is_colliding_with_mouse(IMatter* m);
        void glide_to_random_location(double sec, IMatter* m);
        void glide_to_mouse(double sec, IMatter* m, const Plteen::Port& p = 0.5F, const Plteen::Vector& vec = Plteen::Vector::O);

    public:
        Plteen::IMatter* find_matter(const Position& pos, Plteen::IMatter* after = nullptr) override;
        Plteen::IMatter* find_matter(Plteen::IMatter* collided_matter, Plteen::IMatter* after = nullptr) override;
        Plteen::Dot get_matter_location(Plteen::IMatter* m, const Plteen::Port& p = 0.0F) override;
        Plteen::Box get_matter_bounding_box(Plteen::IMatter* m) override;
        Plteen::Box get_bounding_box() override;
        void insert_at(IMatter* m, const Plteen::Position& pos, const Plteen::Port& p, const Plteen::Vector& vec) override;
        void insert_as_speech_bubble(IMatter* m) override;
        void bring_to_front(IMatter* m, IMatter* target = nullptr) override;
        void bring_forward(IMatter* m, int n = 1) override;
        void send_to_back(IMatter* m, IMatter* target = nullptr) override;
        void send_backward(IMatter* m, int n = 1) override;
        void move(IMatter* m, double length, bool ignore_gliding = false) override;
        void move(IMatter* m, const Plteen::Vector& vec, bool ignore_gliding = false) override;
        void move_to(IMatter* m, const Plteen::Position& pos, const Plteen::Port& p = 0.0F, const Plteen::Vector& vec = Plteen::Vector::O) override;
        void glide(double sec, IMatter* m, double length) override;
        void glide(double sec, IMatter* m, const Plteen::Vector& vec) override;
        void glide_to(double sec, IMatter* m, const Plteen::Position& pos, const Plteen::Port& p = 0.0F, const Plteen::Vector& vec = Plteen::Vector::O) override;
        void remove(IMatter* m, bool needs_delete = true) override;
        void erase() override;
        void size_cache_invalid();
        void clear_motion_actions(IMatter* m = nullptr, bool stop_current_motion = true);

    public:
        void bind_canvas(IMatter* m, Plteen::Tracklet* canvas, const Plteen::Port& anchor = 0.5F, bool shared = false);
        void reset_pen(IMatter* m);
        void stamp(IMatter* m);
        void pen_down(IMatter* m) { this->set_drawing(m, true); }
        void pen_up(IMatter* m) { this->set_drawing(m, false); }
        void set_drawing(IMatter* m, bool yes_or_no);
        void set_pen_width(IMatter* m, uint8_t width);
        void set_pen_color(IMatter* m, const Plteen::RGBA& color);
        void set_heading(IMatter* m, double direction, bool is_radian = false);
        void turn(IMatter* m, double theta, bool is_radian = false);

    public:
        void log_message(Plteen::Log level, const std::string& msg);

        void shh(ISprite* m) override;
        void say(ISprite* m, double sec, IMatter* message, Plteen::SpeechBubble type) override;
        void say(ISprite* m, double sec, const std::string& message, const Plteen::RGBA& color, Plteen::SpeechBubble type) override;

    public:
        IMatter* find_next_selected_matter(IMatter* start = nullptr) override;
        void add_selected(IMatter* m) override;
        void remove_selected(IMatter* m) override;
        void set_selected(IMatter* m) override;
        void no_selected() override;
        size_t count_selected() override;
        bool is_selected(IMatter* m) override;
        bool can_select(IMatter* m) override;

    public:
        Plteen::IMatter* get_focused_matter() override;
        void set_caret_owner(IMatter* m) override;
        void notify_matter_ready(IMatter* m) override;
        void notify_matter_timeline_restart(IMatter* m, uint32_t count0 = 1, int duration = 0) override;

    public:
        void set_matter_fps(IMatter* m, int fps, bool restart = false);
        void set_local_fps(int fps, bool restart = false);

    protected:
        void draw_visible_selection(Plteen::dc_t* renderer, float x, float y, float width, float height) override;
        virtual bool update_tooltip(IMatter* m, float local_x, float local_y, float global_x, float global_y) { return false; }
        virtual void on_double_tap_sentry_sprite(Plteen::ISprite* sentry) { this->mission_complete(); }

    protected:
        IMatter* make_bubble_text(const std::string& message, const Plteen::RGBA& color) override;
        bool merge_bubble_text(IMatter* bubble, const std::string& message, const Plteen::RGBA& color) override;
        bool is_bubble_showing(IMatter* m, SpeechBubble* type) override;
        virtual void place_speech_bubble(IMatter* m, float bubble_width, float bubble_height, float Width, float Height,
                                            Plteen::Port* mp, Plteen::Port* bp, float* dx, float* dy);
        
    protected:
        bool on_pointer_pressed(uint8_t button, float x, float y, uint8_t clicks) override;
        bool on_pointer_move(uint32_t state, float x, float y, float dx, float dy) override;
        bool on_pointer_released(uint8_t button, float x, float y, uint8_t clicks) override;
        bool on_scroll(int horizon, int vertical, float hprecise, float vprecise) override;
        
        void on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) override;
        void on_text(const char* text, size_t size, bool entire) override;
        void on_editing_text(const char* text, int pos, int span) override;
        void on_tap(Plteen::IMatter* m, float x, float y) override;
        void on_tap_selected(Plteen::IMatter* m, float x, float y) override;

    protected:
        void on_enter(Plteen::IPlane* from) override;
        void on_leave(Plteen::IPlane* to) override;
        void mission_complete() override;

    protected:
        void on_elapse(uint64_t count, uint32_t interval, uint64_t uptime) override;
        void on_matter_ready(IMatter* m) override {}

    private:
        void handle_queued_motion(IMatter* m, MatterInfo* info, float dwidth, float dheight);
        bool move_matter_via_info(IMatter* m, MatterInfo* info, double length, bool ignore_gliding, bool heading);
        bool move_matter_via_info(IMatter* m, MatterInfo* info, const Position& pos, bool absolute, bool ignore_gliding, bool heading);
        bool move_matter_to_location_via_info(IMatter* m, MatterInfo* info, const Position& pos, const Port& p, float dx, float dy);
        bool glide_matter_via_info(IMatter* m, MatterInfo* info, double sec, double length);
        bool glide_matter_via_info(IMatter* m, MatterInfo* info, double sec, const Position& pos, bool absolute, bool heading);
        bool glide_matter_to_location_via_info(IMatter* m, MatterInfo* info, double sec, const Position& pos, const Port& p, float dx, float dy, bool heading);
        bool do_moving_via_info(IMatter* m, MatterInfo* info, const Position& pos, bool absolute, bool ignore_track, bool heading);
        bool do_gliding_via_info(IMatter* m, MatterInfo* info, const Position& pos, double sec, double sec_delta, bool absolute, bool ignore_track);
        bool do_vector_moving(IMatter* m, MatterInfo* info, double length, bool heading);
        bool do_vector_gliding(IMatter* m, MatterInfo* info, double length, double sec);
        
    private:
        void handle_new_matter(IMatter* m, SpeechInfo* info);
        void handle_new_matter(IMatter* m, MatterInfo* info, const Position& pos, const Port& p, float dx, float dy);
        void draw_matter(Plteen::dc_t* renderer, IMatter* self, MatterInfo* info, float X, float Y, float dsX, float dsY, float dsWidth, float dsHeight);
        void draw_speech(Plteen::dc_t* renderer, IMatter* self, MatterInfo* info, float Width, float Height, float X, float Y, float dsX, float dsY, float dsWidth, float dsHeight);
        void recalculate_matters_extent_when_invalid();
        bool say_goodbye_to_hover_matter(uint32_t state, float x, float y, float dx, float dy);
        bool is_matter_found(IMatter* m, MatterInfo* info, const Dot& dot);
        Plteen::IMatter* find_matter_for_tooltip(const Plteen::Dot& pos);
        Plteen::IMatter* find_least_recent_matter(const Plteen::Dot& pos);
        void place_tooltip(IMatter* target);
        void no_selected_except(IMatter* m);
        void delete_matter(IMatter* m);

    private:
        Plteen::Box extent;

    private:
        Plteen::IMatter* head_matter = nullptr;
        Plteen::IMatter* head_speech = nullptr;
        Plteen::IMatter* focused_matter = nullptr;
        Plteen::IMatter* hovering_matter = nullptr;
        uint32_t local_frame_delta = 0U;
        uint32_t local_frame_count = 1U;
        uint32_t local_elapse = 0U;
        Plteen::Dot hovering_gm;
        Plteen::Dot hovering_lm;

    private:
        // TODO: implement other transformation
        Plteen::Dot translate = {};
    
    private:
        Plteen::ISprite* sentry = nullptr;
        bool mission_done = false;

    private:
        Plteen::IMatter* tooltip = nullptr;
        Plteen::Vector tooltip_offset = Plteen::Vector::O;

    private:
        Plteen::RGBA bubble_border = GAINSBORO;
        Plteen::RGBA bubble_color = GHOSTWHITE;
        Plteen::Margin bubble_margin = { 4.0F, 8.0F };
        double bubble_second = 0.0;
        shared_font_t bubble_font;
    };
}
