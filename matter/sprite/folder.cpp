#include "folder.hpp"

#include "../../datum/box.hpp"
#include "../../datum/path.hpp"
#include "../../datum/string.hpp"

#include "../../graphics/geometry.hpp"

#include <filesystem>

using namespace WarGrey::STEM;
using namespace std::filesystem;

/*************************************************************************************************/
WarGrey::STEM::Sprite::Sprite(const std::string& pathname) : _pathname(pathname) {
    this->enable_resize(true);
}

WarGrey::STEM::Sprite::Sprite(const char* pathname_fmt, ...) {
    VSNPRINT(pathname, pathname_fmt);

    this->_pathname = pathname;
    this->enable_resize(true);
}

void WarGrey::STEM::Sprite::pre_construct(SDL_Renderer* renderer) {
    path target = imgdb_absolute_path(this->_pathname);

    if (exists(target)) {
        if (is_directory(target)) {
            for (auto entry : directory_iterator(target)) {
                if (entry.is_regular_file()) {
                    this->load_costume(renderer, entry.path().string());
                } else if (entry.is_directory()) {
                    std::string decorate_name = file_basename_from_path(entry.path().string());

                    for (auto subentry : directory_iterator(entry)) {
                        if (subentry.is_regular_file()) {
                            this->load_decorate(renderer, decorate_name, subentry.path().string());
                        }
                    }
                }
            }
        } else {
            this->load_costume(renderer, this->_pathname);
        }
    }
}

void WarGrey::STEM::Sprite::feed_costume_extent(int idx, float* width, float* height) {
    this->costumes[idx].second->feed_extent(width, height);
}

void WarGrey::STEM::Sprite::draw_costume(SDL_Renderer* renderer, int idx, float x, float y, float Width, float Height) {
    SDL_RendererFlip flip = this->current_flip_status();
    SDL_FRect region = { x, y, Width, Height };

    game_render_texture(renderer, this->costumes[idx].second->texture(), &region, flip);

    if (this->current_decorate.size() > 0) {
        std::string c_name = this->costumes[idx].first;
        auto decorate = this->decorates[this->current_decorate];

        if (decorate.find(c_name) != decorate.end()) {
            game_render_texture(renderer, decorate[c_name]->texture(), &region, flip);
        }
    }
}

size_t WarGrey::STEM::Sprite::costume_count() {
    return this->costumes.size();
}

int WarGrey::STEM::Sprite::costume_name_to_index(const char* name) {
    int cidx = -1;

    for (int idx = 0; idx < this->costumes.size(); idx ++) {
        if (this->costumes[idx].first.compare(name) == 0) {
            cidx = idx;
            break;
        }
    }

    return cidx;
}

const char* WarGrey::STEM::Sprite::costume_index_to_name(int idx) {
    return this->costumes[idx].first.c_str();
}

void WarGrey::STEM::Sprite::wear(const std::string& name) {
    if (this->decorates.find(name) != this->decorates.end()) {
        this->current_decorate = name;
        this->notify_updated();
    }
}

void WarGrey::STEM::Sprite::take_off() {
    if (!this->current_decorate.empty()) {
        this->current_decorate.clear();
        this->notify_updated();
    }
}

void WarGrey::STEM::Sprite::load_costume(SDL_Renderer* renderer, const std::string& png) {
    shared_costume_t costume = imgdb_ref(png, renderer);

    if (costume->okay()) {
        std::string name = file_basename_from_path(png);
        auto datum = std::pair<std::string, shared_costume_t>(name, costume);
        
        for (auto it = this->costumes.begin(); ; it++) {
            if (it == this->costumes.end()) {
                this->costumes.push_back(datum);
                break;
            } else {
                if (name.compare((*it).first) < 0) {
                    this->costumes.insert(it, datum);
                    break;
                }
            }
        }     
    }
}

void WarGrey::STEM::Sprite::load_decorate(SDL_Renderer* renderer, const std::string& d_name, const std::string& png) {
    shared_costume_t deco_costume = imgdb_ref(png, renderer);

    if (deco_costume->okay()) {
        std::string c_name = file_basename_from_path(png);

        if (this->decorates.find(d_name) == this->decorates.end()) {
            this->decorates[d_name] = { { c_name, deco_costume } };
        } else {
            this->decorates[d_name][c_name] = deco_costume;
        }
    }
}
