#include "planetcute.hpp"

#include "../../../../datum/box.hpp"
#include "../../../../datum/path.hpp"

using namespace WarGrey::STEM;

#define GROUND_PATH digimon_mascot_path("atlas/planetcute", "")

/*************************************************************************************************/
WarGrey::STEM::PlanetCuteTile::PlanetCuteTile(GroundBlockType default_type) : Sprite(GROUND_PATH), type(default_type) {
    this->camouflage(true);
}

int WarGrey::STEM::PlanetCuteTile::get_initial_costume_index() {
    return this->costume_name_to_index(this->type_to_name(this->type));
}

float WarGrey::STEM::PlanetCuteTile::get_thickness() {
    return 25.0F * this->get_vertical_scale();
}

void WarGrey::STEM::PlanetCuteTile::feed_original_margin(float x, float y, float* top, float* right, float* bottom, float* left) {
    SET_BOXES(left, right, 0.0F);
    SET_VALUES(top, 32.0F, bottom, 0.0F);
}

void WarGrey::STEM::PlanetCuteTile::set_type(GroundBlockType type) {
    if (this->type != type) {
        this->type = type;
        this->switch_to_costume(this->type_to_name(this->type));
    }
}

const char* WarGrey::STEM::PlanetCuteTile::type_to_name(GroundBlockType type) {
    switch (type) {
    case GroundBlockType::Grass: return "grass"; break;
    case GroundBlockType::Soil: return "soil"; break;
    case GroundBlockType::Dirt: return "dirt"; break;
    case GroundBlockType::Stone: return "stone"; break;
    case GroundBlockType::Water: return "water"; break;
    case GroundBlockType::Wood: return "wood"; break;
    default: return "";
    }
}
