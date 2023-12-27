#include "platformer.hpp"

#include "../../datum/flonum.hpp"

using namespace GYDM;

/*************************************************************************************************/
void GYDM::IPlatformMotion::dispatch_heading_event(double theta_rad, double vx, double vy, double pvr) {
    double hsgn, vsgn;

    this->feed_flip_signs(&hsgn, &vsgn);

    if (flsign(vx) * hsgn * this->default_facing_sgn == -1.0F) {
        this->horizontal_flip();    
    }

    if (this->walk_only) {
        if (vx != 0.0) {
            this->on_walk(theta_rad, vx, vy);
        }
    } else {
        if (vy == 0.0) {
            this->on_walk(theta_rad, vx, vy);
        } else if (vy < 0.0) {
            this->on_jump(theta_rad, vx, vy);
        }
    }
}
