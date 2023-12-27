#pragma once

#include "../../graphics/texture.hpp"

#include <string>

namespace GYDM {
    __lambda__ void imgdb_setup(const char* rootdir);
    __lambda__ void imgdb_setup(const std::string& rootdir);
    __lambda__ void imgdb_teardown();

    __lambda__ shared_texture_t imgdb_ref(const char* subpath, SDL_Renderer* rendener);
    __lambda__ shared_texture_t imgdb_ref(const std::string& subpath, SDL_Renderer* rendener);

    __lambda__ void imgdb_remove(const char* subpath);
    __lambda__ void imgdb_remove(const std::string& subpath);

    __lambda__ std::string imgdb_build_path(const std::string& subpath, const std::string& filename, const std::string& extension);
    __lambda__ std::string imgdb_build_path(const char* subpath, const char* filename, const char* extension);

    __lambda__ std::string imgdb_absolute_path(const char* subpath);
    __lambda__ std::string imgdb_absolute_path(const std::string& subpath);
}
