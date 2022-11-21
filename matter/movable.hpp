#pragma once

#include "../forward.hpp"

namespace WarGrey::STEM {
    class IMovable {
        public: // gameplay events
            virtual void on_border(float hoffset, float voffset);

        public:
            void set_speed(float spd, float dir);
            float x_speed() { return this->xspeed; }
            float y_speed() { return this->yspeed; }

        public:
            void set_border_strategy(WarGrey::STEM::BorderStrategy s);
            void set_border_strategy(WarGrey::STEM::BorderStrategy vs, WarGrey::STEM::BorderStrategy hs);
            void set_border_strategy(BorderStrategy ts, BorderStrategy rs, BorderStrategy bs, BorderStrategy ls);

        public:
            void motion_stop(bool horizon, bool vertical);
            void motion_bounce(bool horizon, bool vertical);

        private:
            WarGrey::STEM::BorderStrategy border_strategies[4];
            float xspeed = 0.0F;
            float yspeed = 0.0F;
    };
}

