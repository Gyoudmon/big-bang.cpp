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
        void set_acceleration(float acc, float direction, bool is_radian = false);
        void add_acceleration(float acc, float direction, bool is_radian = false);
        float get_acceleration(float* direction = nullptr, float* vx = nullptr, float* vy = nullptr);
        float get_acceleration_direction(bool need_radian = true);

        void set_delta_speed(float xacc, float yacc);
        void add_delta_speed(float xacc, float yacc);
        float x_delta_speed() { return this->ax; }
        float y_delta_speed() { return this->ay; }

    public:
        void set_velocity(float spd, float direction, bool is_radian = false);
        void add_velocity(float spd, float direction, bool is_radian = false);
        float get_velocity(float* direction = nullptr, float* vx = nullptr, float* vy = nullptr);
        float get_velocity_direction(bool need_radian = true);
        
        void set_speed(float xspd, float yspd);
        void add_speed(float xspd, float yspd);
        float x_speed() { return this->vx; }
        float y_speed() { return this->vy; }

    public:
        void set_terminal_velocity(float max_spd, float direction, bool is_radian = false);
        void set_terminal_speed(float mxspd, float myspd) { this->mvx = mxspd; this->mvy = myspd; }
        float get_heading(bool need_radian = true) { return this->get_velocity_direction(); }
        void heading_rotate(float theta, bool is_radian = false);

    public:
        void set_border_strategy(WarGrey::STEM::BorderStrategy s);
        void set_border_strategy(WarGrey::STEM::BorderStrategy vs, WarGrey::STEM::BorderStrategy hs);
        void set_border_strategy(BorderStrategy ts, BorderStrategy rs, BorderStrategy bs, BorderStrategy ls);

    public:
        void step(float* sx, float* sy);
        void motion_stop(bool horizon = true, bool vertical = true);
        void motion_bounce(bool horizon, bool vertical);
        void disable_acceleration_bounce(bool yes = true) { this->bounce_acc = !yes; }
    
    public:
        bool x_stopped() { return (this->ax == 0.0F) && (this->vx == 0.0F); }
        bool y_stopped() { return (this->ay == 0.0F) && (this->vy == 0.0F); }
        bool motion_stopped() { return this->x_stopped() && this->y_stopped(); }

    protected:
        virtual void on_heading_changed(float theta_rad, float vx, float vy) {}
        virtual void on_motion_stopped() {}

    private:
        void on_acceleration_changed();
        void check_velocity_changing();
        void on_velocity_changed();

    private:
        WarGrey::STEM::BorderStrategy border_strategies[4];
        float bounce_acc = false;
        float ar = 0.0F;
        float ax = 0.0F;
        float ay = 0.0F;
        float vr = 0.0F;
        float vx = 0.0F;
        float vy = 0.0F;
        float mvx = 0.0F;
        float mvy = 0.0F;
    };

    class I4WayMotion {
    public:
        virtual void dispatch_heading_event(float theta_rad, float vx, float vy);

    protected:
        virtual void on_nward(float theta_rad, float vx, float vy) = 0;
        virtual void on_eward(float theta_rad, float vx, float vy) = 0;
        virtual void on_sward(float theta_rad, float vx, float vy) = 0;
        virtual void on_wward(float theta_rad, float vx, float vy) = 0;
    };

    class I8WayMotion : public WarGrey::STEM::I4WayMotion {
    public:
        virtual void dispatch_heading_event(float theta_rad, float vx, float vy);

    protected:
        virtual void on_neward(float theta_rad, float vx, float vy) = 0;
        virtual void on_nwward(float theta_rad, float vx, float vy) = 0;
        virtual void on_esward(float theta_rad, float vx, float vy) = 0;
        virtual void on_wsward(float theta_rad, float vx, float vy) = 0;
    };
}
