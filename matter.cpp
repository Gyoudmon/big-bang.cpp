#include "matter.hpp"
#include "plane.hpp"

#include "datum/string.hpp"
#include "datum/box.hpp"

#include "graphics/image.hpp"

using namespace WarGrey::STEM;

/**************************************************************************************************/
WarGrey::STEM::IMatter::~IMatter() {
    if (this->info != nullptr) {
        delete this->info;
        this->info = nullptr;
    }
}

IPlane* WarGrey::STEM::IMatter::master() {
    IPlane* plane = nullptr;

    if (this->info != nullptr) {
        plane = this->info->master;
    }

    return plane;
}

void WarGrey::STEM::IMatter::feed_extent(float x, float y, float* w, float* h) {
    SET_VALUES(w, 0.0F, h, 0.0F);
}

void WarGrey::STEM::IMatter::feed_margin(float x, float y, float* top, float* right, float* bottom, float* left) {
    SET_VALUES(top, 0.0F, bottom, 0.0F);
    SET_VALUES(left, 0.0F, right, 0.0F);
}

void WarGrey::STEM::IMatter::notify_updated() {
    if (this->info != nullptr) {
        if (this->anchor != MatterAnchor::LT) {
            this->info->master->move_to(this, this->anchor_x, this->anchor_y, this->anchor);
            this->clear_moor();
        }

        this->info->master->notify_updated();
    }
}

void WarGrey::STEM::IMatter::moor(MatterAnchor anchor) {
    if (anchor != MatterAnchor::LT) {
        if (this->info != nullptr) {
            this->anchor = anchor;
            this->info->master->feed_matter_location(this, &this->anchor_x, &this->anchor_y, anchor);
        }
    }
}

void WarGrey::STEM::IMatter::clear_moor() {
    this->anchor = MatterAnchor::LT;
}

bool WarGrey::STEM::IMatter::has_caret() {
    bool careted = false;

    if (this->info != nullptr) {
        careted = (this->info->master->get_focused_matter() == this);
    }

    return careted;
}

void WarGrey::STEM::IMatter::feed_location(float* x, float* y, WarGrey::STEM::MatterAnchor a) {
    if (this->info != nullptr) {
        this->info->master->feed_matter_location(this, x, y, a);
    }
}

void WarGrey::STEM::IMatter::log_message(const char* fmt, ...) {
    if (this->info != nullptr) {
        VSNPRINT(text, fmt);
        this->log_message(-1, text);
    }
}

void WarGrey::STEM::IMatter::log_message(int fgc, const char* fmt, ...) {
    if (this->info != nullptr) {
        VSNPRINT(text, fmt);
        this->log_message(fgc, text);
    }
}

void WarGrey::STEM::IMatter::log_message(int fgc, const std::string& msg) {
    if (this->info != nullptr) {
        this->info->master->log_message(fgc, msg);
    }
}

/**************************************************************************************************/
SDL_Surface* WarGrey::STEM::IMatter::snapshot() {
    SDL_Surface* photograph = nullptr;
    float width, height;

    this->feed_extent(0.0F, 0.0F, &width, &height);
    photograph = game_blank_image(width, height);

    if (photograph != nullptr) {
        SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(photograph);
        
        if (renderer != nullptr) {
            this->draw(renderer, 0, 0, width, height);
            SDL_RenderPresent(renderer);
            SDL_DestroyRenderer(renderer);
        }
    }

    return photograph;
}

bool WarGrey::STEM::IMatter::save_snapshot(const std::string& pname) {
    return this->save_snapshot(pname.c_str());
}

bool WarGrey::STEM::IMatter::save_snapshot(const char* pname) {
    SDL_Surface* photograph = this->snapshot();
    bool okay = game_save_image(photograph, pname);

    SDL_FreeSurface(photograph);

    return okay;
}

