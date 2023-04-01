#include "citizen.hpp"

#include <filesystem>

#include "../../../../datum/path.hpp"
#include "../../../../physics/random.hpp"

using namespace WarGrey::STEM;
using namespace std::filesystem;

/*************************************************************************************************/
static std::vector<std::string> list_citizen_names(const char* subdir) {
    std::string rootdir = digimon_mascot_subdir(subdir);
    std::vector<std::string> names;

    for (auto entry : directory_iterator(rootdir)) {
        path self = entry.path();

        if (entry.is_directory()) {
            names.push_back(self.filename().string());
        }
    }

    return names;
}

/*************************************************************************************************/
std::vector<std::string> WarGrey::STEM::Citizen::list_special_names() {
    return list_citizen_names(TRIAL_SPECIALS_PATH);
}

std::vector<std::string> WarGrey::STEM::TrailKid::list_names() {
    return list_citizen_names(TRIAL_KIDS_PATH);
}

std::vector<std::string> WarGrey::STEM::TrailStudent::list_names() {
    return list_citizen_names(TRIAL_STUDENTS_PATH);
}

TrailKid* WarGrey::STEM::TrailKid::randomly_create() {
    std::vector<std::string> names = TrailKid::list_names();
    TrailKid* kid = nullptr;

    if (!names.empty()) {
        kid = new TrailKid(names[random_uniform(0, int(names.size()) - 1)]);
    }

    return kid;
}

TrailStudent* WarGrey::STEM::TrailStudent::randomly_create() {
    std::vector<std::string> names = TrailStudent::list_names();
    TrailStudent* student = nullptr;

    if (!names.empty()) {
        student = new TrailStudent(names[random_uniform(0, int(names.size()) - 1)]);
    }

    return student;
}

/*************************************************************************************************/
WarGrey::STEM::Citizen::Citizen(const std::string& fullpath) : Sprite(fullpath) {
    this->set_virtual_canvas(36.0F, 72.0F);
}

WarGrey::STEM::TrailKid::TrailKid(const char* name) : Citizen(digimon_mascot_path(name, "", TRIAL_KIDS_PATH)) {
    this->set_virtual_canvas(32.0F, 56.0F);
}

WarGrey::STEM::TrailStudent::TrailStudent(const char* name) : Citizen(digimon_mascot_path(name, "", TRIAL_STUDENTS_PATH)) {
    this->set_virtual_canvas(32.0F, 68.0F);
}

/*************************************************************************************************/
void WarGrey::STEM::Citizen::construct(SDL_Renderer* renderer) {
    Sprite::construct(renderer);
    this->play("walk_s");
}

void WarGrey::STEM::Citizen::on_heading_changed(double theta_rad, double vx, double vy, double prev_vr) {
    I8WayMotion::dispatch_heading_event(theta_rad, vx, vy, prev_vr);
}

void WarGrey::STEM::Citizen::on_eward(double theta_rad, double vx, double vy) {
    this->play("walk_e_");
}

void WarGrey::STEM::Citizen::on_wward(double theta_rad, double vx, double vy) {
    this->play("walk_w_");
}

void WarGrey::STEM::Citizen::on_sward(double theta_rad, double vx, double vy) {
    this->play("walk_s_");
}

void WarGrey::STEM::Citizen::on_nward(double theta_rad, double vx, double vy) {
    this->play("walk_n_");
}

void WarGrey::STEM::Citizen::on_esward(double theta_rad, double vx, double vy) {
    this->play("walk_es_");
}

void WarGrey::STEM::Citizen::on_enward(double theta_rad, double vx, double vy) {
    this->play("walk_en_");
}

void WarGrey::STEM::Citizen::on_wsward(double theta_rad, double vx, double vy) {
    this->play("walk_ws_");
}

void WarGrey::STEM::Citizen::on_wnward(double theta_rad, double vx, double vy) {
    this->play("walk_wn_");
}
