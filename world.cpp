#include "world.hpp"

using namespace WarGrey::STEM;

namespace {
    class WorldPlanet : public WarGrey::STEM::Planet {
        public:
            WorldPlanet(World* master, const char* title, int mode = 0)
                : Planet(title, mode), master(master) {}

        public:
            void load(float Width, float Height) override {
                master->load(Width, Height);
            }

        public:
            bool can_interactive_move(IMatter* g, float local_x, float local_y) override {
                return master->can_interactive_move(g, local_x, local_y);
            }

            bool can_select(IMatter* g) override {
                return master->can_select(g);
            }

            bool can_select_multiple() override {
                return master->can_select_multiple();
            }

            void before_select(IMatter* g, bool on_or_off) override {
                return master->before_select(g, on_or_off);
            }

            void after_select(IMatter* g, bool on_or_off) override {
                return master->after_select(g, on_or_off);
            }

        private:
            World* master;
    };
}

/*************************************************************************************************/
WarGrey::STEM::World::World(const char* title, int fps, uint32_t fgc, uint32_t bgc) : Cosmos(fps, fgc, bgc) {
    this->self = new WorldPlanet(this, title);
    this->push_planet(this->self);
}
