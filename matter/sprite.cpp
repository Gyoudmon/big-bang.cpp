#include "sprite.hpp"

#include "../datum/box.hpp"
#include "../datum/path.hpp"
#include "../datum/string.hpp"
#include "../datum/flonum.hpp"

#include "../graphics/geometry.hpp"

#include <filesystem>

using namespace WarGrey::STEM;
using namespace std::filesystem;

/*************************************************************************************************/
WarGrey::STEM::ISprite::ISprite() {
    this->_sprite = this;
    this->current_custome_idx = 0;
    this->set_fps();
}

WarGrey::STEM::ISprite::~ISprite() {
    for (auto custome : this->customes) {
        game_unload_image(custome.second);
    }
}

void WarGrey::STEM::ISprite::feed_extent(float x, float y, float* width, float* height) {
    if (this->current_custome_idx >= this->customes.size()) {
        SET_BOXES(width, height, 0.0F);
    } else {
        SDL_Texture* custome = this->customes[this->current_custome_idx].second;
        int tw, th;

        SDL_QueryTexture(custome, nullptr, nullptr, &tw, &th);

        SET_BOX(width, float(tw) * flabs(this->xscale));
        SET_BOX(height, float(th) * flabs(this->yscale));
    }
}

void WarGrey::STEM::ISprite::on_resize(float width, float height, float old_width, float old_height) {
    this->xscale = width / old_width;
    this->yscale = height / old_height;
}

void WarGrey::STEM::ISprite::draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) {
    if (this->current_custome_idx < this->customes.size()) {
        game_draw_image(renderer, this->customes[this->current_custome_idx].second,
                            x, y, Width, Height,
                            game_scales_to_flip(this->xscale, this->yscale));
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
    for (int idx = 0; idx < this->customes.size(); idx ++) {
        if (this->customes[idx].first == custome_name) {
            if (idx != this->current_custome_idx) {
                this->current_custome_idx = idx;
                this->notify_updated();
            }

            break;
        }
    }    
}

void WarGrey::STEM::ISprite::update(uint32_t count, uint32_t interval, uint32_t uptime) {
    size_t frame_size = this->frame_refs.size();

    if (frame_size > 1) {
        if (interval * this->animation_subframe_count <= this->animation_interval) {
            this->animation_subframe_count ++;
        } else {
            this->animation_subframe_count = 1;

            if (this->current_subframe_idx >= frame_size) {
                if (this->animation_rest != 0) {
                    this->switch_to_custome(this->frame_refs[0]);
                    this->current_subframe_idx = 1;
                    if (this->animation_rest > 0) {
                        this->animation_rest --;
                    }
                }
            } else {
                this->switch_to_custome(this->frame_refs[this->current_subframe_idx]);
                this->current_subframe_idx ++;
            }
        }
    }
}

void WarGrey::STEM::ISprite::set_fps(int fps) {
    this->animation_interval = 1000 / fps;
    this->animation_subframe_count = 1;
}

size_t WarGrey::STEM::ISprite::play(const char* action, int repetition) {
    this->animation_rest = repetition;
    this->animation_subframe_count = 1;
    this->frame_refs.clear();
    this->current_subframe_idx = 0;
    
    for (int i = 0; i < this->customes.size(); i++) {
        if (string_prefix(this->customes[i].first, action)) {
            this->frame_refs.push_back(i);
        }
    }

    if (this->frame_refs.size()) {
        this->switch_to_custome(this->frame_refs[0]);
        this->current_subframe_idx = 1;
    }

    return this->frame_refs.size();
}

void WarGrey::STEM::ISprite::push_custome(const char* name, SDL_Texture* custome) {
    this->push_custome(std::string(name), custome);
}

void WarGrey::STEM::ISprite::push_custome(const std::string& name, SDL_Texture* custome) {
    if (custome != nullptr) {
        auto datum = std::pair<std::string, SDL_Texture*>(name, custome);
        
        for (auto it = this->customes.begin(); ; it++) {
            if (it == this->customes.end()) {
                this->customes.push_back(datum);
                break;
            } else {
                if (name.compare((*it).first) < 0) {
                    this->customes.insert(it, datum);
                    break;
                }
            }
        }
    }
}

void WarGrey::STEM::ISprite::stop() {
    this->animation_rest = 0;
    this->frame_refs.clear();
}

/*************************************************************************************************/
WarGrey::STEM::Sprite::Sprite(const char* pathname, MatterAnchor resize_anchor) : Sprite(std::string(pathname), resize_anchor) {}
WarGrey::STEM::Sprite::Sprite(const std::string& pathname, MatterAnchor resize_anchor) : _pathname(pathname) {
    this->enable_resize(true, resize_anchor);
}

void WarGrey::STEM::Sprite::pre_construct(SDL_Renderer* renderer) {
    path target(this->_pathname);

    if (exists(target)) {
        if (is_directory(target)) {
            for (auto entry : directory_iterator(target)) {
                if (entry.is_regular_file()) {
                    std::string pathname = entry.path().string();

                    if (file_extension_from_path(pathname) == ".png") { 
                        this->load_custome(renderer, pathname);
                    } 
                }
            }
        } else {
            this->load_custome(renderer, this->_pathname);
        }
    }
}

void WarGrey::STEM::Sprite::load_custome(SDL_Renderer* renderer, std::string& png) {
    SDL_Texture* custome = game_load_image_as_texture(renderer, png);

    if (custome != nullptr) {
        this->push_custome(file_basename_from_path(png), custome);       
    }
}
