#include "movable.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
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
        this->xspeed = 0.0F;
        this->yspeed = 0.0F;
    } else {
        if (hstrategy == BorderStrategy::BOUNCE) {
            this->xspeed *= -1.0F;
        }

        if (vstrategy == BorderStrategy::BOUNCE) {
            this->yspeed *= -1.0F;
        }
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

void WarGrey::STEM::IMovable::motion_bounce(bool horizon, bool vertical) {
    if (horizon) {
        this->xspeed *= -1.0F;
    }
    
    if (vertical) {
        this->yspeed *= -1.0F;
    }
}

void WarGrey::STEM::IMovable::motion_stop(bool horizon, bool vertical) {
    if (horizon) {
        this->xspeed = 0.0F;
    }
    
    if (vertical) {
        this->yspeed = 0.0F;
    }
}

