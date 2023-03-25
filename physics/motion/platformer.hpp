#pragma once

#include "../../forward.hpp"

namespace WarGrey::STEM {
    class IPlatformMotion {
    public:
        IPlatformMotion(bool facing_right = true, bool walk_only = false)
            : default_facing_sgn(facing_right ? 1.0 : -1.0), walk_only(walk_only) {}

    protected:
        virtual void dispatch_heading_event(double theta_rad, double vx, double vy, double prev_vr);
        virtual void on_walk(double theta_rad, double vx, double vy) {}
        virtual void on_jump(double theta_rad, double vx, double vy) {}

    protected:
        virtual void feed_flip_signs(double* hsgn, double* vsgn) = 0;
        virtual void horizontal_flip() = 0;

    private:
        double default_facing_sgn;
        bool walk_only;
    };
}
