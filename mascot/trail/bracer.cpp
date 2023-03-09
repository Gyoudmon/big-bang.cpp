#include "bracer.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::Bracer::Bracer(const char* name)
    : Citizen(digimon_mascot_path(name, "", "digitama/big_bang/mascot/trail/Bracers")) {}

void WarGrey::STEM::Bracer::construct(SDL_Renderer* renderer) {
    Sprite::construct(renderer);
    this->auto_virtual_canvas("walk");
}
