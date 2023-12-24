#pragma once

#include "graphics/font.hpp"
#include "physics/color/rgba.hpp"
#include "physics/color/names.hpp"

#include "virtualization/screen.hpp"

namespace WarGrey::STEM {
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
    friend class WarGrey::STEM::Cosmos;
    friend class WarGrey::STEM::Continent;
    public:
        virtual ~IPlane();
        IPlane(const std::string& name);
        IPlane(const char* name);

    public:
        const char* name();
        WarGrey::STEM::IScreen* master();
        SDL_Renderer* master_renderer();

    public:
        virtual bool has_mission_completed() { return false; }
 
    public:
        virtual void construct(float Width, float Height) {}
        virtual void load(float Width, float Height) {}
        virtual void reflow(float width, float height) {}
        virtual void update(uint64_t count, uint32_t interval, uint64_t uptime) {}
        virtual void draw(SDL_Renderer* renderer, float X, float Y, float Width, float Height) {}
    
    public:
        virtual WarGrey::STEM::IMatter* find_matter(float x, float y, WarGrey::STEM::IMatter* after) = 0;
        virtual bool feed_matter_location(IMatter* m, float* x, float* y, float fx, float fy) = 0;
        virtual bool feed_matter_boundary(IMatter* m, float* x, float* y, float* width, float* height) = 0;
        virtual void feed_matters_boundary(float* x, float* y, float* width, float* height) = 0;
        virtual void insert_at(IMatter* m, float x, float y, float fx, float fy, float dx, float dy) = 0;
        virtual void insert_as_speech_bubble(IMatter* m) = 0;
        virtual void bring_to_front(IMatter* m, IMatter* target) = 0;
        virtual void bring_forward(IMatter* m, int n) = 0;
        virtual void send_to_back(IMatter* m, IMatter* target) = 0;
        virtual void send_backward(IMatter* m, int n) = 0;
        virtual void move(IMatter* m, double length, bool ignore_gliding) = 0;
        virtual void move(IMatter* m, float x, float y, bool ignore_gliding) = 0;
        virtual void move_to(IMatter* m, float x, float y, float fx, float fy, float dx, float dy) = 0;
        virtual void move_to(IMatter* m, IMatter* tm, float tfx, float tfy, float fx, float fy, float dx, float dy) = 0;
        virtual void move_to(IMatter* m, IMatter* xtm, float xfx, IMatter* ytm, float yfy, float fx, float fy, float dx, float dy) = 0;
        virtual void glide(double sec, IMatter* m, double length) = 0;
        virtual void glide(double sec, IMatter* m, float x, float y) = 0;
        virtual void glide_to(double sec, IMatter* m, float x, float y, float fx, float fy, float dx, float dy) = 0;
        virtual void glide_to(double sec, IMatter* m, IMatter* tm, float tfx, float tfy, float fx, float fy, float dx, float dy) = 0;
        virtual void glide_to(double sec, IMatter* m, IMatter* xtm, float xfx, IMatter* ytm, float yfy, float fx, float fy, float dx, float dy) = 0;
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
        virtual WarGrey::STEM::IMatter* get_focused_matter() = 0;
        virtual void set_caret_owner(IMatter* m) = 0;
        virtual void notify_matter_ready(IMatter* m) = 0;
        virtual void notify_matter_timeline_restart(IMatter* m, uint32_t count0, int duration = 0) = 0;

    public:
        virtual void shh(ISprite* m) = 0;
        virtual void say(ISprite* m, double sec, IMatter* message, SpeechBubble type) = 0;
        virtual void say(ISprite* m, double sec, const std::string& message, const WarGrey::STEM::RGBA& color, SpeechBubble type) = 0;
        
    public:
        void begin_update_sequence();
        bool is_in_update_sequence();
        void end_update_sequence();
        bool should_update();
        void notify_updated(IMatter* m = nullptr);

    public:
        void set_background(const WarGrey::STEM::RGBA& color) { this->background = color; }
        WarGrey::STEM::RGBA get_background() { return this->background; }
        void start_input_text(const std::string& prompt);
        void start_input_text(const char* prompt, ...);
        void log_message(WarGrey::STEM::Log log, const std::string& msg);

    public:
        bool is_colliding(IMatter* m, IMatter* target);
        bool is_colliding(IMatter* m, IMatter* target, float fx, float fy);
        bool is_colliding(IMatter* m, IMatter* target, MatterAnchor a);
        bool is_colliding_with_mouse(IMatter* m);
        bool feed_matter_location(IMatter* m, float* x, float* y, MatterAnchor a);
        void insert_at(IMatter* m, float x, float y, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
        void move(IMatter* m, float length, bool ignore_gliding = false) { this->move(m, double(length), ignore_gliding); }
        void move_to(IMatter* m, float x, float y, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
        void move_to(IMatter* m, IMatter* tm, MatterAnchor ta, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
        void move_to(IMatter* m, IMatter* tm, MatterAnchor ta, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F);
        void move_to(IMatter* m, IMatter* tm, float tfx, float tfy, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
        void move_to(IMatter* m, IMatter* xtm, float xfx, IMatter* ytm, float yfy, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
        void glide(double sec, IMatter* m, float length) { this->glide(sec, m, double(length)); }
        void glide_to(double sec, IMatter* m, float x, float y, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
        void glide_to(double sec, IMatter* m, IMatter* tm, MatterAnchor ta, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
        void glide_to(double sec, IMatter* m, IMatter* tm, MatterAnchor ta, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F);
        void glide_to(double sec, IMatter* m, IMatter* tm, float tfx, float tfy, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
        void glide_to(double sec, IMatter* m, IMatter* xtm, float xfx, IMatter* ytm, float yfy, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
        void glide_to_random_location(double sec, IMatter* m);
        void glide_to_mouse(double sec, IMatter* m, MatterAnchor a = MatterAnchor::CC, float dx = 0.0F, float dy = 0.0F);
        
    public:
        void create_grid(int col, float x = 0.0F, float y = 0.0F, float width = 0.0F);
        void create_grid(int row, int col, float x = 0.0F, float y = 0.0F, float width = 0.0F, float height = 0.0F);
        void create_grid(int row, int col, IMatter* m);
        void create_grid(float cell_width, float x = 0.0F, float y = 0.0F, int col = 0);
        void create_grid(float cell_width, float cell_height, float x = 0.0F, float y = 0.0F, int row = 0, int col = 0);
        int grid_cell_index(float x, float y, int* r = nullptr, int* c = nullptr);
        int grid_cell_index(IMatter* m, int* r = nullptr, int* c = nullptr, MatterAnchor a = MatterAnchor::CC);
        void feed_grid_cell_extent(float* width, float* height);
        void feed_grid_cell_location(int idx, float* x, float* y, MatterAnchor a = MatterAnchor::CC);
        void feed_grid_cell_location(int row, int col, float* x, float* y, MatterAnchor a = MatterAnchor::CC);
        void insert_at_grid(IMatter* m, int idx, MatterAnchor a = MatterAnchor::CC, float dx = 0.0F, float dy = 0.0F);
        void insert_at_grid(IMatter* m, int row, int col, MatterAnchor a = MatterAnchor::CC, float dx = 0.0F, float dy = 0.0F);
        void move_to_grid(IMatter* m, int idx, MatterAnchor a = MatterAnchor::CC, float dx = 0.0F, float dy = 0.0F);
        void move_to_grid(IMatter* m, int row, int col, MatterAnchor a = MatterAnchor::CC, float dx = 0.0F, float dy = 0.0F);
        void glide_to_grid(double sec, IMatter* m, int idx, MatterAnchor a = MatterAnchor::CC, float dx = 0.0F, float dy = 0.0F);
        void glide_to_grid(double sec, IMatter* m, int row, int col, MatterAnchor a = MatterAnchor::CC, float dx = 0.0F, float dy = 0.0F);
        void set_grid_color(const WarGrey::STEM::RGBA& color) { this->grid_color = color; }

    public:
        void say(ISprite* m, IMatter* message) { this->say(m, 0.0, message, SpeechBubble::Default); }
        void say(ISprite* m, double sec, IMatter* message) { this->say(m, sec, message, SpeechBubble::Default); }
        void think(ISprite* m, IMatter* message) { this->say(m, 0.0, message, SpeechBubble::Thought); }
        void think(ISprite* m, double sec, IMatter* message) { this->say(m, sec, message, SpeechBubble::Thought); }
    
        void say(ISprite* m, const std::string& sentence, const WarGrey::STEM::RGBA& color = BLACK);
        void say(ISprite* m, double sec, const std::string& sentence, const WarGrey::STEM::RGBA& color = BLACK);
        void think(ISprite* m, const std::string& sentence, const WarGrey::STEM::RGBA& color = DIMGRAY);
        void think(ISprite* m, double sec, const std::string& sentence, const WarGrey::STEM::RGBA& color = DIMGRAY);
        
        bool in_speech(ISprite* m);
        bool is_speaking(ISprite* m);
        bool is_thinking(ISprite* m);
         
    protected:
        virtual bool on_pointer_pressed(uint8_t button, float x, float y, uint8_t clicks) { return false; }
        virtual bool on_pointer_released(uint8_t button, float x, float y, uint8_t clicks) { return false; }
        virtual bool on_pointer_move(uint32_t state, float x, float y, float dx, float dy) { return false; }
        virtual bool on_scroll(int horizon, int vertical, float hprecise, float vprecise) { return false; }

    protected:
        virtual void on_focus(WarGrey::STEM::IMatter* m, bool on_off) {}
        virtual void on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) {}
        virtual void on_text(const char* text, size_t size, bool entire) {}
        virtual void on_editing_text(const char* text, int pos, int span) {}
        virtual void on_elapse(uint64_t count, uint32_t interval, uint64_t uptime) {}
        virtual void on_hover(WarGrey::STEM::IMatter* m, float local_x, float local_y) {}
        virtual void on_goodbye(WarGrey::STEM::IMatter* m, float local_x, float local_y) {}
        virtual void on_tap(WarGrey::STEM::IMatter* m, float local_x, float local_y) {}
        virtual void on_tap_selected(WarGrey::STEM::IMatter* m, float local_x, float local_y) {}

    protected:
        virtual void on_motion_start(WarGrey::STEM::IMatter* m, double sec, float x, float y, double xspd, double yspd) {}
        virtual void on_motion_step(WarGrey::STEM::IMatter* m, float x, float y, double xspd, double yspd, double percentage) {}
        virtual void on_motion_complete(WarGrey::STEM::IMatter* m, float x, float y, double xspd, double yspd) {}
        
    protected:
        virtual void on_enter(WarGrey::STEM::IPlane* from);
        virtual void on_leave(WarGrey::STEM::IPlane* to) { /* the completion of mission doesn't imply leaving */ }
        virtual void mission_complete() { this->on_mission_complete(); }
        virtual void on_mission_start(float width, float height) {}
        virtual void on_mission_complete() {}

    protected:
        virtual void on_matter_ready(IMatter* m) = 0;

    protected:
        virtual const char* usrdata_extension() { return nullptr; }
        virtual void on_save(const std::string& usrdata_path, std::ofstream& dev_datout) {}

    protected:
        virtual void draw_visible_selection(SDL_Renderer* renderer, float X, float Y, float width, float height) = 0;

    protected:
        virtual IMatter* make_bubble_text(const std::string& message, const WarGrey::STEM::RGBA& color) = 0;
        virtual bool merge_bubble_text(IMatter* bubble, const std::string& message, const WarGrey::STEM::RGBA& color) = 0;
        virtual bool is_bubble_showing(IMatter* m, SpeechBubble* type) = 0;

    public:
        template<class M>
        M* insert_for_speech(M* m) {
            this->insert_as_speech_bubble(m);

            return m;
        }
        
        template<class M>
        M* insert(M* m, float x = 0.0F, float y = 0.0F, MatterAnchor a = MatterAnchor::LT, float dx = 0.0F, float dy = 0.0F) {
            this->insert_at(m, x, y, a, dx, dy);

            return m;
        }
        
        template<class M>
        M* insert(M* m, int idx, MatterAnchor a = MatterAnchor::CC, float dx = 0.0F, float dy = 0.0F) {
            this->insert_at_grid(m, idx, a, dx, dy);

            return m;
        }

        template<class M>
        M* insert(M* m, int row, int col, MatterAnchor a = MatterAnchor::CC, float dx = 0.0F, float dy = 0.0F) {
            this->insert_at_grid(m, row, col, a, dx, dy);

            return m;
        }

    public:
        IPlaneInfo* info = nullptr;

    protected:
        WarGrey::STEM::RGBA background;

    protected:
        int column = 0;
        int row = 0;
        float grid_x = 0.0F;
        float grid_y = 0.0F;
        float cell_width = 0.0F;
        float cell_height = 0.0F;
        WarGrey::STEM::RGBA grid_color;
        
    private:
        std::string caption;
    };

    class __lambda__ Plane : public WarGrey::STEM::IPlane {
    public:
        virtual ~Plane();
        Plane(const std::string& caption);
        Plane(const char* caption);

    public:
        bool has_mission_completed() override;
        void set_sentry_sprite(WarGrey::STEM::ISprite* sentry) { this->sentry = sentry; }
        void set_tooltip_matter(WarGrey::STEM::IMatter* m, float dx = 0.0F, float dy = 0.0F);
        void set_bubble_color(const WarGrey::STEM::RGBA& border, const WarGrey::STEM::RGBA& background);
        void set_bubble_font(shared_font_t font) { this->bubble_font = font; }
        void set_bubble_duration(double second = 3600.0);
        void set_bubble_margin(float margin) { this->set_bubble_margin(margin, margin); }
        void set_bubble_margin(float hmargin, float vmargin) { this->set_bubble_margin(vmargin, hmargin, vmargin, hmargin); }
        void set_bubble_margin(float top, float right, float bottom, float left);
        
    public:
        void draw(SDL_Renderer* renderer, float X, float Y, float Width, float Height) override;

    public: // learn C++ "Name Hiding"
        using WarGrey::STEM::IPlane::feed_matter_location;
        using WarGrey::STEM::IPlane::insert_at;
        using WarGrey::STEM::IPlane::move;
        using WarGrey::STEM::IPlane::move_to;
        using WarGrey::STEM::IPlane::glide;
        using WarGrey::STEM::IPlane::glide_to;

        WarGrey::STEM::IMatter* find_matter(float x, float y, WarGrey::STEM::IMatter* after = nullptr) override;
        bool feed_matter_location(IMatter* m, float* x, float* y, float fx = 0.0F, float fy = 0.0F) override;
        bool feed_matter_boundary(IMatter* m, float* x, float* y, float* width, float* height) override;
        void feed_matters_boundary(float* x, float* y, float* width, float* height) override;
        void insert_at(IMatter* m, float x, float y, float fx, float fy, float dx, float dy) override;
        void insert_as_speech_bubble(IMatter* m) override;
        void bring_to_front(IMatter* m, IMatter* target = nullptr) override;
        void bring_forward(IMatter* m, int n = 1) override;
        void send_to_back(IMatter* m, IMatter* target = nullptr) override;
        void send_backward(IMatter* m, int n = 1) override;
        void move(IMatter* m, double length, bool ignore_gliding = false) override;
        void move(IMatter* m, float x, float y, bool ignore_gliding = false) override;
        void move_to(IMatter* m, float x, float y, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) override;
        void move_to(IMatter* m, IMatter* tm, float tfx, float tfy, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) override;
        void move_to(IMatter* m, IMatter* xtm, float xfx, IMatter* ytm, float yfy, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) override;
        void glide(double sec, IMatter* m, double length) override;
        void glide(double sec, IMatter* m, float x, float y) override;
        void glide_to(double sec, IMatter* m, float x, float y, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) override;
        void glide_to(double sec, IMatter* m, IMatter* tm, float tfx, float tfy, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) override;
        void glide_to(double sec, IMatter* m, IMatter* xtm, float xfx, IMatter* ytm, float yfy, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) override;
        void remove(IMatter* m, bool needs_delete = true) override;
        void erase() override;
        void size_cache_invalid();
        void clear_motion_actions(IMatter* m);

    public:
        void bind_canvas(IMatter* m, WarGrey::STEM::Tracklet* canvas, MatterAnchor anchor, bool shared = false);
        void bind_canvas(IMatter* m, WarGrey::STEM::Tracklet* canvas, float fx = 0.5F, float fy = 0.5F, bool shared = false);
        void reset_pen(IMatter* m);
        void stamp(IMatter* m);
        void pen_down(IMatter* m) { this->set_drawing(m, true); }
        void pen_up(IMatter* m) { this->set_drawing(m, false); }
        void set_drawing(IMatter* m, bool yes_or_no);
        void set_pen_width(IMatter* m, uint8_t width);
        void set_pen_color(IMatter* m, const WarGrey::STEM::RGBA& color);
        void set_heading(IMatter* m, double direction, bool is_radian = false);
        void turn(IMatter* m, double theta, bool is_radian = false);

    public:
        void log_message(WarGrey::STEM::Log level, const std::string& msg);

        void shh(ISprite* m) override;
        void say(ISprite* m, double sec, IMatter* message, WarGrey::STEM::SpeechBubble type) override;
        void say(ISprite* m, double sec, const std::string& message, const WarGrey::STEM::RGBA& color, WarGrey::STEM::SpeechBubble type) override;

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
        WarGrey::STEM::IMatter* get_focused_matter() override;
        void set_caret_owner(IMatter* m) override;
        void notify_matter_ready(IMatter* m) override;
        void notify_matter_timeline_restart(IMatter* m, uint32_t count0 = 1, int duration = 0) override;

    public:
        void set_matter_fps(IMatter* m, int fps, bool restart = false);
        void set_local_fps(int fps, bool restart = false);

    protected:
        void draw_visible_selection(SDL_Renderer* renderer, float x, float y, float width, float height) override;
        virtual bool update_tooltip(IMatter* m, float local_x, float local_y, float global_x, float global_y) { return false; }
        virtual void on_double_tap_sentry_sprite(WarGrey::STEM::ISprite* sentry) { this->mission_complete(); }

    protected:
        IMatter* make_bubble_text(const std::string& message, const WarGrey::STEM::RGBA& color) override;
        bool merge_bubble_text(IMatter* bubble, const std::string& message, const WarGrey::STEM::RGBA& color) override;
        bool is_bubble_showing(IMatter* m, SpeechBubble* type) override;
        virtual void place_speech_bubble(IMatter* m, float bubble_width, float bubble_height, float Width, float Height,
                                            float* mfx, float* mfy, float* bfx, float* bfy, float* dx, float* dy);
        
    protected:
        bool on_pointer_pressed(uint8_t button, float x, float y, uint8_t clicks) override;
        bool on_pointer_move(uint32_t state, float x, float y, float dx, float dy) override;
        bool on_pointer_released(uint8_t button, float x, float y, uint8_t clicks) override;
        bool on_scroll(int horizon, int vertical, float hprecise, float vprecise) override;
        
        void on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) override;
        void on_text(const char* text, size_t size, bool entire) override;
        void on_editing_text(const char* text, int pos, int span) override;
        void on_tap(WarGrey::STEM::IMatter* m, float x, float y) override;
        void on_tap_selected(WarGrey::STEM::IMatter* m, float x, float y) override;

    protected:
        void on_enter(WarGrey::STEM::IPlane* from) override;
        void mission_complete() override;

    protected:
        void on_elapse(uint64_t count, uint32_t interval, uint64_t uptime) override;
        void on_matter_ready(IMatter* m) override {}

    private:
        void handle_queued_motion(IMatter* m, MatterInfo* info, float dwidth, float dheight);
        bool move_matter_via_info(IMatter* m, MatterInfo* info, double length, bool ignore_gliding, bool heading);
        bool move_matter_via_info(IMatter* m, MatterInfo* info, float x, float y, bool absolute, bool ignore_gliding, bool heading);
        bool move_matter_to_location_via_info(IMatter* m, MatterInfo* info, float x, float y, float fx, float fy, float dx, float dy);
        bool glide_matter_via_info(IMatter* m, MatterInfo* info, double sec, double length);
        bool glide_matter_via_info(IMatter* m, MatterInfo* info, double sec, float x, float y, bool absolute, bool heading);
        bool glide_matter_to_location_via_info(IMatter* m, MatterInfo* info, double sec, float x, float y, float fx, float fy, float dx, float dy, bool heading);
        bool do_moving_via_info(IMatter* m, MatterInfo* info, float x, float y, bool absolute, bool ignore_track, bool heading);
        bool do_gliding_via_info(IMatter* m, MatterInfo* info, float x, float y, double sec, double sec_delta, bool absolute, bool ignore_track);
        bool do_vector_moving(IMatter* m, MatterInfo* info, double length, bool heading);
        bool do_vector_gliding(IMatter* m, MatterInfo* info, double length, double sec);
        
    private:
        void handle_new_matter(IMatter* m, SpeechInfo* info);
        void handle_new_matter(IMatter* m, MatterInfo* info, float x, float y, float fx, float fy, float dx, float dy);
        void draw_matter(SDL_Renderer* renderer, IMatter* self, MatterInfo* info, float X, float Y, float dsX, float dsY, float dsWidth, float dsHeight);
        void draw_speech(SDL_Renderer* renderer, IMatter* self, MatterInfo* info, float Width, float Height, float X, float Y, float dsX, float dsY, float dsWidth, float dsHeight);
        void recalculate_matters_extent_when_invalid();
        bool say_goodbye_to_hover_matter(uint32_t state, float x, float y, float dx, float dy);
        bool is_matter_found(IMatter* m, MatterInfo* info, float x, float y);
        WarGrey::STEM::IMatter* find_matter_for_tooltip(float x, float y);
        WarGrey::STEM::IMatter* find_least_recent_matter(float x, float y);
        void place_tooltip(IMatter* target);
        void no_selected_except(IMatter* m);
        void delete_matter(IMatter* m);

    private:
        float matters_left;
        float matters_top;
        float matters_right;
        float matters_bottom;

    private:
        WarGrey::STEM::IMatter* head_matter = nullptr;
        WarGrey::STEM::IMatter* head_speech = nullptr;
        WarGrey::STEM::IMatter* focused_matter = nullptr;
        WarGrey::STEM::IMatter* hovering_matter = nullptr;
        uint32_t local_frame_delta = 0U;
        uint32_t local_frame_count = 1U;
        uint32_t local_elapse = 0U;
        float hovering_mgx = 0.0F;
        float hovering_mgy = 0.0F;
        float hovering_mlx = 0.0F;
        float hovering_mly = 0.0F;

    private:
        // TODO: implement other transformation
        float translate_x = 0.0F;
        float translate_y = 0.0F;
    
    private:
        WarGrey::STEM::ISprite* sentry = nullptr;
        bool mission_done = false;

    private:
        WarGrey::STEM::IMatter* tooltip = nullptr;
        float tooltip_dx = 0.0F;
        float tooltip_dy = 0.0F;

    private:
        WarGrey::STEM::RGBA bubble_border = GAINSBORO;
        WarGrey::STEM::RGBA bubble_color = GHOSTWHITE;
        double bubble_second = 0.0;
        float bubble_top_margin = 0.0F;
        float bubble_right_margin = 0.0F;
        float bubble_bottom_margin = 0.0F;
        float bubble_left_margin = 0.0F;
        shared_font_t bubble_font;
    };
}
