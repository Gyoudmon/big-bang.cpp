#include "motion.hpp"

#include "../datum/box.hpp"

#include "../physics/mathematics.hpp"

using namespace GYDM;

/*************************************************************************************************/
GYDM::IMovable::IMovable() {
    this->set_border_strategy(BorderStrategy::IGNORE);
    this->motion_stop(true, true);

    this->tvx = infinity;
    this->tvy = infinity;
    this->ar = flnan;
    this->vr = 0.0; // not using `flnan` for heading
}

void GYDM::IMovable::set_acceleration(double acc, double dir, bool is_radian) {
    double ax = 0.0;
    double ay = 0.0;

    orthogonal_decompose(acc, dir, &ax, &ay, is_radian);
    this->set_delta_speed(ax, ay);
}

void GYDM::IMovable::set_delta_speed(double xacc, double yacc) {
    bool xchanged = (this->ax != xacc);
    bool ychanged = (this->ay != yacc);

    if (xchanged) this->ax = xacc;
    if (ychanged) this->ay = yacc;

    if (xchanged || ychanged) {
        this->on_acceleration_changed();
    }
}

void GYDM::IMovable::add_delta_speed(double xacc, double yacc) {
    bool xchanged = (xacc != 0.0);
    bool ychanged = (yacc != 0.0);

    if (xchanged) this->ax += xacc;
    if (ychanged) this->ay += yacc;

    if (xchanged || ychanged) {
        this->on_acceleration_changed();
    }
}

void GYDM::IMovable::add_acceleration(double acc, double dir, bool is_radian) {
    double ax = 0.0;
    double ay = 0.0;

    orthogonal_decompose(acc, dir, &ax, &ay, is_radian);
    this->add_delta_speed(ax, ay);
}

double GYDM::IMovable::get_acceleration(double* direction, double* x, double* y) {
    double mag = vector_magnitude(this->ax, this->ay);

    SET_BOX(direction, this->get_acceleration_direction());
    SET_VALUES(x, this->ax, y, this->ay);

    return mag;
}

double GYDM::IMovable::get_acceleration_direction(bool need_radian) {
    double rad = this->ar;

    if (flisnan(rad)) {
        rad = flatan(this->ay, this->ax);
    }

    return (need_radian ? rad : radians_to_degrees(rad));
}

void GYDM::IMovable::set_velocity(double spd, double dir, bool is_radian) {
    double vx = 0.0;
    double vy = 0.0;

    orthogonal_decompose(spd, dir, &vx, &vy, is_radian);

    this->set_speed(vx, vy);
}

void GYDM::IMovable::add_velocity(double spd, double dir, bool is_radian) {
    double vx = 0.0;
    double vy = 0.0;

    orthogonal_decompose(spd, dir, &vx, &vy, is_radian);
    this->add_speed(vx, vy);
}

void GYDM::IMovable::set_speed(double xspd, double yspd) {
    bool trigger_heading_event = this->motion_stopped();

    xspd = vector_clamp(xspd, this->tvx);
    yspd = vector_clamp(yspd, this->tvy);

    bool xchanged = (this->vx != xspd);
    bool ychanged = (this->vy != yspd);

    if (xchanged) this->vx = xspd;
    if (ychanged) this->vy = yspd;
    
    if (xchanged || ychanged) {
        this->on_velocity_changed(trigger_heading_event);
    }
}

void GYDM::IMovable::add_speed(double xspd, double yspd) {
    this->set_speed(this->vx + xspd, this->vy + yspd);
}

double GYDM::IMovable::get_velocity(double* direction, double* x, double* y) {
    double mag = vector_magnitude(this->vx, this->vy);

    SET_BOX(direction, this->get_velocity_direction());
    SET_VALUES(x, this->vx, y, this->vy);

    return mag;
}

double GYDM::IMovable::get_velocity_direction(bool need_radian) {
    double rad = this->vr;

    return (need_radian ? rad : radians_to_degrees(rad));
}

void GYDM::IMovable::set_terminal_velocity(double v, double dir, bool is_radian) {
    double vx = 0.0;
    double vy = 0.0;

    orthogonal_decompose(v, dir, &vx, &vy, is_radian);
    this->set_terminal_speed(vx, vy);
}

void GYDM::IMovable::set_terminal_speed(double mxspd, double myspd) {
    bool changed = false;
    
    this->tvx = flabs(mxspd);
    this->tvy = flabs(myspd);

    if flout(-this->tvx, this->vx, this->tvx) {
        this->vx = vector_clamp(this->vx, this->tvx);
        changed = true;
    }

    if flout(-this->tvy, this->vy, this->tvy) {
        this->vy = vector_clamp(this->vy, this->tvy);
        changed = true;
    }

    if (changed) {
        this->on_velocity_changed(false);
    }
}

void GYDM::IMovable::set_heading(double dx, double dy) {
    // Usually this is meaningful when stopped
    this->set_heading(flatan(dy, dx), true);
}

void GYDM::IMovable::set_heading(double direction, bool is_radian) {
    // Usually this is meaningful when stopped

    if (!is_radian) {
        direction = degrees_to_radians(direction);
    }

    this->check_heading_changing(direction, false);
}

void GYDM::IMovable::add_heading(double theta, bool is_radian) {
    // Usually this is meaningful when stopped

    if (!is_radian) {
        theta = degrees_to_radians(theta);
    }

    this->check_heading_changing(this->vr + theta, false);
}

void GYDM::IMovable::heading_rotate(double theta, bool is_radian) {
    if (theta != 0.0) {
        vector_rotate(this->vx, this->vy, theta, &this->vx, &this->vy, 0.0, 0.0, is_radian);
        this->on_velocity_changed(false);
    }
}

void GYDM::IMovable::on_border(float hoffset, float voffset) {
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

void GYDM::IMovable::set_border_strategy(BorderStrategy s) {
    this->set_border_strategy(s, s, s, s);
}

void GYDM::IMovable::set_border_strategy(BorderStrategy vs, BorderStrategy hs) {
    this->set_border_strategy(vs, hs, vs, hs);
}
    
void GYDM::IMovable::set_border_strategy(BorderStrategy ts, BorderStrategy rs, BorderStrategy bs, BorderStrategy ls) {
    this->border_strategies[static_cast<int>(BorderEdge::TOP)] = ts;
    this->border_strategies[static_cast<int>(BorderEdge::RIGHT)] = rs;
    this->border_strategies[static_cast<int>(BorderEdge::BOTTOM)] = bs;
    this->border_strategies[static_cast<int>(BorderEdge::LEFT)] = ls;
}

void GYDM::IMovable::step(float* sx, float* sy) {
    double flsx = double(*sx);
    double flsy = double(*sy);
    
    this->step(&flsx, &flsy);
    (*sx) = float(flsx);
    (*sy) = float(flsy);
}

void GYDM::IMovable::step(double* sx, double* sy) {
    if (this->ax != 0.0) this->vx = vector_clamp(this->vx + this->ax, this->tvx);
    if (this->ay != 0.0) this->vy = vector_clamp(this->vy + this->ay, this->tvy);
    this->check_velocity_changing();

    if (this->vx != 0.0) (*sx) += this->vx;
    if (this->vy != 0.0) (*sy) += this->vy;
}

void GYDM::IMovable::motion_bounce(bool horizon, bool vertical) {
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
        this->on_velocity_changed(false);

        if (this->bounce_acc) {
            this->on_acceleration_changed();
        }
    }
}

void GYDM::IMovable::motion_stop(bool horizon, bool vertical) {
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
        // this->vr = flnan; // leaving it for heading
        this->on_motion_stopped();
    } else {
        this->on_acceleration_changed();
        this->on_velocity_changed(false);
    }
}

/*************************************************************************************************/
void GYDM::IMovable::on_acceleration_changed() {
    this->ar = flatan(this->ay, this->ax);
}

void GYDM::IMovable::on_velocity_changed(bool always_trigger_heading_event) {
    double rad = flatan(this->vy, this->vx);

    this->check_heading_changing(rad, always_trigger_heading_event);
}

void GYDM::IMovable::check_velocity_changing() {
    if ((this->ax != 0.0) || (this->ay != 0.0)) {
        if (this->ar != this->vr) {
            this->on_velocity_changed(false);
        }
    }
}

void GYDM::IMovable::check_heading_changing(double rad, bool always_trigger_event) {
    if (this->vr != rad) {
        double pvr = this->vr;

        this->vr = rad;
        this->on_heading_changed(rad, this->vx, this->vy, pvr);
    } else if (always_trigger_event) {
        this->on_heading_changed(this->vr, this->vx, this->vy, this->vr);
    }
}
