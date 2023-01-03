#include "sprite.hpp"

#include "../datum/box.hpp"
#include "../datum/path.hpp"

#include <filesystem>

using namespace WarGrey::STEM;
using namespace std::filesystem;

/*************************************************************************************************/
WarGrey::STEM::ISprite::ISprite() {
    this->_sprite = this;
    this->current_custome_idx = 0;
}

WarGrey::STEM::ISprite::~ISprite() {
    for (auto custome : this->customes) {
        game_unload_image(custome);
    }
}

void WarGrey::STEM::ISprite::feed_extent(float x, float y, float* width, float* height) {
    if (this->current_custome_idx >= this->customes.size()) {
        SET_BOXES(width, height, 0.0F);
    } else {
        SDL_Surface* custome = this->customes[this->current_custome_idx];

        SET_BOX(width, float(custome->w) * this->xscale);
        SET_BOX(height, float(custome->h) * this->yscale);
    }
}

void WarGrey::STEM::ISprite::on_resize(float width, float height, float old_width, float old_height) {
    if (this->current_custome_idx < this->customes.size()) {
        SDL_Surface* custome = this->customes[this->current_custome_idx];
        
        this->xscale = width / float(custome->w);
        this->yscale = height / float(custome->h);
    }
}

void WarGrey::STEM::ISprite::draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) {
    if (this->current_custome_idx < this->customes.size()) {
        game_draw_image(renderer, this->customes[this->current_custome_idx], x, y, Width, Height);
    }
}

size_t WarGrey::STEM::ISprite::custome_count() {
    return this->customes.size();
}

void WarGrey::STEM::ISprite::switch_to_custome(int idx) {
    if (this->customes.size() > 0) {
        int actual_idx = idx % this->customes.size();

        if (actual_idx != this->current_custome_idx) {
            this->current_custome_idx = actual_idx;
            this->notify_updated();
        }
    }
}

void WarGrey::STEM::ISprite::switch_to_custome(const char* name) {
    this->switch_to_custome(std::string(name));
}            

void WarGrey::STEM::ISprite::switch_to_custome(const std::string& custome_name) {
    for (int idx = 0; idx < this->names.size(); idx ++) {
        if (this->names[idx] == custome_name) {
            if (idx != this->current_custome_idx) {
                this->current_custome_idx = idx;
                this->notify_updated();
            }

            break;
        }
    }    
}

void WarGrey::STEM::ISprite::push_custome(const char* name, SDL_Surface* custome) {
    this->push_custome(std::string(name), custome);
}

void WarGrey::STEM::ISprite::push_custome(const std::string& name, SDL_Surface* custome) {
    if (custome != nullptr) {
        this->names.push_back(name);
        this->customes.push_back(custome);
    }
}

/*************************************************************************************************/
void WarGrey::STEM::Sprite::pre_construct() {
    path target(this->_pathname);

    if (exists(target)) {
        if (is_directory(target)) {
            for (auto entry : directory_iterator(target)) {
                if (entry.is_regular_file()) {
                    std::string pathname = entry.path().string();

                    if (file_extension_from_path(pathname) == ".png") { 
                        this->load_custome(pathname);
                    } 
                }
            }
        } else {
            this->load_custome(this->_pathname);
        }
    }
}

void WarGrey::STEM::Sprite::load_custome(std::string& png) {
    SDL_Surface* custome = game_load_image(png);

    if (custome != nullptr) {
        this->push_custome(file_basename_from_path(png), custome);        
    }
}
