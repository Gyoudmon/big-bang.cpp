#include "map2d.hpp"

#include "../../datum/flonum.hpp"

using namespace WarGrey::STEM;

static const float pi_f125 = q_pi_f * 0.5F;

static const float theta_threholds [] = {
    pi_f125,
    pi_f125 + q_pi_f, pi_f125 + h_pi_f, pi_f125 + q_pi_f * 3.0F, pi_f125 + pi_f,
    pi_f125 + q_pi_f * 5.0F, pi_f125 + q_pi_f * 6.0F, pi_f125 + q_pi_f * 7.0F
};

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

/*************************************************************************************************/
void WarGrey::STEM::I8WayMotion::dispatch_heading_event(float theta_rad, float vx, float vy, float prev_vr) {
    float theta = theta_rad;
    
    if (theta < 0.0F) {
        theta = pi_f * 2.0F + theta;
    }

    if (theta <= theta_threholds[0]) {
        this->on_eward(theta_rad, vx, vy);
    } else if (theta <= theta_threholds[1]) {
        this->on_esward(theta_rad, vx, vy);
    } else if (theta <= theta_threholds[2]) {
        this->on_sward(theta_rad, vx, vy);
    } else if (theta <= theta_threholds[3]) {
        this->on_wsward(theta_rad, vx, vy);
    } else if (theta <= theta_threholds[4]) {
        this->on_wward(theta_rad, vx, vy);
    } else if (theta <= theta_threholds[5]) {
        this->on_wnward(theta_rad, vx, vy);
    } else if (theta <= theta_threholds[6]) {
        this->on_nward(theta_rad, vx, vy);
    } else if (theta <= theta_threholds[7]) {
        this->on_enward(theta_rad, vx, vy);
    } else {
        this->on_eward(theta_rad, vx, vy);
    }
}
