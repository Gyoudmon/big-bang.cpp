#include "display.hpp"
#include "image.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
bool WarGrey::STEM::IDisplay::save_snapshot(const std::string& path) {
    return this->save_snapshot(path.c_str());
}

bool WarGrey::STEM::IDisplay::save_snapshot(const char* pname) {
    return game_save_image(this->snapshot(), pname);
}

