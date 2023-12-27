#include "map2d.hpp"

#include "../../datum/flonum.hpp"

using namespace GYDM;

static const double pi_125 = q_pi * 0.5;

static const double theta_thresholds [] = {
    pi_125,
    pi_125 + q_pi, pi_125 + h_pi, pi_125 + q_pi * 3.0, pi_125 + pi,
    pi_125 + q_pi * 5.0, pi_125 + q_pi * 6.0, pi_125 + q_pi * 7.0
};

/*************************************************************************************************/
void GYDM::I4WayMotion::dispatch_heading_event(double theta_rad, double vx, double vy, double prev_vr) {
    double theta = flabs(theta_rad);

    if (theta < q_pi) {
        this->on_eward(theta_rad, vx, vy);
    } else if (theta > q_pi * 3.0) {
        this->on_wward(theta_rad, vx, vy);
    } else if (theta_rad >= 0.0) {
        this->on_sward(theta_rad, vx, vy);
    } else {
        this->on_nward(theta_rad, vx, vy);
    }
}

/*************************************************************************************************/
void GYDM::I8WayMotion::dispatch_heading_event(double theta_rad, double vx, double vy, double prev_vr) {
    double theta = theta_rad;
    
    if (theta < 0.0) {
        theta = pi * 2.0 + theta;
    }

    if (theta <= theta_thresholds[0]) {
        this->on_eward(theta_rad, vx, vy);
    } else if (theta <= theta_thresholds[1]) {
        this->on_esward(theta_rad, vx, vy);
    } else if (theta <= theta_thresholds[2]) {
        this->on_sward(theta_rad, vx, vy);
    } else if (theta <= theta_thresholds[3]) {
        this->on_wsward(theta_rad, vx, vy);
    } else if (theta <= theta_thresholds[4]) {
        this->on_wward(theta_rad, vx, vy);
    } else if (theta <= theta_thresholds[5]) {
        this->on_wnward(theta_rad, vx, vy);
    } else if (theta <= theta_thresholds[6]) {
        this->on_nward(theta_rad, vx, vy);
    } else if (theta <= theta_thresholds[7]) {
        this->on_enward(theta_rad, vx, vy);
    } else {
        this->on_eward(theta_rad, vx, vy);
    }
}
