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

        private:
            World* master;
    };
}

/*************************************************************************************************/
WarGrey::STEM::World::World(const char* title, int fps, uint32_t fgc, uint32_t bgc) : Cosmos(fps, fgc, bgc) {
    this->self = new WorldPlanet(this, title);
    this->push_planet(this->self);
}

