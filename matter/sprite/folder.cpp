#include "folder.hpp"

#include "../../datum/box.hpp"
#include "../../datum/path.hpp"
#include "../../datum/string.hpp"
#include "../../datum/flonum.hpp"

#include <filesystem>

using namespace GYDM;
using namespace std::filesystem;

/*************************************************************************************************/
GYDM::Sprite::Sprite(const char* pathname_fmt, ...) {
    VSNPRINT(pathname, pathname_fmt);

    this->_pathname = pathname;
    this->enable_resize(true);
}

GYDM::Sprite::Sprite(const std::string& pathname) : _pathname(pathname) {
    this->enable_resize(true);
}

const char* GYDM::Sprite::name() {
    static std::string _name;

    _name = file_name_from_path(_pathname);
    
    return _name.c_str();
}

void GYDM::Sprite::construct(GYDM::dc_t* dc) {
    path target = imgdb_absolute_path(this->_pathname);
    
    if (exists(target)) {
        if (is_directory(target)) {
            for (auto entry : directory_iterator(target)) {
                if (entry.is_regular_file()) {
                    this->load_costume(dc, entry.path().string());
                } else if (entry.is_directory()) {
                    std::string decorate_name = file_basename_from_path(entry.path().string());

                    for (auto subentry : directory_iterator(entry)) {
                        if (subentry.is_regular_file()) {
                            this->load_decorate(dc, decorate_name, subentry.path().string());
                        }
                    }
                }
            }
        } else {
            this->load_costume(dc, this->_pathname);
        }

        this->on_costumes_load();
        ISprite::construct(dc);
    }
}

void GYDM::Sprite::feed_costume_extent(size_t idx, float* width, float* height) {
    this->costumes[idx].second->feed_extent(width, height);
}

void GYDM::Sprite::draw_costume(GYDM::dc_t* dc, size_t idx, SDL_Rect* src, SpriteRenderArguments* argv) {
    dc->stamp(this->costumes[idx].second->self(), src, &argv->dst, argv->flip);

    if (this->current_decorate.size() > 0) {
        std::string c_name = this->costumes[idx].first;
        auto decorate = this->decorates[this->current_decorate];

        if (decorate.find(c_name) != decorate.end()) {
            dc->stamp(decorate[c_name]->self(), src, &argv->dst, argv->flip);
        }
    }
}

size_t GYDM::Sprite::costume_count() {
    return this->costumes.size();
}

const char* GYDM::Sprite::costume_index_to_name(size_t idx) {
    return this->costumes[idx].first.c_str();
}

void GYDM::Sprite::wear(const std::string& name) {
    if (this->decorates.find(name) != this->decorates.end()) {
        this->current_decorate = name;
        this->notify_updated();
    }
}

void GYDM::Sprite::take_off() {
    if (!this->current_decorate.empty()) {
        this->current_decorate.clear();
        this->notify_updated();
    }
}

void GYDM::Sprite::load_costume(GYDM::dc_t* dc, const std::string& png) {
    std::string name = file_basename_from_path(png);
    
    if (!name.empty()) { // ignore dot files
        shared_texture_t costume = imgdb_ref(png, dc->self());

        if (costume->okay()) {
            auto datum = std::pair<std::string, shared_texture_t>(name, costume);
        
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
}

void GYDM::Sprite::load_decorate(GYDM::dc_t* dc, const std::string& d_name, const std::string& png) {
    std::string c_name = file_basename_from_path(png);

    if (!c_name.empty()) {
        shared_texture_t deco_costume = imgdb_ref(png, dc->self());

        if (deco_costume->okay()) {
        
            if (this->decorates.find(d_name) == this->decorates.end()) {
                this->decorates[d_name] = { { c_name, deco_costume } };
            } else {
                this->decorates[d_name][c_name] = deco_costume;
            }
        }
    }
}
