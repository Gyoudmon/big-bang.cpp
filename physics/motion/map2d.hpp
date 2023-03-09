#pragma once

#include "../../forward.hpp"

namespace WarGrey::STEM {
    class I4WayMotion {
    protected:
        virtual void dispatch_heading_event(float theta_rad, float vx, float vy, float prev_vr);

    protected:
        virtual void on_nward(float theta_rad, float vx, float vy) = 0;
        virtual void on_eward(float theta_rad, float vx, float vy) = 0;
        virtual void on_sward(float theta_rad, float vx, float vy) = 0;
        virtual void on_wward(float theta_rad, float vx, float vy) = 0;
    };

    class I8WayMotion : public WarGrey::STEM::I4WayMotion {
    protected:
        virtual void dispatch_heading_event(float theta_rad, float vx, float vy, float prev_vr);

    protected:
        virtual void on_enward(float theta_rad, float vx, float vy) = 0;
        virtual void on_wnward(float theta_rad, float vx, float vy) = 0;
        virtual void on_esward(float theta_rad, float vx, float vy) = 0;
        virtual void on_wsward(float theta_rad, float vx, float vy) = 0;
    };
}
