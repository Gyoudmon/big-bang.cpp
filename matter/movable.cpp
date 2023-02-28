#include "movable.hpp"

#include "../datum/box.hpp"

#include "../physics/mathematics.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::IMovable::IMovable() {
    this->set_border_strategy(BorderStrategy::IGNORE);
    this->motion_stop(true, true);
}

void WarGrey::STEM::IMovable::set_acceleration(float acc, float dir, bool is_radian) {
    float ax, ay;

    orthogonal_decomposition(acc, dir, &ax, &ay, is_radian);
    this->set_delta_speed(ax, ay);
}

void WarGrey::STEM::IMovable::set_delta_speed(float xacc, float yacc) {
    this->ax = xacc;
    this->ay = yacc;
    this->on_acceleration_changed();
}

void WarGrey::STEM::IMovable::add_delta_speed(float xacc, float yacc) {
    this->ax += xacc;
    this->ay += yacc;
    this->on_acceleration_changed();
}

void WarGrey::STEM::IMovable::add_acceleration(float acc, float dir, bool is_radian) {
    float ax, ay;

    orthogonal_decomposition(acc, dir, &ax, &ay, is_radian);
    this->add_delta_speed(ax, ay);
}

float WarGrey::STEM::IMovable::get_acceleration(float* direction, float* x, float* y) {
    float mag = vector_magnitude(this->ax, this->ay);

    SET_BOX(direction, this->get_acceleration_direction());
    SET_VALUES(x, this->ax, y, this->ay);

    return mag;
}

float WarGrey::STEM::IMovable::get_acceleration_direction(bool need_radian) {
    float rad = this->ar;

    if (flisnan(rad)) {
        rad = flatan(this->ay, this->ax);
    }

    return (need_radian ? rad : radians_to_degrees(rad));
}

void WarGrey::STEM::IMovable::set_velocity(float spd, float dir, bool is_radian) {
    float rad = dir;
    
    if (!is_radian) {
        rad = degrees_to_radians(dir);
    }

    this->set_speed(spd * flcos(rad), spd * flsin(rad));
}

void WarGrey::STEM::IMovable::add_velocity(float spd, float dir, bool is_radian) {
    float rad = dir;
    
    if (!is_radian) {
        rad = degrees_to_radians(dir);
    }

    this->add_speed(spd * flcos(rad), spd * flsin(rad));
}

void WarGrey::STEM::IMovable::set_speed(float xspd, float yspd) {
    this->vx = xspd;
    this->vy = yspd;
    this->on_velocity_changed();
}

void WarGrey::STEM::IMovable::add_speed(float xspd, float yspd) {
    this->vx += xspd;
    this->vy += yspd;
    this->on_velocity_changed();
}

float WarGrey::STEM::IMovable::get_velocity(float* direction, float* x, float* y) {
    float mag = vector_magnitude(this->vx, this->vy);

    SET_BOX(direction, this->get_velocity_direction());
    SET_VALUES(x, this->vx, y, this->vy);

    return mag;
}

float WarGrey::STEM::IMovable::get_velocity_direction(bool need_radian) {
    float rad = this->vr;

    if (flisnan(rad)) {
        rad = flatan(this->vy, this->vx);
    }

    return (need_radian ? rad : radians_to_degrees(rad));
}

void WarGrey::STEM::IMovable::set_terminal_velocity(float spd, float dir, bool is_radian) {
    float rad = dir;
    
    if (!is_radian) {
        rad = degrees_to_radians(dir);
    }

    this->set_terminal_speed(spd * flcos(rad), spd * flsin(rad));
}

void WarGrey::STEM::IMovable::heading_rotate(float theta, bool is_radian) {
    vector_rotate(this->vx, this->vy, theta, &this->vx, &this->vy, 0.0F, 0.0F, is_radian);
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
    this->vx += this->ax;
    this->vy += this->ay;

    if (this->mvx > 0.0F) {
        if (this->vx > this->mvx) {
            this->vx = this->mvx;
        } else if (this->vx < -this->mvx) {
            this->vx = -this->mvx;
        }
    }

    if (this->mvy > 0.0F) {
        if (this->vy > this->mvy) {
            this->vy = this->mvy;
        } else if (this->vy < -this->mvy) {
            this->vy = -this->mvy;
        }
    }

    this->check_velocity_changing();

    (*sx) += this->vx;
    (*sy) += this->vy;
}

void WarGrey::STEM::IMovable::motion_bounce(bool horizon, bool vertical) {
    if (horizon) {
        this->vx *= -1.0F;

        if (this->bounce_acc) {
            this->ax *= -1.0F;
        }
    }
    
    if (vertical) {
        this->vy *= -1.0F;

        if (this->bounce_acc) {
            this->ay *= -1.0F;
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
        this->vx = 0.0F;
        this->ax = 0.0F;
    }
    
    if (vertical) {
        this->vy = 0.0F;
        this->ay = 0.0F;
    }

    if (horizon && vertical) {
        this->ar = flnan_f;
        this->vr = flnan_f;
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
    float rad = flatan(this->vy, this->vx);

    if (this->vr != rad) {
        this->vr = rad;
        this->on_heading_changed(rad, this->vx, this->vy);
    }
}

void WarGrey::STEM::IMovable::check_velocity_changing() {
    if ((this->ax != 0.0F) || (this->ay != 0.0F)) {
        if (this->ar != this->vr) {
            this->on_velocity_changed();
        }
    }
}
