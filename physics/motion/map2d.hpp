#pragma once

#include "../../forward.hpp"

namespace Plteen {
    class __lambda__ I4WayMotion {
    protected:
        virtual void dispatch_heading_event(double theta_rad, double vx, double vy, double prev_vr);

    protected:
        virtual void on_nward(double theta_rad, double vx, double vy) = 0;
        virtual void on_eward(double theta_rad, double vx, double vy) = 0;
        virtual void on_sward(double theta_rad, double vx, double vy) = 0;
        virtual void on_wward(double theta_rad, double vx, double vy) = 0;
    };

    class __lambda__ I8WayMotion : public Plteen::I4WayMotion {
    protected:
        virtual void dispatch_heading_event(double theta_rad, double vx, double vy, double prev_vr);

    protected:
        virtual void on_enward(double theta_rad, double vx, double vy) = 0;
        virtual void on_wnward(double theta_rad, double vx, double vy) = 0;
        virtual void on_esward(double theta_rad, double vx, double vy) = 0;
        virtual void on_wsward(double theta_rad, double vx, double vy) = 0;
    };
}
