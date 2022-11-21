#pragma once

#include "cosmos.hpp"
#include "planet.hpp"

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

        protected:
            WarGrey::STEM::Planet* self;
    };
}

