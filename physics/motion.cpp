#include "motion.hpp"

#include "../datum/box.hpp"

#include "../graphics/geometry.hpp"
#include "../physics/mathematics.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::IMovable::IMovable() {
    this->set_border_strategy(BorderStrategy::IGNORE);
    this->motion_stop(true, true);
}

void WarGrey::STEM::IMovable::set_acceleration(double acc, double dir, bool is_radian) {
    double ax, ay;

    orthogonal_decomposition(acc, dir, &ax, &ay, is_radian);
    this->set_delta_speed(ax, ay);
}

void WarGrey::STEM::IMovable::set_delta_speed(double xacc, double yacc) {
    this->ax = xacc;
    this->ay = yacc;
    this->on_acceleration_changed();
}

void WarGrey::STEM::IMovable::add_delta_speed(double xacc, double yacc) {
    this->ax += xacc;
    this->ay += yacc;
    this->on_acceleration_changed();
}

void WarGrey::STEM::IMovable::add_acceleration(double acc, double dir, bool is_radian) {
    double ax, ay;

    orthogonal_decomposition(acc, dir, &ax, &ay, is_radian);
    this->add_delta_speed(ax, ay);
}

double WarGrey::STEM::IMovable::get_acceleration(double* direction, double* x, double* y) {
    double mag = vector_magnitude(this->ax, this->ay);

    SET_BOX(direction, this->get_acceleration_direction());
    SET_VALUES(x, this->ax, y, this->ay);

    return mag;
}

double WarGrey::STEM::IMovable::get_acceleration_direction(bool need_radian) {
    double rad = this->ar;

    if (flisnan(rad)) {
        rad = flatan(this->ay, this->ax);
    }

    return (need_radian ? rad : radians_to_degrees(rad));
}

void WarGrey::STEM::IMovable::set_velocity(double spd, double dir, bool is_radian) {
    double rad = dir;
    
    if (!is_radian) {
        rad = degrees_to_radians(dir);
    }

    this->set_speed(spd * flcos(rad), spd * flsin(rad));
}

void WarGrey::STEM::IMovable::add_velocity(double spd, double dir, bool is_radian) {
    double rad = dir;
    
    if (!is_radian) {
        rad = degrees_to_radians(dir);
    }

    this->add_speed(spd * flcos(rad), spd * flsin(rad));
}

void WarGrey::STEM::IMovable::set_speed(double xspd, double yspd) {
    this->vx = xspd;
    this->vy = yspd;
    this->on_velocity_changed();
}

void WarGrey::STEM::IMovable::add_speed(double xspd, double yspd) {
    this->vx += xspd;
    this->vy += yspd;
    this->on_velocity_changed();
}

double WarGrey::STEM::IMovable::get_velocity(double* direction, double* x, double* y) {
    double mag = vector_magnitude(this->vx, this->vy);

    SET_BOX(direction, this->get_velocity_direction());
    SET_VALUES(x, this->vx, y, this->vy);

    return mag;
}

double WarGrey::STEM::IMovable::get_velocity_direction(bool need_radian) {
    double rad = this->vr;

    if (flisnan(rad)) {
        rad = flatan(this->vy, this->vx);
    }

    return (need_radian ? rad : radians_to_degrees(rad));
}

void WarGrey::STEM::IMovable::set_terminal_velocity(double spd, double dir, bool is_radian) {
    double rad = dir;
    
    if (!is_radian) {
        rad = degrees_to_radians(dir);
    }

    this->set_terminal_speed(spd * flcos(rad), spd * flsin(rad));
}

void WarGrey::STEM::IMovable::set_terminal_speed(double mxspd, double myspd) {
    bool changed = false;

    this->tvx = mxspd;
    this->tvy = myspd;

    if ((this->tvx > 0.0F) && (this->vx > this->tvx)) {
        this->vx = this->tvx;
        changed = true;
    }

    if ((this->tvy > 0.0F) && (this->vy > this->tvy)) {
        this->vy = this->tvy;
        changed = true;
    }

    if (changed) {
        this->on_velocity_changed();
    }
}       

void WarGrey::STEM::IMovable::heading_rotate(double theta, bool is_radian) {
    vector_rotate(this->vx, this->vy, theta, &this->vx, &this->vy, 0.0, 0.0, is_radian);
}

void WarGrey::STEM::IMovable::on_border(float hoffset, float voffset) {
    BorderStrategy hstrategy = BorderStrategy::IGNORE;
    BorderStrategy vstrategy = BorderStrategy::IGNORE;

    if (hoffset < 0.0F) {
        hstrategy = this->border_strategies[static_cast<int>(BorderEdge::LEFT)];
    } else if (hoffset > 0.0F) {
        hstrategy = this->border_strategies[static_cast<int>(BorderEdge::RIGHT)];
    }

    if (voffset < 0.0F) {
        vstrategy = this->border_strategies[static_cast<int>(BorderEdge::TOP)];
    } else if (voffset > 0.0F) {
        vstrategy = this->border_strategies[static_cast<int>(BorderEdge::BOTTOM)];
    }

    if ((hstrategy == BorderStrategy::STOP) || (vstrategy == BorderStrategy::STOP)) {
        this->motion_stop(true, true); // Yes, if stopping, both direction should stop. 
    } else if ((hstrategy == BorderStrategy::BOUNCE) || (vstrategy == BorderStrategy::BOUNCE)) {
        this->motion_bounce((hstrategy == BorderStrategy::BOUNCE), (vstrategy == BorderStrategy::BOUNCE));
    }
}

void WarGrey::STEM::IMovable::set_border_strategy(BorderStrategy s) {
    this->set_border_strategy(s, s, s, s);
}

void WarGrey::STEM::IMovable::set_border_strategy(BorderStrategy vs, BorderStrategy hs) {
    this->set_border_strategy(vs, hs, vs, hs);
}
    
void WarGrey::STEM::IMovable::set_border_strategy(BorderStrategy ts, BorderStrategy rs, BorderStrategy bs, BorderStrategy ls) {
    this->border_strategies[static_cast<int>(BorderEdge::TOP)] = ts;
    this->border_strategies[static_cast<int>(BorderEdge::RIGHT)] = rs;
    this->border_strategies[static_cast<int>(BorderEdge::BOTTOM)] = bs;
    this->border_strategies[static_cast<int>(BorderEdge::LEFT)] = ls;
}

void WarGrey::STEM::IMovable::step(float* sx, float* sy) {
    double flsx = double(*sx);
    double flsy = double(*sy);
    
    this->step(&flsx, &flsy);
    (*sx) = float(flsx);
    (*sy) = float(flsy);
}

void WarGrey::STEM::IMovable::step(double* sx, double* sy) {
    this->vx += this->ax;
    this->vy += this->ay;

    if (this->tvx > 0.0) {
        if (this->vx > this->tvx) {
            this->vx = this->tvx;
        } else if (this->vx < -this->tvx) {
            this->vx = -this->tvx;
        }
    }

    if (this->tvy > 0.0) {
        if (this->vy > this->tvy) {
            this->vy = this->tvy;
        } else if (this->vy < -this->tvy) {
            this->vy = -this->tvy;
        }
    }

    this->check_velocity_changing();

    (*sx) += this->vx;
    (*sy) += this->vy;
}

void WarGrey::STEM::IMovable::motion_bounce(bool horizon, bool vertical) {
    if (horizon) {
        this->vx *= -1.0;

        if (this->bounce_acc) {
            this->ax *= -1.0;
        }
    }
    
    if (vertical) {
        this->vy *= -1.0;

        if (this->bounce_acc) {
            this->ay *= -1.0;
        }
    }

    if (horizon || vertical) {
        this->on_velocity_changed();

        if (this->bounce_acc) {
            this->on_acceleration_changed();
        }
    }
}

void WarGrey::STEM::IMovable::motion_stop(bool horizon, bool vertical) {
    if (horizon) {
        this->vx = 0.0;
        this->ax = 0.0;
    }
    
    if (vertical) {
        this->vy = 0.0;
        this->ay = 0.0;
    }

    if (horizon && vertical) {
        this->ar = flnan;
        this->vr = flnan;
        this->on_motion_stopped();
    } else {
        this->on_acceleration_changed();
        this->on_velocity_changed();
    }
}

/*************************************************************************************************/
void WarGrey::STEM::IMovable::on_acceleration_changed() {
    this->ar = flatan(this->ay, this->ax);
}

void WarGrey::STEM::IMovable::on_velocity_changed() {
    double rad = flatan(this->vy, this->vx);

    if (this->vr != rad) {
        double pvr = this->vr;

        this->vr = rad;
        this->on_heading_changed(rad, this->vx, this->vy, pvr);
    }
}

void WarGrey::STEM::IMovable::check_velocity_changing() {
    if ((this->ax != 0.0) || (this->ay != 0.0)) {
        if (this->ar != this->vr) {
            this->on_velocity_changed();
        }
    }
}
