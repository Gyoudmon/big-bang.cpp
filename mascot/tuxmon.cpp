#include "tuxmon.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::Tuxmon::Tuxmon() : Sprite(digimon_mascot_path("tuxmon", "")) {}

int WarGrey::STEM::Tuxmon::get_initial_costume_index() {
    return this->costume_name_to_index("idle-0");
}
