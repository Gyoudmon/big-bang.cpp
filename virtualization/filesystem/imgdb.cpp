#include "imgdb.hpp"

#include "../../graphics/image.hpp"

#include "../../datum/string.hpp"
#include "../../datum/path.hpp"
#include "../../datum/box.hpp"

#include <map>
#include <unordered_map>
#include <filesystem>

using namespace GYDM;
using namespace std::filesystem;

/*************************************************************************************************/
static shared_texture_t empty_costume = std::make_shared<Texture>(nullptr);
static std::map<std::string, std::unordered_map<SDL_Renderer*, shared_texture_t>> costumes;
static std::string imgdb_rootdir;

static inline std::string path_normalize(const std::string& str_path) {
    return path(str_path).is_absolute() ? str_path : imgdb_rootdir + str_path;
}

static inline shared_texture_t imgdb_load(SDL_Renderer* renderer, const std::string& abspath) {
    return std::make_shared<Texture>(game_load_image(renderer, abspath));
}

/*************************************************************************************************/
void GYDM::imgdb_setup(const char* rootdir) {
    if (rootdir != nullptr) {
        imgdb_rootdir = directory_path(rootdir);
    }
}

void GYDM::imgdb_setup(const std::string& rootdir) {
    imgdb_setup(rootdir.c_str());
}

void GYDM::imgdb_teardown() {
    costumes.clear();
}

shared_texture_t GYDM::imgdb_ref(const char* pathname, SDL_Renderer* renderer) {
    return imgdb_ref(std::string(pathname), renderer);
}

shared_texture_t GYDM::imgdb_ref(const std::string& pathname, SDL_Renderer* renderer) {
    std::string abspath = path_normalize(pathname);
    shared_texture_t texture = empty_costume;

    if (string_suffix(abspath, ".png") || string_suffix(abspath, ".svg")) {
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

void GYDM::imgdb_remove(const char* pathname) {
    imgdb_remove(std::string(pathname));
}

void GYDM::imgdb_remove(const std::string& pathname) {
    std::string abspath = path_normalize(pathname);
    auto costume = costumes.find(abspath);

    if (costume != costumes.end()) {
        costumes.erase(costume);
    }
}

std::string GYDM::imgdb_build_path(const std::string& subpath, const std::string& filename, const std::string& extension) {
    return directory_path(subpath) + filename + extension;
}

std::string GYDM::imgdb_build_path(const char* subpath, const char* filename, const char* extension) {
    return directory_path(std::string(subpath)) + filename + extension;
}

std::string GYDM::imgdb_absolute_path(const char* pathname) {
    return imgdb_absolute_path(std::string(pathname));
}

std::string GYDM::imgdb_absolute_path(const std::string& pathname) {
    return path_normalize(pathname);
}
