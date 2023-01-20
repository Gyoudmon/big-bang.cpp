#pragma once

#include "cosmos.hpp"
#include "plane.hpp"

namespace WarGrey::STEM {
    class World : public WarGrey::STEM::Cosmos {
    public:
        World(const char* title,
                int fps = 60, uint32_t fgc = 0x000000U, uint32_t bgc = 0xFFFFFFU,
                unsigned int initial_mode = 0U);

    public:
        void shift_to_mode(unsigned int mode) { this->plane->shift_to_mode(mode); }
        unsigned int current_mode() { return this->plane->current_mode(); }
        bool matter_unmasked(WarGrey::STEM::IMatter* m) { return this->plane->matter_unmasked(m); }

    public:
        void construct(int argc, char* argv[]) override {}
        void reflow(float Width, float Height) override {}

    public:
        virtual void load(float Width, float Height) {}

    public:
        virtual bool can_interactive_move(IMatter* m, float local_x, float local_y) { return false; }
        virtual bool can_select(IMatter* m) { return false; }
        virtual bool can_select_multiple() { return false; }
        virtual void before_select(IMatter* m, bool on_or_off) {}
        virtual void after_select(IMatter* m, bool on_or_off) {}

    public:
        bool feed_matter_location(IMatter* m, float* x, float* y, MatterAnchor a) {
            return this->plane->feed_matter_location(m, x, y, a);
        }

        void move(IMatter* m, float x, float y) {
            this->plane->move(m, x, y);
        }

        void move_to(IMatter* m, float x, float y, MatterAnchor a, float dx = 0.0F, float dy = 0.0F) {
            this->plane->move_to(m, x, y, a, dx, dy);
        }

        void move_to(IMatter* m, IMatter* tm, MatterAnchor ta, MatterAnchor a, float dx = 0.0F, float dy = 0.0F) {
            this->plane->move_to(m, tm, ta, a, dx, dy);
        }

        void move_to(IMatter* m, IMatter* tm, MatterAnchor ta, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) {
            this->plane->move_to(m, tm, ta, fx, fy, dx, dy);
        }

        void move_to(IMatter* m, IMatter* tm, float tfx, float tfy, MatterAnchor a, float dx = 0.0F, float dy = 0.0F) {
            this->plane->move_to(m, tm, tfx, tfy, a, dx, dy);
        }

        void move_to(IMatter* m, IMatter* xtm, float xfx, IMatter* ytm, float yfy, MatterAnchor a, float dx = 0.0F, float dy = 0.0F) {
            this->plane->move_to(m, xtm, xfx, ytm, yfy, a, dx, dy);
        }

        void remove(IMatter* m) {
            this->plane->remove(m);
        }

        void erase() {
            this->plane->erase();
        }
    
    public:
        template<class M>
        M* insert(M* m, float x = 0.0F, float y = 0.0F, MatterAnchor a = MatterAnchor::LT, float dx = 0.0F, float dy = 0.0F) {
            return this->plane->insert(m, x, y, a, dx, dy);
        }

    protected:
        WarGrey::STEM::Plane* plane;
    };
}
