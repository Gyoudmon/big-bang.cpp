#include "display.hpp"

#include <SDL2/SDL_image.h>

#include <filesystem>

using namespace WarGrey::STEM;

/*************************************************************************************************/
bool WarGrey::STEM::IDisplay::save_snapshot(const std::string& path) {
    return this->save_snapshot(path.c_str());
}

bool WarGrey::STEM::IDisplay::save_snapshot(const char* pname) {
    SDL_Surface* snapshot = this->snapshot();
    bool okay = false;

    if (snapshot != NULL) {
        create_directories(std::filesystem::path(pname).parent_path());
        if (IMG_SavePNG(snapshot, pname) == 0) {
            okay = true;
        }
    }

    return okay;
}

