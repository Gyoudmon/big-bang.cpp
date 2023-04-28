#pragma once

#include "../../graphics/texture.hpp"

#include <string>

namespace WarGrey::STEM {
    void imgdb_setup(const char* rootdir);
    void imgdb_setup(const std::string& rootdir);
    void imgdb_teardown();

    shared_texture_t imgdb_ref(const char* subpath, SDL_Renderer* rendener);
    shared_texture_t imgdb_ref(const std::string& subpath, SDL_Renderer* rendener);

    void imgdb_remove(const char* subpath);
    void imgdb_remove(const std::string& subpath);

    std::string imgdb_build_path(const std::string& subpath, const std::string& filename, const std::string& extension);
    std::string imgdb_build_path(const char* subpath, const char* filename, const char* extension);

    std::string imgdb_absolute_path(const char* subpath);
    std::string imgdb_absolute_path(const std::string& subpath);
}
