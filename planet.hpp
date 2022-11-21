#pragma once

#include "universe.hpp"
#include "forward.hpp"

#include "virtualization/screen.hpp"

namespace WarGrey::STEM {
    class IPlanetInfo {
        public:
            virtual ~IPlanetInfo() {}
            IPlanetInfo(IScreen* master) : master(master) {}
        
        public:
            IScreen* master;
    };

    /** Note
     * The destruction of `IPlanet` is always performed by its `display`
     *  since its instance cannot belong to multiple `display`s.
     *
     *  Do not `delete` it on your own.
     */
    class IPlanet {
        public:
            virtual ~IPlanet();
            IPlanet(const std::string& name);
            IPlanet(const char* name);

        public:
            const char* name();
            WarGrey::STEM::IScreen* master();

        public:
            virtual void construct(float Width, float Height) {}
            virtual void load(float Width, float Height) {}
            virtual void reflow(float width, float height) {}
            virtual void update(uint32_t count, uint32_t interval, uint32_t uptime) {}
            virtual void draw(SDL_Renderer* renderer, float X, float Y, float Width, float Height) {}
            virtual bool can_exit() { return false; }
        
        public:
            virtual WarGrey::STEM::IMatter* find_graphlet(float x, float y) = 0;
            virtual bool fill_graphlet_location(IMatter* g, float* x, float* y, float fx = 0.0F, float fy = 0.0F) = 0;
            virtual bool fill_graphlet_boundary(IMatter* g, float* x, float* y, float* width, float* height) = 0;
            virtual void fill_graphlets_boundary(float* x, float* y, float* width, float* height) = 0;
            virtual void insert(IMatter* g, float x = 0.0F, float y = 0.0F, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) = 0;
            virtual void insert(IMatter* g, IMatter* tg, float tfx, float tfy, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) = 0;
            virtual void insert(IMatter* g, IMatter* xtg, float xfx, IMatter* ytg, float yfy, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) = 0;
            virtual void move(IMatter* g, float x, float y) = 0;
            virtual void move_to(IMatter* g, float x, float y, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) = 0;
            virtual void move_to(IMatter* g, IMatter* tg, float tfx, float tfy, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) = 0;
            virtual void move_to(IMatter* g, IMatter* xtg, float xfx, IMatter* ytg, float yfy, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) = 0;
            virtual void remove(IMatter* g) = 0;
            virtual void erase() = 0;

        public:
            void set_background(uint32_t color, float a = 1.0F) { this->background = color; this->bg_alpha = a; }
            void fill_background(SDL_Color* color);
            void start_input_text(const std::string& prompt);
            void start_input_text(const char* prompt, ...);
            void log_message(int fgc, const char* fmt, ...);
            void log_message(int fgc, const std::string& msg);
            void log_message(const char* fmt, ...);

        public:
            virtual bool on_pointer_pressed(uint8_t button, float x, float y, uint8_t clicks, bool touch) { return false; }
            virtual bool on_pointer_released(uint8_t button, float x, float y, uint8_t clicks, bool touch) { return false; }
            virtual bool on_pointer_move(uint32_t state, float x, float y, float dx, float dy, bool touch) { return false; }
            virtual bool on_scroll(int horizon, int vertical, float hprecise, float vprecise) { return false; }

        public:
            virtual void on_focus(WarGrey::STEM::IMatter* g, bool on_off) {}
            virtual void on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) {}
            virtual void on_text(const char* text, size_t size, bool entire) {}
            virtual void on_editing_text(const char* text, int pos, int span) {}
            virtual void on_elapse(uint32_t count, u_int32_t interval, uint32_t uptime) {}
            virtual void on_hover(WarGrey::STEM::IMatter* g, float local_x, float local_y) {}
            virtual void on_goodbye(WarGrey::STEM::IMatter* g, float local_x, float local_y) {}
            virtual void on_tap(WarGrey::STEM::IMatter* g, float local_x, float local_y) {}
            virtual void on_tap_selected(WarGrey::STEM::IMatter* g, float local_x, float local_y) {}

            virtual void on_save() {}

        public:
            virtual void draw_visible_selection(SDL_Renderer* renderer, float X, float Y, float width, float height) = 0;
            virtual IMatter* find_next_selected_graphlet(IMatter* start = nullptr) = 0;
            virtual IMatter* thumbnail_graphlet() = 0;
            virtual void add_selected(IMatter* g) = 0;
            virtual void set_selected(IMatter* g) = 0;
            virtual void no_selected() = 0;
            virtual unsigned int count_selected() = 0;
            virtual bool is_selected(IMatter* g) = 0;

        public:
            virtual bool can_interactive_move(IMatter* g, float local_x, float local_y) { return false; }
            virtual bool can_select(IMatter* g) { return false; }
            virtual bool can_select_multiple() { return false; }
            virtual void before_select(IMatter* g, bool on_or_off) {}
            virtual void after_select(IMatter* g, bool on_or_off) {}
        
        public:
            virtual WarGrey::STEM::IMatter* get_focus_graphlet() = 0;
            virtual void set_caret_owner(IMatter* g) = 0;
            virtual void notify_graphlet_ready(IMatter* g) = 0;
            virtual void on_graphlet_ready(IMatter* g) = 0;

        public:
            void begin_update_sequence();
            bool in_update_sequence();
            void end_update_sequence();
            bool needs_update();
            void notify_updated();

        public:
            SDL_Surface* snapshot(float width, float height, uint32_t bgcolor = 0U, float alpha = 0.0F);
            SDL_Surface* snapshot(float x, float y, float width, float height, uint32_t bgcolor = 0U, float alpha = 0.0F);
        
            bool save_snapshot(const std::string& pname, float width, float height, uint32_t bgcolor = 0U, float alpha = 0.0F);
            bool save_snapshot(const char* pname, float width, float height, uint32_t bgcolor = 0U, float alpha = 0.0F );
            bool save_snapshot(const std::string& pname, float x, float y, float width, float height, uint32_t bgcolor = 0U, float alpha = 0.0F);
            bool save_snapshot(const char* pname, float x, float y, float width, float height, uint32_t bgcolor = 0U, float alpha = 0.0F);

        public:
            bool fill_graphlet_location(IMatter* g, float* x, float* y, MatterAnchor a);
            void insert(IMatter* g, float x, float y, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
            void insert(IMatter* g, IMatter* tg, MatterAnchor ta, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
            void insert(IMatter* g, IMatter* tg, MatterAnchor ta, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F);
            void insert(IMatter* g, IMatter* tg, float tfx, float tfy, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
            void insert(IMatter* g, IMatter* xtg, float xfx, IMatter* ytg, float yfy, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
            void move_to(IMatter* g, float x, float y, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
            void move_to(IMatter* g, IMatter* tg, MatterAnchor ta, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
            void move_to(IMatter* g, IMatter* tg, MatterAnchor ta, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F);
            void move_to(IMatter* g, IMatter* tg, float tfx, float tfy, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
            void move_to(IMatter* g, IMatter* xtg, float xfx, IMatter* ytg, float yfy, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);

        public:
            template<class G>
            G* insert_one(G* g, float x = 0.0F, float y = 0.0F, MatterAnchor a = MatterAnchor::LT) {
                this->insert(g, x, y, a);

                return g;
            }

        public:
            IPlanetInfo* info = nullptr;

        protected:
            uint32_t background = 0U;
            float bg_alpha = 0.0F;

        private:
            std::string caption;
    };

    class Planet : public WarGrey::STEM::IPlanet {
        public:
            virtual ~Planet();
            Planet(const std::string& caption, unsigned int initial_mode = 0);
            Planet(const char* caption, unsigned int initial_mode = 0);

        public:
            void change_mode(unsigned int mode); // NOTE: mode 0 is designed for UI graphlets which will be unmasked in all modes;
            unsigned int current_mode();

            bool graphlet_unmasked(WarGrey::STEM::IMatter* g);

        public:
            void draw(SDL_Renderer* renderer, float X, float Y, float Width, float Height) override;

        public: // learn C++ "Name Hiding"
            using WarGrey::STEM::IPlanet::fill_graphlet_location;
            using WarGrey::STEM::IPlanet::insert;
            using WarGrey::STEM::IPlanet::move_to;

            WarGrey::STEM::IMatter* find_graphlet(float x, float y) override;
            bool fill_graphlet_location(IMatter* g, float* x, float* y, float fx = 0.0F, float fy = 0.0F) override;
            bool fill_graphlet_boundary(IMatter* g, float* x, float* y, float* width, float* height) override;
            void fill_graphlets_boundary(float* x, float* y, float* width, float* height) override;
            void insert(IMatter* g, float x = 0.0F, float y = 0.0F, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) override;
            void insert(IMatter* g, IMatter* tg, float tfx, float tfy, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) override;
            void insert(IMatter* g, IMatter* xtg, float xfx, IMatter* ytg, float yfy, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) override;
            void move(IMatter* g, float x, float y) override;
            void move_to(IMatter* g, float x, float y, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) override;
            void move_to(IMatter* g, IMatter* tg, float tfx, float tfy, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) override;
            void move_to(IMatter* g, IMatter* xtg, float xfx, IMatter* ytg, float yfy, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) override;
            void remove(IMatter* g) override;
            void erase() override;
            void size_cache_invalid();

        public:
            bool on_pointer_pressed(uint8_t button, float x, float y, uint8_t clicks, bool touch) override;
            bool on_pointer_move(uint32_t state, float x, float y, float dx, float dy, bool touch) override;
            bool on_pointer_released(uint8_t button, float x, float y, uint8_t clicks, bool touch) override;
            bool on_scroll(int horizon, int vertical, float hprecise, float vprecise) override;
            
            void on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) override;
            void on_text(const char* text, size_t size, bool entire) override;
            void on_editing_text(const char* text, int pos, int span) override;
            void on_tap(WarGrey::STEM::IMatter* g, float x, float y) override;
            void on_elapse(uint32_t count, u_int32_t interval, uint32_t uptime) override;

        public:
            void draw_visible_selection(SDL_Renderer* renderer, float x, float y, float width, float height) override;
            IMatter* find_next_selected_graphlet(IMatter* start = nullptr) override;
            IMatter* thumbnail_graphlet() override { return nullptr; }
            void add_selected(IMatter* g) override;
            void set_selected(IMatter* g) override;
            void no_selected() override;
            unsigned int count_selected() override;
            bool is_selected(IMatter* g) override;

        public:
            WarGrey::STEM::IMatter* get_focus_graphlet() override;
            void set_caret_owner(IMatter* g) override;
            void notify_graphlet_ready(IMatter* g) override;
            void on_graphlet_ready(IMatter* g) override {}

        private:
            void recalculate_graphlets_extent_when_invalid();
            bool say_goodbye_to_hover_graphlet(uint32_t state, float x, float y, float dx, float dy);
            void collapse();

        private:
            float graphlets_left;
            float graphlets_top;
            float graphlets_right;
            float graphlets_bottom;

        private:
            WarGrey::STEM::IMatter* head_graphlet = nullptr;
            WarGrey::STEM::IMatter* focused_graphlet = nullptr;
            WarGrey::STEM::IMatter* hovering_graphlet = nullptr;
            unsigned int mode = 0U;

        private:
            float translate_x = 0.0F;
            float translate_y = 0.0F;
            float scale_x = 1.0F;
            float scale_y = 1.0F;
    };
}

