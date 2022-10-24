#ifndef _WARGREY_STEM_PLANET_H
#define _WARGREY_STEM_PLANET_H

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
            virtual void update(long long count, long long interval, long long uptime) {}
            virtual void draw(SDL_Renderer* renderer, float X, float Y, float Width, float Height) {}
            virtual void collapse();
        
        public:
            virtual WarGrey::STEM::IGraphlet* find_graphlet(float x, float y) = 0;
            virtual bool fill_graphlet_location(IGraphlet* g, float* x, float* y, float fx = 0.0F, float fy = 0.0F) = 0;
            virtual bool fill_graphlet_boundary(IGraphlet* g, float* x, float* y, float* width, float* height) = 0;
            virtual void fill_graphlets_boundary(float* x, float* y, float* width, float* height) = 0;
            virtual void insert(IGraphlet* g, float x = 0.0F, float y = 0.0F, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) = 0;
            virtual void insert(IGraphlet* g, IGraphlet* tg, float tfx, float tfy, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) = 0;
            virtual void insert(IGraphlet* g, IGraphlet* xtg, float xfx, IGraphlet* ytg, float yfy, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) = 0;
            virtual void move(IGraphlet* g, float x, float y) = 0;
            virtual void move_to(IGraphlet* g, float x, float y, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) = 0;
            virtual void move_to(IGraphlet* g, IGraphlet* tg, float tfx, float tfy, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) = 0;
            virtual void move_to(IGraphlet* g, IGraphlet* xtg, float xfx, IGraphlet* ytg, float yfy, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) = 0;
            virtual void remove(IGraphlet* g) = 0;
            virtual void erase() = 0;

        public:
            virtual void set_background(int color) { this->background = color; }
            void send_message(int fgc, const char* fmt, ...);
            void send_message(int fgc, const std::string& msg);
            void send_message(const char* fmt, ...);

        public:
            virtual void on_focus(WarGrey::STEM::IGraphlet* g, bool on_off) {}
            virtual void on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) {}
            virtual void on_text(const char* text, bool entire) {}
            virtual void on_text(const char* text, int pos, int span) {}
            virtual void on_elapse(long long count, long long interval, long long uptime) {}
            virtual void on_hover(WarGrey::STEM::IGraphlet* g, float local_x, float local_y) {}
            virtual void on_goodbye(WarGrey::STEM::IGraphlet* g, float local_x, float local_y) {}
            virtual void on_tap(WarGrey::STEM::IGraphlet* g, float local_x, float local_y) {}
            virtual void on_tap_selected(WarGrey::STEM::IGraphlet* g, float local_x, float local_y) {}

        public:
            virtual void draw_visible_selection(SDL_Renderer* renderer, float X, float Y, float width, float height) = 0;
            virtual IGraphlet* find_next_selected_graphlet(IGraphlet* start = nullptr) = 0;
            virtual IGraphlet* thumbnail_graphlet() = 0;
            virtual void add_selected(IGraphlet* g) = 0;
            virtual void set_selected(IGraphlet* g) = 0;
            virtual void no_selected() = 0;
            virtual unsigned int count_selected() = 0;
            virtual bool is_selected(IGraphlet* g) = 0;

        public:
            virtual bool can_interactive_move(IGraphlet* g, float local_x, float local_y) { return false; }
            virtual bool can_select(IGraphlet* g) { return true; }
            virtual bool can_select_multiple() { return false; }
            virtual void before_select(IGraphlet* g, bool on_or_off) {}
            virtual void after_select(IGraphlet* g, bool on_or_off) {}
        
        public:
            virtual WarGrey::STEM::IGraphlet* get_focus_graphlet() = 0;
            virtual void set_caret_owner(IGraphlet* g) = 0;
            virtual void notify_graphlet_ready(IGraphlet* g) = 0;
            virtual void on_graphlet_ready(IGraphlet* g) = 0;

        public:
            void begin_update_sequence();
            bool in_update_sequence();
            void end_update_sequence();
            bool needs_update();
            void notify_updated();

        public:
            SDL_Surface* snapshot(float width, float height, int bgcolor = -1);
            SDL_Surface* snapshot(float x, float y, float width, float height, int bgcolor = -1);
        
            bool save_snapshot(const std::string& pname, float width, float height, int bgcolor = -1);
            bool save_snapshot(const char* pname, float width, float height, int bgcolor = -1);
            bool save_snapshot(const std::string& pname, float x, float y, float width, float height, int bgcolor = -1);
            bool save_snapshot(const char* pname, float x, float y, float width, float height, int bgcolor = -1);

        public:
            bool fill_graphlet_location(IGraphlet* g, float* x, float* y, GraphletAnchor a);
            void insert(IGraphlet* g, float x, float y, GraphletAnchor a, float dx = 0.0F, float dy = 0.0F);
            void insert(IGraphlet* g, IGraphlet* tg, GraphletAnchor ta, GraphletAnchor a, float dx = 0.0F, float dy = 0.0F);
            void insert(IGraphlet* g, IGraphlet* tg, GraphletAnchor ta, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F);
            void insert(IGraphlet* g, IGraphlet* tg, float tfx, float tfy, GraphletAnchor a, float dx = 0.0F, float dy = 0.0F);
            void insert(IGraphlet* g, IGraphlet* xtg, float xfx, IGraphlet* ytg, float yfy, GraphletAnchor a, float dx = 0.0F, float dy = 0.0F);
            void move_to(IGraphlet* g, float x, float y, GraphletAnchor a, float dx = 0.0F, float dy = 0.0F);
            void move_to(IGraphlet* g, IGraphlet* tg, GraphletAnchor ta, GraphletAnchor a, float dx = 0.0F, float dy = 0.0F);
            void move_to(IGraphlet* g, IGraphlet* tg, GraphletAnchor ta, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F);
            void move_to(IGraphlet* g, IGraphlet* tg, float tfx, float tfy, GraphletAnchor a, float dx = 0.0F, float dy = 0.0F);
            void move_to(IGraphlet* g, IGraphlet* xtg, float xfx, IGraphlet* ytg, float yfy, GraphletAnchor a, float dx = 0.0F, float dy = 0.0F);

        public:
            template<class G>
            G* insert_one(G* g, float x = 0.0F, float y = 0.0F, GraphletAnchor a = GraphletAnchor::LT) {
                this->insert(g, x, y, a);

                return g;
            }

        public:
            IPlanetInfo* info = nullptr;

        protected:
            int background = -1;

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

            bool graphlet_unmasked(WarGrey::STEM::IGraphlet* g);

        public:
            void draw(SDL_Renderer* renderer, float X, float Y, float Width, float Height) override;

        public: // learn C++ "Name Hiding"
            using WarGrey::STEM::IPlanet::fill_graphlet_location;
            using WarGrey::STEM::IPlanet::insert;
            using WarGrey::STEM::IPlanet::move_to;

            WarGrey::STEM::IGraphlet* find_graphlet(float x, float y) override;
            bool fill_graphlet_location(IGraphlet* g, float* x, float* y, float fx = 0.0F, float fy = 0.0F) override;
            bool fill_graphlet_boundary(IGraphlet* g, float* x, float* y, float* width, float* height) override;
            void fill_graphlets_boundary(float* x, float* y, float* width, float* height) override;
            void insert(IGraphlet* g, float x = 0.0F, float y = 0.0F, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) override;
            void insert(IGraphlet* g, IGraphlet* tg, float tfx, float tfy, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) override;
            void insert(IGraphlet* g, IGraphlet* xtg, float xfx, IGraphlet* ytg, float yfy, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) override;
            void move(IGraphlet* g, float x, float y) override;
            void move_to(IGraphlet* g, float x, float y, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) override;
            void move_to(IGraphlet* g, IGraphlet* tg, float tfx, float tfy, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) override;
            void move_to(IGraphlet* g, IGraphlet* xtg, float xfx, IGraphlet* ytg, float yfy, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) override;
            void remove(IGraphlet* g) override;
            void erase() override;
            void size_cache_invalid();

        public:
            using WarGrey::STEM::IPlanet::on_elapse;

            void on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) override;
            void on_text(const char* text, bool entire) override;
            void on_text(const char* text, int pos, int span) override;
            void on_tap(WarGrey::STEM::IGraphlet* g, float x, float y) override;
            void on_elapse(long long count, long long interval, long long uptime) override;

        public:
            void draw_visible_selection(SDL_Renderer* renderer, float x, float y, float width, float height) override;
            IGraphlet* find_next_selected_graphlet(IGraphlet* start = nullptr) override;
            IGraphlet* thumbnail_graphlet() override { return nullptr; }
            void add_selected(IGraphlet* g) override;
            void set_selected(IGraphlet* g) override;
            void no_selected() override;
            unsigned int count_selected() override;
            bool is_selected(IGraphlet* g) override;

        public:
            WarGrey::STEM::IGraphlet* get_focus_graphlet() override;
            void set_caret_owner(IGraphlet* g) override;
            void notify_graphlet_ready(IGraphlet* g) override;
            void on_graphlet_ready(IGraphlet* g) override {}

        private:
            void recalculate_graphlets_extent_when_invalid();

        private:
            float graphlets_left;
            float graphlets_top;
            float graphlets_right;
            float graphlets_bottom;

        private:
            WarGrey::STEM::IGraphlet* head_graphlet = nullptr;
            WarGrey::STEM::IGraphlet* focused_graphlet = nullptr;
            WarGrey::STEM::IGraphlet* hovering_graphlet = nullptr;
            unsigned int mode = 0U;

	private:
	    float translate_x = 0.0F;
	    float translate_y = 0.0F;
	    float scale_x = 1.0F;
	    float scale_y = 1.0F;
    };
}

#endif

