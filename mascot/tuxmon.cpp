#include "tuxmon.hpp"

#include "../datum/flonum.hpp"
#include "../graphics/geometry.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::Tuxmon::Tuxmon(bool walk_only)
    : IPlatformMotion(true, walk_only), Sprite(digimon_mascot_path("tuxmon", "")) {}

int WarGrey::STEM::Tuxmon::get_initial_costume_index() {
    return this->costume_name_to_index("idle-0");
}

void WarGrey::STEM::Tuxmon::on_heading_changed(float theta_rad, float vx, float vy, float prev_vr) {
    IPlatformMotion::dispatch_heading_event(theta_rad, vx, vy, prev_vr);
}

void WarGrey::STEM::Tuxmon::on_walk(float theta_rad, float vx, float vy) {
    this->play("walk");
}

void WarGrey::STEM::Tuxmon::on_jump(float theta_rad, float vx, float vy) {
    this->play("buttjump");
}

void WarGrey::STEM::Tuxmon::on_motion_stopped() {
    this->stop();
    this->switch_to_costume(this->get_initial_costume_index());
}

void WarGrey::STEM::Tuxmon::feed_flip_signs(float* hsgn, float* vsgn) {
    game_flip_to_signs(this->current_flip_status(), hsgn, vsgn);
}
