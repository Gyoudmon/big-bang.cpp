#include "bracer.hpp"

#include "../../datum/box.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::Bracer::Bracer(const char* name)
    : Citizen(digimon_mascot_path(name, "", "trail/Bracers")) {}

void WarGrey::STEM::Bracer::on_costumes_load() {
    this->switch_mode(BracerMode::Walk);
}

void WarGrey::STEM::Bracer::switch_mode(BracerMode mode, int repeat, MatterAnchor anchor) {
    if (this->mode != mode) {
        this->mode = mode;
        this->moor(anchor);
    }
    
    switch (this->mode) {
    case BracerMode::Walk: this->on_walk_mode(repeat); break;
    case BracerMode::Run: this->on_run_mode(repeat); break;
    case BracerMode::Win: this->on_win_mode(repeat); break;
    }
    
    this->clear_moor();
}

void WarGrey::STEM::Bracer::retrigger_heading_change_event() {
    double vx, vy, vr;

    this->get_velocity(&vr, &vx, &vy);
    this->dispatch_heading_event(vr, vx, vy, vr);
}

/*************************************************************************************************/
void WarGrey::STEM::Bracer::on_walk_mode(int repeat) {
    this->set_virtual_canvas(36.0F, 72.0F);
    this->retrigger_heading_change_event();
}

void WarGrey::STEM::Bracer::on_run_mode(int repeat) {
    this->set_virtual_canvas(48.0F, 72.0F);
    this->retrigger_heading_change_event();
}

void WarGrey::STEM::Bracer::on_win_mode(int repeat) {
    this->set_virtual_canvas(90.0F, 90.0F);
    this->play("win", repeat);
}

void WarGrey::STEM::Estelle::on_win_mode(int repeat) {
    this->set_virtual_canvas(96.0F, 96.0F);
    this->play("win", repeat);
}

void WarGrey::STEM::Tita::on_run_mode(int repeat) {
    this->set_virtual_canvas(50.0F, 72.0F);
    this->retrigger_heading_change_event();
}

/*************************************************************************************************/
void WarGrey::STEM::Bracer::on_heading_changed(double theta_rad, double vx, double vy, double prev_vr) {
    this->dispatch_heading_event(theta_rad, vx, vy, prev_vr);
}

void WarGrey::STEM::Bracer::on_eward(double theta_rad, double vx, double vy) {
    switch (this->mode) {
    case BracerMode::Run: this->play("run_e_"); break;
    default: this->play("walk_e_"); break;
    }
}

void WarGrey::STEM::Bracer::on_wward(double theta_rad, double vx, double vy) {
    switch (this->mode) {
    case BracerMode::Run: this->play("run_w_"); break;
    default: this->play("walk_w_"); break;
    }
}

void WarGrey::STEM::Bracer::on_sward(double theta_rad, double vx, double vy) {
    switch (this->mode) {
    case BracerMode::Run: this->play("run_s_"); break;
    default: this->play("walk_s_"); break;
    }
}

void WarGrey::STEM::Bracer::on_nward(double theta_rad, double vx, double vy) {
    switch (this->mode) {
    case BracerMode::Run: this->play("run_n_"); break;
    default: this->play("walk_n_"); break;
    }
}

void WarGrey::STEM::Bracer::on_esward(double theta_rad, double vx, double vy) {
    switch (this->mode) {
    case BracerMode::Run: this->play("run_es_"); break;
    default: this->play("walk_es_"); break;
    }
}

void WarGrey::STEM::Bracer::on_enward(double theta_rad, double vx, double vy) {
    switch (this->mode) {
    case BracerMode::Run: this->play("run_en_"); break;
    default: this->play("walk_en_"); break;
    }
}

void WarGrey::STEM::Bracer::on_wsward(double theta_rad, double vx, double vy) {
    switch (this->mode) {
    case BracerMode::Run: this->play("run_ws_"); break;
    default: this->play("walk_ws_"); break;
    }
}

void WarGrey::STEM::Bracer::on_wnward(double theta_rad, double vx, double vy) {
    switch (this->mode) {
    case BracerMode::Run: this->play("run_wn_"); break;
    default: this->play("walk_wn_"); break;
    }
}
