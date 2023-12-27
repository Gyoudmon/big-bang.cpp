#include "animal.hpp"

#include <filesystem>

#include "../../../../datum/path.hpp"
#include "../../../../physics/random.hpp"

using namespace GYDM;

/*************************************************************************************************/
GYDM::Animal::Animal(const std::string& fullpath) : Sprite(fullpath) {}

GYDM::Cat::Cat() : Animal(digimon_mascot_path("Cat", "", "trail/Animals")) {
    this->set_virtual_canvas(48.0F, 48.0F);
}

GYDM::Cow::Cow() : Animal(digimon_mascot_path("Cow", "", "trail/Animals")) {
    this->set_virtual_canvas(96.0F, 96.0F);
}

GYDM::Rooster::Rooster() : Animal(digimon_mascot_path("Rooster", "", "trail/Animals")) {
    this->set_virtual_canvas(42.0F, 42.0F);
}

GYDM::Pigeon::Pigeon() : Animal(digimon_mascot_path("Pigeon", "", "trail/Animals")) {
    this->set_virtual_canvas(32.0F, 32.0F);
}

/*************************************************************************************************/
void GYDM::Animal::on_costumes_load() {
    this->play("walk_s");
}

void GYDM::Animal::on_heading_changed(double theta_rad, double vx, double vy, double prev_vr) {
    I8WayMotion::dispatch_heading_event(theta_rad, vx, vy, prev_vr);
}

void GYDM::Animal::on_eward(double theta_rad, double vx, double vy) {
    this->play("walk_e_");
}

void GYDM::Animal::on_wward(double theta_rad, double vx, double vy) {
    this->play("walk_w_");
}

void GYDM::Animal::on_sward(double theta_rad, double vx, double vy) {
    this->play("walk_s_");
}

void GYDM::Animal::on_nward(double theta_rad, double vx, double vy) {
    this->play("walk_n_");
}

void GYDM::Animal::on_esward(double theta_rad, double vx, double vy) {
    this->play("walk_es_");
}

void GYDM::Animal::on_enward(double theta_rad, double vx, double vy) {
    this->play("walk_en_");
}

void GYDM::Animal::on_wsward(double theta_rad, double vx, double vy) {
    this->play("walk_ws_");
}

void GYDM::Animal::on_wnward(double theta_rad, double vx, double vy) {
    this->play("walk_wn_");
}
