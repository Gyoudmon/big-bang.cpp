#include "folder.hpp"

#include "../../datum/box.hpp"
#include "../../datum/path.hpp"
#include "../../datum/string.hpp"

#include "../../graphics/geometry.hpp"

#include <filesystem>

using namespace WarGrey::STEM;
using namespace std::filesystem;

/*************************************************************************************************/
WarGrey::STEM::Sprite::Sprite(const char* pathname, MatterAnchor resize_anchor) : Sprite(std::string(pathname), resize_anchor) {}
WarGrey::STEM::Sprite::Sprite(const std::string& pathname, MatterAnchor resize_anchor) : _pathname(pathname) {
    this->enable_resize(true, resize_anchor);
}

WarGrey::STEM::Sprite::~Sprite() {
    for (auto custome : this->customes) {
        game_unload_image(custome.second);
    }
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

void WarGrey::STEM::Sprite::feed_custome_extent(int idx, float* width, float* height) {
    SDL_Texture* custome = this->customes[idx].second;
    int original_width, original_height;

    SDL_QueryTexture(custome, nullptr, nullptr, &original_width, &original_height);

    SET_BOX(width, float(original_width));
    SET_BOX(height, float(original_height));
}

void WarGrey::STEM::Sprite::draw_custome(SDL_Renderer* renderer, int idx, float x, float y, float Width, float Height) {
    game_draw_image(renderer, this->customes[idx].second, x, y, Width, Height, this->current_flip_status());
}

size_t WarGrey::STEM::Sprite::custome_count() {
    return this->customes.size();
}

int WarGrey::STEM::Sprite::custome_name_to_index(const char* name) {
    int cidx = -1;

    for (int idx = 0; idx < this->customes.size(); idx ++) {
        if (this->customes[idx].first.compare(name) == 0) {
            cidx = idx;
            break;
        }
    }

    return cidx;
}

bool WarGrey::STEM::Sprite::is_key_frame(int idx, const char* action) {
    return string_prefix(this->customes[idx].first, action);
}

void WarGrey::STEM::Sprite::load_custome(SDL_Renderer* renderer, std::string& png) {
    SDL_Texture* custome = game_load_image_as_texture(renderer, png);

    if (custome != nullptr) {
        this->push_custome(file_basename_from_path(png), custome);       
    }
}

void WarGrey::STEM::Sprite::push_custome(const std::string& name, SDL_Texture* custome) {
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
