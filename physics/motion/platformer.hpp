#pragma once

#include "../../forward.hpp"

namespace WarGrey::STEM {
    class IPlatformMotion {
    public:
        IPlatformMotion(bool facing_right = true, bool walk_only = false)
            : default_facing_sgn(facing_right ? 1.0F : -1.0F), walk_only(walk_only) {}

    protected:
        virtual void dispatch_heading_event(float theta_rad, float vx, float vy, float prev_vr);
        virtual void on_walk(float theta_rad, float vx, float vy) {}
        virtual void on_jump(float theta_rad, float vx, float vy) {}

    protected:
        virtual void feed_flip_signs(float* hsgn, float* vsgn) = 0;
        virtual void horizontal_flip() = 0;

    private:
        float default_facing_sgn;
        bool walk_only;
    };
}
