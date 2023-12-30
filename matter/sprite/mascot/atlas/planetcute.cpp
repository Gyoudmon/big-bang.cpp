#include "planetcute.hpp"

#include "../../../../datum/box.hpp"
#include "../../../../datum/path.hpp"
#include "../../../../datum/fixnum.hpp"

using namespace GYDM;

/*************************************************************************************************/
#define GROUND_ATLAS_PATH digimon_mascot_path("atlas/planetcute", ".png")

static const float planet_cute_tile_top_margin = 32.0F;
static const float planet_cute_tile_thickness = 25.0F;

/*************************************************************************************************/
GYDM::PlanetCuteAtlas::PlanetCuteAtlas(int row, int col, GroundBlockType default_type)
    : GridAtlas(GROUND_ATLAS_PATH, 1, 8), default_type(default_type) {
        this->map_row = row;
        this->map_col = col;
}

GYDM::PlanetCuteAtlas::~PlanetCuteAtlas() {
    if (this->tiles != nullptr) {
        for (int r = 0; r < this->map_row; r ++) {
            delete [] this->tiles[r];
        }

        delete [] this->tiles;
    }
}

Margin GYDM::PlanetCuteAtlas::get_original_margin() {
    return { planet_cute_tile_top_margin, 0.0F, 0.0F, 0.0F };
}

Margin GYDM::PlanetCuteAtlas::get_original_map_overlay() {
    return { planet_cute_tile_top_margin, 0.5F, planet_cute_tile_thickness, 0.5F };
}

void GYDM::PlanetCuteAtlas::set_tile_type(int r, int c, GroundBlockType type) {
    r = safe_index(r, this->map_row);
    c = safe_index(c, this->map_col);

    if (this->tiles[r][c] != type) {
        this->tiles[r][c] = type;
        this->notify_updated();
    }
}

void GYDM::PlanetCuteAtlas::on_tilemap_load(shared_texture_t atlas) {
    GridAtlas::on_tilemap_load(atlas);

    this->tiles = new GroundBlockType*[this->map_row];
    for (int r = 0; r < this->map_row; r ++) {
        this->tiles[r] = new GroundBlockType[this->map_col];

        for (int c = 0; c < this->map_col; c ++) {
            this->tiles[r][c] = default_type;   
        }
    }
}

int GYDM::PlanetCuteAtlas::get_atlas_tile_index(size_t map_idx, int& xoff, int& yoff) {
    int idx = -1;

    if (this->tiles != nullptr) {
        idx = static_cast<int>(this->tiles[map_idx / this->map_col][map_idx % this->map_col]);
    }

    return idx;
}

/*************************************************************************************************/
GYDM::PlanetCuteTile::PlanetCuteTile(GroundBlockType default_type, int row, int col)
    : GridAtlas(GROUND_ATLAS_PATH, 1, 8), type(default_type) {
        this->map_row = row;
        this->map_col = col;
}

Margin GYDM::PlanetCuteTile::get_original_margin() {
    return { planet_cute_tile_top_margin, 0.0F, 0.0F, 0.0F };
}

Margin GYDM::PlanetCuteTile::get_original_map_overlay() {
    return { planet_cute_tile_top_margin, 0.5F, planet_cute_tile_thickness, 0.5F };
}

void GYDM::PlanetCuteTile::set_type(GroundBlockType type) {
    if (this->type != type) {
        this->type = type;
        this->notify_updated();
    }
}

int GYDM::PlanetCuteTile::get_atlas_tile_index(size_t map_idx, int& xoff, int& yoff) {
    return static_cast<int>(this->type);
}
