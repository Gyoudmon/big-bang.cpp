#include "citizen.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
void WarGrey::STEM::Citizen::construct(SDL_Renderer* renderer) {
    Sprite::construct(renderer);
    this->auto_virtual_canvas();
}

void WarGrey::STEM::Citizen::on_heading_changed(float theta_rad, float vx, float vy, float prev_vr) {
    I8WayMotion::dispatch_heading_event(theta_rad, vx, vy, prev_vr);
}

void WarGrey::STEM::Citizen::on_eward(float theta_rad, float vx, float vy) {
    this->play("walk_e_");
}

void WarGrey::STEM::Citizen::on_wward(float theta_rad, float vx, float vy) {
    this->play("walk_w_");
}

void WarGrey::STEM::Citizen::on_sward(float theta_rad, float vx, float vy) {
    this->play("walk_s_");
}

void WarGrey::STEM::Citizen::on_nward(float theta_rad, float vx, float vy) {
    this->play("walk_n_");
}

void WarGrey::STEM::Citizen::on_esward(float theta_rad, float vx, float vy) {
    this->play("walk_es_");
}

void WarGrey::STEM::Citizen::on_enward(float theta_rad, float vx, float vy) {
    this->play("walk_en_");
}

void WarGrey::STEM::Citizen::on_wsward(float theta_rad, float vx, float vy) {
    this->play("walk_ws_");
}

void WarGrey::STEM::Citizen::on_wnward(float theta_rad, float vx, float vy) {
    this->play("walk_wn_");
}
