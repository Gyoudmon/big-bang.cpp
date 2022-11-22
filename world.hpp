#pragma once

#include "cosmos.hpp"
#include "plane.hpp"

namespace WarGrey::STEM {
    class World : public WarGrey::STEM::Cosmos {
        public:
            World(const char* title, int fps = 60, uint32_t fgc = 0x000000U, uint32_t bgc = 0xFFFFFFU);

        public:
            void construct(int argc, char* argv[]) override {}
            void reflow(float Width, float Height) override {}

        public:
            virtual void load(float Width, float Height) {}

        public:
            virtual bool can_interactive_move(IMatter* g, float local_x, float local_y) { return false; }
            virtual bool can_select(IMatter* g) { return false; }
            virtual bool can_select_multiple() { return false; }
            virtual void before_select(IMatter* g, bool on_or_off) {}
            virtual void after_select(IMatter* g, bool on_or_off) {}

        public:
            template<class G>
            G* insert(G* g, float x = 0.0F, float y = 0.0F, MatterAnchor a = MatterAnchor::LT) {
                return this->plane->insert(g, x, y, a);
            }

        public:
            bool fill_matter_location(IMatter* g, float* x, float* y, MatterAnchor a) {
                return this->plane->fill_matter_location(g, x, y, a);
            }

            void insert_at(IMatter* g, float x, float y, MatterAnchor a, float dx = 0.0F, float dy = 0.0F) {
                this->plane->insert_at(g, x, y, a, dx, dy);
            }

            void insert_at(IMatter* g, IMatter* tg, MatterAnchor ta, MatterAnchor a, float dx = 0.0F, float dy = 0.0F) {
                this->plane->insert_at(g, tg, ta, a, dx, dy);
            }

            void insert_at(IMatter* g, IMatter* tg, MatterAnchor ta, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) {
                this->plane->insert_at(g, tg, ta, fx, fy, dx, dy);
            }

            void insert_at(IMatter* g, IMatter* tg, float tfx, float tfy, MatterAnchor a, float dx = 0.0F, float dy = 0.0F) {
                this->plane->insert_at(g, tg, tfx, tfy, a, dx, dy);
            }

            void insert_at(IMatter* g, IMatter* xtg, float xfx, IMatter* ytg, float yfy, MatterAnchor a, float dx = 0.0F, float dy = 0.0F) {
                this->plane->insert_at(g, xtg, xfx, ytg, yfy, a, dx, dy);
            }

            void move(IMatter* g, float x, float y) {
                this->plane->move(g, x, y);
            }

            void move_to(IMatter* g, float x, float y, MatterAnchor a, float dx = 0.0F, float dy = 0.0F) {
                this->plane->move_to(g, x, y, a, dx, dy);
            }

            void move_to(IMatter* g, IMatter* tg, MatterAnchor ta, MatterAnchor a, float dx = 0.0F, float dy = 0.0F) {
                this->plane->move_to(g, tg, ta, a, dx, dy);
            }

            void move_to(IMatter* g, IMatter* tg, MatterAnchor ta, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) {
                this->plane->move_to(g, tg, ta, fx, fy, dx, dy);
            }

            void move_to(IMatter* g, IMatter* tg, float tfx, float tfy, MatterAnchor a, float dx = 0.0F, float dy = 0.0F) {
                this->plane->move_to(g, tg, tfx, tfy, a, dx, dy);
            }

            void move_to(IMatter* g, IMatter* xtg, float xfx, IMatter* ytg, float yfy, MatterAnchor a, float dx = 0.0F, float dy = 0.0F) {
                this->plane->move_to(g, xtg, xfx, ytg, yfy, a, dx, dy);
            }

            void remove(IMatter* g) {
                this->plane->remove(g);
            }

            void erase() {
                this->plane->erase();
            }

        protected:
            WarGrey::STEM::Plane* plane;
    };
}

