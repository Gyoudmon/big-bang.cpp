#include "animal.hpp"

#include <filesystem>

#include "../../../../datum/path.hpp"
#include "../../../../physics/random.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::Animal::Animal(const std::string& fullpath) : Sprite(fullpath) {}

WarGrey::STEM::Cat::Cat() : Animal(digimon_mascot_path("Cat", "", "trail/Animals")) {
    this->set_virtual_canvas(48.0F, 48.0F);
}

WarGrey::STEM::Cow::Cow() : Animal(digimon_mascot_path("Cow", "", "trail/Animals")) {
    this->set_virtual_canvas(96.0F, 96.0F);
}

WarGrey::STEM::Rooster::Rooster() : Animal(digimon_mascot_path("Rooster", "", "trail/Animals")) {
    this->set_virtual_canvas(42.0F, 42.0F);
}

WarGrey::STEM::Pigeon::Pigeon() : Animal(digimon_mascot_path("Pigeon", "", "trail/Animals")) {
    this->set_virtual_canvas(32.0F, 32.0F);
}

/*************************************************************************************************/
void WarGrey::STEM::Animal::on_costumes_load() {
    this->play("walk_s");
}

void WarGrey::STEM::Animal::on_heading_changed(double theta_rad, double vx, double vy, double prev_vr) {
    I8WayMotion::dispatch_heading_event(theta_rad, vx, vy, prev_vr);
}

void WarGrey::STEM::Animal::on_eward(double theta_rad, double vx, double vy) {
    this->play("walk_e_");
}

void WarGrey::STEM::Animal::on_wward(double theta_rad, double vx, double vy) {
    this->play("walk_w_");
}

void WarGrey::STEM::Animal::on_sward(double theta_rad, double vx, double vy) {
    this->play("walk_s_");
}

void WarGrey::STEM::Animal::on_nward(double theta_rad, double vx, double vy) {
    this->play("walk_n_");
}

void WarGrey::STEM::Animal::on_esward(double theta_rad, double vx, double vy) {
    this->play("walk_es_");
}

void WarGrey::STEM::Animal::on_enward(double theta_rad, double vx, double vy) {
    this->play("walk_en_");
}

void WarGrey::STEM::Animal::on_wsward(double theta_rad, double vx, double vy) {
    this->play("walk_ws_");
}

void WarGrey::STEM::Animal::on_wnward(double theta_rad, double vx, double vy) {
    this->play("walk_wn_");
}
