#include "imgdb.hpp"

#include "../../graphics/image.hpp"

#include "../../datum/string.hpp"
#include "../../datum/path.hpp"
#include "../../datum/box.hpp"

#include <map>
#include <unordered_map>

using namespace WarGrey::STEM;

/*************************************************************************************************/
static shared_costume_t empty_costume = std::make_shared<Costume>(nullptr);
static std::map<std::string, std::unordered_map<SDL_Renderer*, shared_costume_t>> costumes;
static std::string imgdb_rootdir;

static inline std::string path_normalize(const std::string& path) {
    return string_prefix(path, imgdb_rootdir.c_str()) ? path : imgdb_rootdir + path;
}

static inline shared_costume_t imgdb_load(SDL_Renderer* renderer, const std::string& abspath) {
    return std::make_shared<Costume>(game_load_image_as_texture(renderer, abspath));
}

/*************************************************************************************************/
void WarGrey::STEM::imgdb_setup(const char* rootdir) {
    if (rootdir != nullptr) {
        imgdb_rootdir = directory_path(rootdir);
    }
}

void WarGrey::STEM::imgdb_teardown() {
    costumes.clear();
}

std::shared_ptr<Costume> WarGrey::STEM::imgdb_ref(const char* pathname, SDL_Renderer* renderer) {
    return imgdb_ref(std::string(pathname), renderer);
}

std::shared_ptr<Costume> WarGrey::STEM::imgdb_ref(const std::string& pathname, SDL_Renderer* renderer) {
    std::string abspath = path_normalize(pathname);
    shared_costume_t texture = empty_costume;

    if (string_suffix(abspath, ".png")) {
        if (costumes.find(abspath) != costumes.end()) {
            auto shared_costumes = costumes[abspath];

            if (shared_costumes.find(renderer) == shared_costumes.end()) {
                texture = imgdb_load(renderer, pathname);
                shared_costumes[renderer] = texture;
            } else {
                texture = shared_costumes[renderer];
            }
        } else {
            texture = imgdb_load(renderer, abspath);
            costumes[abspath] = { { renderer, texture } };
        }
    }

    return texture;
}

void WarGrey::STEM::imgdb_remove(const char* pathname) {
    imgdb_remove(std::string(pathname));
}

void WarGrey::STEM::imgdb_remove(const std::string& pathname) {
    std::string abspath = path_normalize(pathname);
    auto costume = costumes.find(abspath);

    if (costume != costumes.end()) {
        costumes.erase(costume);
    }
}

std::string WarGrey::STEM::imgdb_build_path(const std::string& subpath, const std::string& filename, const std::string& extension) {
    return directory_path(subpath) + filename + extension;
}

std::string WarGrey::STEM::imgdb_build_path(const char* subpath, const char* filename, const char* extension) {
    return directory_path(std::string(subpath)) + filename + extension;
}

std::string WarGrey::STEM::imgdb_absolute_path(const char* pathname) {
    return imgdb_absolute_path(std::string(pathname));
}

std::string WarGrey::STEM::imgdb_absolute_path(const std::string& pathname) {
    return path_normalize(pathname);
}

/*************************************************************************************************/
void WarGrey::STEM::Costume::feed_extent(int* width, int* height) {
    if (this->costume != nullptr) {
        SDL_QueryTexture(this->costume, nullptr, nullptr, width, height);
    }
}

void WarGrey::STEM::Costume::feed_extent(float* width, float* height) {
    int w, h;
    
    this->feed_extent(&w, &h);
    SET_BOX(width, float(w));
    SET_BOX(height, float(h));
}
