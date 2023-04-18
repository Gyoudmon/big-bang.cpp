#include "planetcute.hpp"

#include "../../../../datum/box.hpp"
#include "../../../../datum/path.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
#define GROUND_PATH digimon_mascot_path("atlas/planetcute", "")
#define GROUND_ATLAS_PATH digimon_mascot_path("atlas/planetcute", ".png")

static const float planet_cute_tile_top_margin = 32.0F;
static const float planet_cute_tile_thickness = 25.0F;

/*************************************************************************************************/
WarGrey::STEM::PlanetCuteTile::PlanetCuteTile(GroundBlockType default_type, int row, int col)
    : GridAtlas(GROUND_ATLAS_PATH, 1, 8), type(default_type) {
        this->map_row = row;
        this->map_col = col;
}

void WarGrey::STEM::PlanetCuteTile::feed_original_margin(float x, float y, float* top, float* right, float* bottom, float* left) {
    SET_BOXES(left, right, 0.0F);
    SET_VALUES(top, planet_cute_tile_top_margin, bottom, 0.0F);
}

void WarGrey::STEM::PlanetCuteTile::feed_original_map_overlay(float* top, float* right, float* bottom, float* left) {
    SET_BOXES(left, right, 0.5F);
    SET_VALUES(top, planet_cute_tile_top_margin, bottom, planet_cute_tile_thickness);
}

void WarGrey::STEM::PlanetCuteTile::set_type(GroundBlockType type) {
    if (this->type != type) {
        this->type = type;
        this->notify_updated();
    }
}

int WarGrey::STEM::PlanetCuteTile::get_atlas_tile_index(size_t map_idx, int& xoff, int& yoff) {
    return static_cast<int>(this->type);
}
