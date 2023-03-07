#include "map2d.hpp"

#include "../../datum/flonum.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
void WarGrey::STEM::I4WayMotion::dispatch_heading_event(float theta_rad, float vx, float vy, float prev_vr) {
    float theta = flabs(theta_rad);

    if (theta < q_pi_f) {
        this->on_eward(theta_rad, vx, vy);
    } else if (theta > q_pi_f * 3.0F) {
        this->on_wward(theta_rad, vx, vy);
    } else if (theta_rad >= 0.0F) {
        this->on_sward(theta_rad, vx, vy);
    } else {
        this->on_nward(theta_rad, vx, vy);
    }
}
