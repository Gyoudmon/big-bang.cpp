#include "../matter.hpp"
#include "../plane.hpp"

#include "position.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
Point<float> WarGrey::STEM::Position::calculate_dot() const {
    Point<float> pos;

    if (this->xtarget == nullptr) {
        pos.x = this->dot.fx;
        pos.y = this->dot.fy;
    } else if (this->ytarget == nullptr) {
        IPlane* master = this->xtarget->master();

        if (master != nullptr) {
            master->feed_matter_location(const_cast<IMatter*>(this->xtarget), &pos.x, &pos.y, this->dot);
        }
    } else {
        IPlane* xmaster = this->xtarget->master();
        IPlane* ymaster = this->ytarget->master();

        if (xmaster != nullptr) {
            xmaster->feed_matter_location(const_cast<IMatter*>(this->xtarget), &pos.x, nullptr, this->dot);
        }
        
        if (ymaster != nullptr) {
            ymaster->feed_matter_location(const_cast<IMatter*>(this->ytarget), nullptr, &pos.y, this->dot);
        }
    }

    pos.x += this->offset.x;
    pos.y += this->offset.y;

    return pos;
}
