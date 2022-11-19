#include "graphlet.hpp"
#include "planet.hpp"

#include "datum/string.hpp"
#include "datum/box.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::IGraphlet::~IGraphlet() {
    if (this->info != nullptr) {
        delete this->info;
        this->info = nullptr;
    }
}

IPlanet* WarGrey::STEM::IGraphlet::master() {
    IPlanet* planet = nullptr;

    if (this->info != nullptr) {
        planet = this->info->master;
    }

    return planet;
}

void WarGrey::STEM::IGraphlet::fill_extent(float x, float y, float* w, float* h) {
    SET_VALUES(w, 0.0F, h, 0.0F);
}

void WarGrey::STEM::IGraphlet::notify_updated() {
    if (this->info != nullptr) {
        if (this->anchor != GraphletAnchor::LT) {
            this->info->master->move_to(this, this->anchor_x, this->anchor_y, this->anchor);
            this->clear_moor();
        }

        this->info->master->notify_updated();
    }
}

void WarGrey::STEM::IGraphlet::moor(GraphletAnchor anchor) {
    if (anchor != GraphletAnchor::LT) {
        if (this->info != nullptr) {
            this->anchor = anchor;
            this->info->master->fill_graphlet_location(this, &this->anchor_x, &this->anchor_y, anchor);
        }
    }
}

void WarGrey::STEM::IGraphlet::clear_moor() {
    this->anchor = GraphletAnchor::LT;
}

bool WarGrey::STEM::IGraphlet::has_caret() {
    bool careted = false;

    if (this->info != nullptr) {
        careted = (this->info->master->get_focus_graphlet() == this);
    }

    return careted;
}

void WarGrey::STEM::IGraphlet::fill_location(float* x, float* y, WarGrey::STEM::GraphletAnchor a) {
    if (this->info != nullptr) {
        this->info->master->fill_graphlet_location(this, x, y, a);
    }
}

void WarGrey::STEM::IGraphlet::send_message(const char* fmt, ...) {
    if (this->info != nullptr) {
        VSNPRINT(text, fmt);
        this->send_message(-1, text);
    }
}

void WarGrey::STEM::IGraphlet::send_message(int fgc, const char* fmt, ...) {
    if (this->info != nullptr) {
        VSNPRINT(text, fmt);
        this->send_message(fgc, text);
    }
}

void WarGrey::STEM::IGraphlet::send_message(int fgc, const std::string& msg) {
    if (this->info != nullptr) {
        this->info->master->send_message(fgc, msg);
    }
}

/*************************************************************************************************/
void WarGrey::STEM::IGraphlet::on_boundary(float hoffset, float voffset) {
    BorderCollisionStrategy hstrategy = BorderCollisionStrategy::IGNORE;
    BorderCollisionStrategy vstrategy = BorderCollisionStrategy::IGNORE;

    if (hoffset < 0.0F) {
        hstrategy = this->border_collision_strategies[static_cast<int>(BorderEdge::LEFT)];
    } else if (hoffset > 0.0F) {
        hstrategy = this->border_collision_strategies[static_cast<int>(BorderEdge::RIGHT)];
    }

    if (voffset < 0.0F) {
        vstrategy = this->border_collision_strategies[static_cast<int>(BorderEdge::TOP)];
    } else if (voffset > 0.0F) {
        vstrategy = this->border_collision_strategies[static_cast<int>(BorderEdge::BOTTOM)];
    }

    if ((hstrategy == BorderCollisionStrategy::STOP) || (vstrategy == BorderCollisionStrategy::STOP)) {
        this->xspeed = 0.0F;
        this->yspeed = 0.0F;
    } else {
        if (hstrategy == BorderCollisionStrategy::BOUNCE) {
            this->xspeed *= -1.0F;
        }

        if (vstrategy == BorderCollisionStrategy::BOUNCE) {
            this->yspeed *= -1.0F;
        }
    }
}

void WarGrey::STEM::IGraphlet::set_border_collision_strategy(BorderCollisionStrategy s) {
    this->set_border_collision_strategy(s, s, s, s);
}

void WarGrey::STEM::IGraphlet::set_border_collision_strategy(BorderCollisionStrategy vs, BorderCollisionStrategy hs) {
    this->set_border_collision_strategy(vs, hs, vs, hs);
}
    
void WarGrey::STEM::IGraphlet::set_border_collision_strategy(BorderCollisionStrategy ts, BorderCollisionStrategy rs, BorderCollisionStrategy bs, BorderCollisionStrategy ls) {
    this->border_collision_strategies[static_cast<int>(BorderEdge::TOP)] = ts;
    this->border_collision_strategies[static_cast<int>(BorderEdge::RIGHT)] = rs;
    this->border_collision_strategies[static_cast<int>(BorderEdge::BOTTOM)] = bs;
    this->border_collision_strategies[static_cast<int>(BorderEdge::LEFT)] = ls;
}

void WarGrey::STEM::IGraphlet::motion_bounce(bool horizon, bool vertical) {
    if (horizon) {
        this->xspeed *= -1.0F;
    }
    
    if (vertical) {
        this->yspeed *= -1.0F;
    }
}

void WarGrey::STEM::IGraphlet::motion_stop(bool horizon, bool vertical) {
    if (horizon) {
        this->xspeed = 0.0F;
    }
    
    if (vertical) {
        this->yspeed = 0.0F;
    }
}

