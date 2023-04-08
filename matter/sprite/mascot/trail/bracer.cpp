#include "bracer.hpp"

#include "../../../../datum/box.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::Bracer::Bracer(const char* name)
    : Citizen(digimon_mascot_path(name, "", "trail/Bracers")) {}

void WarGrey::STEM::Bracer::on_costumes_load() {
    Citizen::on_costumes_load();
    this->switch_mode(BracerMode::Walk);
}

void WarGrey::STEM::Bracer::switch_mode(BracerMode mode, int repeat, MatterAnchor anchor) {
    if (this->mode != mode) {
        float cwidth, cheight;

        this->mode = mode;

        this->moor(anchor);
        this->feed_canvas_size(mode, &cwidth, &cheight);
        this->set_virtual_canvas(cwidth, cheight);
        this->clear_moor();
    }
    
    // don't move this into the IF-block above
    switch (this->mode) {
    case BracerMode::Walk: this->on_walk_mode(repeat); break;
    case BracerMode::Run: this->on_run_mode(repeat); break;
    case BracerMode::Win: this->on_win_mode(repeat); break;
    }
}

void WarGrey::STEM::Bracer::retrigger_heading_change_event() {
    double vx, vy, vr;

    this->get_velocity(&vr, &vx, &vy);
    this->dispatch_heading_event(vr, vx, vy, vr);
}

/*************************************************************************************************/
void WarGrey::STEM::Bracer::on_walk_mode(int repeat) {
    this->retrigger_heading_change_event();
}

void WarGrey::STEM::Bracer::on_run_mode(int repeat) {
    this->retrigger_heading_change_event();
}

void WarGrey::STEM::Bracer::on_win_mode(int repeat) {
    this->play("win", repeat);
}

void WarGrey::STEM::Bracer::feed_canvas_size(BracerMode mode, float* width, float* height) {
    switch (mode) {
    case BracerMode::Walk: SET_VALUES(width, 36.0F, height, 72.0F); break;
    case BracerMode::Run:  SET_VALUES(width, 48.0F, height, 72.0F); break;
    case BracerMode::Win:  SET_VALUES(width, 90.0F, height, 90.0F); break;
    }
}

/*************************************************************************************************/
void WarGrey::STEM::Estelle::feed_canvas_size(BracerMode mode, float* width, float* height) {
    if (mode == BracerMode::Win) {
        SET_VALUES(width, 96.0F, height, 96.0F);
    } else {
        Bracer::feed_canvas_size(mode, width, height);
    }
}

void WarGrey::STEM::Tita::feed_canvas_size(BracerMode mode, float* width, float* height) {
    switch (mode) {
    case BracerMode::Walk: SET_VALUES(width, 48.0F, height, 72.0F); break;
    case BracerMode::Run:  SET_VALUES(width, 50.0F, height, 72.0F); break;
    default: Bracer::feed_canvas_size(mode, width, height);
    }
}

void WarGrey::STEM::Zin::feed_canvas_size(BracerMode mode, float* width, float* height) {
    switch (mode) {
    case BracerMode::Walk: case BracerMode::Run: SET_VALUES(width, 64.0F, height, 96.0F); break;
    default: Bracer::feed_canvas_size(mode, width, height);
    }
}

/*************************************************************************************************/
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
