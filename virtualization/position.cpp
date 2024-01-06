#include "../matter.hpp"
#include "../plane.hpp"

#include "position.hpp"

using namespace GYDM;

/*************************************************************************************************/
GYDM::Position::Position(const GYDM::Position& pos) : dot(pos.dot), offset(pos.offset) {
    this->xtarget = pos.xtarget;
    this->ytarget = pos.ytarget;
}

Position& GYDM::Position::operator=(const GYDM::Position& pos) {
    this->dot = pos.dot;
    this->xtarget = pos.xtarget;
    this->ytarget = pos.ytarget;
    this->offset = pos.offset;

    return *this;
}

/*************************************************************************************************/
Point<float> GYDM::Position::calculate_point() const {
    Point<float> pos;

    if (this->xtarget == nullptr) {
        pos.x = this->dot.fx;
        pos.y = this->dot.fy;
    } else if (this->ytarget == nullptr) {
        IPlane* master = this->xtarget->master();

        if (master != nullptr) {
            pos = master->get_matter_location(const_cast<IMatter*>(this->xtarget), this->dot);
        }
    } else {
        IPlane* xmaster = this->xtarget->master();
        IPlane* ymaster = this->ytarget->master();
        Dot xdot, ydot;

        if (xmaster != nullptr) {
            xdot = xmaster->get_matter_location(const_cast<IMatter*>(this->xtarget), this->dot);
        }
        
        if (ymaster != nullptr) {
            ydot = ymaster->get_matter_location(const_cast<IMatter*>(this->ytarget), this->dot);
        }

        pos.x = xdot.x;
        pos.y = ydot.y;
    }

    pos.x += this->offset.x;
    pos.y += this->offset.y;

    return pos;
}

std::string GYDM::Position::desc() const {
    std::string description;

    if (this->xtarget == nullptr) {
        description = this->dot.desc();
    } else if (this->ytarget == nullptr) {
        description = this->dot.desc() + "@" + const_cast<IMatter*>(this->xtarget)->name();
    } else {
        description = "(" + std::to_string(this->dot.fx) + "@" + const_cast<IMatter*>(this->xtarget)->name()
                    + ", " + std::to_string(this->dot.fy) + "@" + const_cast<IMatter*>(this->ytarget)->name() + ")";
    }

    return description; 
}
