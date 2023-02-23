#include "mario.hpp"
#include "../../datum/path.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::MarioWorldAtlas::MarioWorldAtlas(int row, int col, float tile_size)
        : GridAtlas(digimon_mascot_path("atlas/mario", ".png"), 45, 68) {
    this->map_row = row;
    this->map_col = col;
    this->map_tile_width = tile_size;
    this->map_tile_height = tile_size;
}
