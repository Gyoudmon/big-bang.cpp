#include "graphlet.hpp"
#include "planet.hpp"

#include "datum/string.hpp"

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

