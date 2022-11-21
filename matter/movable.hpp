#pragma once

#include "../forward.hpp"

namespace WarGrey::STEM {
    class IMovable {
        public: // gameplay events
            virtual void on_border(float hoffset, float voffset);

        public:
            void set_speed(float xspd, float yspd) { this->xspeed = xspd; this->yspeed = yspd; }
            void set_border_strategy(WarGrey::STEM::BorderStrategy s);
            void set_border_strategy(WarGrey::STEM::BorderStrategy vs, WarGrey::STEM::BorderStrategy hs);
            void set_border_strategy(BorderStrategy ts, BorderStrategy rs, BorderStrategy bs, BorderStrategy ls);
            void fill_speed(float* xspd, float* yspd) { if (xspd != nullptr) (*xspd) = this->xspeed; if (yspd != nullptr) (*yspd) = this->yspeed; }
            void motion_stop(bool horizon, bool vertical);
            void motion_bounce(bool horizon, bool vertical);

        private:
            WarGrey::STEM::BorderStrategy border_strategies[4];
            float xspeed = 0.0F;
            float yspeed = 0.0F;
    };
}

