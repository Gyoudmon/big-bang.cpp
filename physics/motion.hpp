#pragma once

#include "../forward.hpp"

namespace WarGrey::STEM {
    class IMovable {
    public:
        IMovable();
        virtual ~IMovable() {}

    public: // gameplay events
        virtual void on_border(float hoffset, float voffset);

    public:
        void set_acceleration(double acc, double direction, bool is_radian = false);
        void add_acceleration(double acc, double direction, bool is_radian = false);
        double get_acceleration(double* direction = nullptr, double* vx = nullptr, double* vy = nullptr);
        double get_acceleration_direction(bool need_radian = true);

        void set_delta_speed(double xacc, double yacc);
        void add_delta_speed(double xacc, double yacc);
        double x_delta_speed() { return this->ax; }
        double y_delta_speed() { return this->ay; }

    public:
        void set_velocity(double spd, double direction, bool is_radian = false);
        void add_velocity(double spd, double direction, bool is_radian = false);
        double get_velocity(double* direction = nullptr, double* vx = nullptr, double* vy = nullptr);
        double get_velocity_direction(bool need_radian = true);
        
        void set_speed(double xspd, double yspd);
        void add_speed(double xspd, double yspd);
        double x_speed() { return this->vx; }
        double y_speed() { return this->vy; }

    public:
        void set_terminal_velocity(double max_spd, double direction, bool is_radian = false);
        void set_terminal_speed(double mxspd, double myspd);
        double get_heading(bool need_radian = true) { return this->get_velocity_direction(); }
        void set_heading(double direction, bool is_radian = false);
        void heading_rotate(double theta, bool is_radian = false);

    public:
        void set_border_strategy(WarGrey::STEM::BorderStrategy s);
        void set_border_strategy(WarGrey::STEM::BorderStrategy vs, WarGrey::STEM::BorderStrategy hs);
        void set_border_strategy(BorderStrategy ts, BorderStrategy rs, BorderStrategy bs, BorderStrategy ls);

    public:
        void step(float* sx, float* sy);
        void step(double* sx, double* sy);
        void motion_stop(bool horizon = true, bool vertical = true);
        void motion_bounce(bool horizon, bool vertical);
        void disable_acceleration_bounce(bool yes = true) { this->bounce_acc = !yes; }
    
    public:
        bool x_stopped() { return (this->ax == 0.0) && (this->vx == 0.0); }
        bool y_stopped() { return (this->ay == 0.0) && (this->vy == 0.0); }
        bool motion_stopped() { return this->x_stopped() && this->y_stopped(); }

    protected:
        virtual void on_heading_changed(double theta_rad, double vx, double vy, double prev_vr) {}
        virtual void on_motion_stopped() {}

    private:
        void on_acceleration_changed();
        void check_velocity_changing();
        void check_heading_changing(double rad);
        void on_velocity_changed();

    private:
        WarGrey::STEM::BorderStrategy border_strategies[4];
        bool bounce_acc = false;
        double ar = 0.0;
        double ax = 0.0;
        double ay = 0.0;
        double vr = 0.0;
        double vx = 0.0;
        double vy = 0.0;
        double tvx;
        double tvy;
    };
}
