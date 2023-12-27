#include "mario.hpp"
#include "../../../../datum/path.hpp"

using namespace GYDM;

/*************************************************************************************************/
#define MARIO_WORLD_MAP digimon_mascot_path("atlas/mario", ".png")

typedef std::pair<int, int> row_col_t;

static const row_col_t mario_ground_positions[] = {
    { 0, 0 }, { 0, 17 }, { 0, 34 }, { 0, 51 },
    { 7, 0 }, { 7, 17 }, { 7, 34 }, { 7, 51 },
    { 14, 0 }, { 14, 17 },
    { 21, 0 }, { 21, 17 },
    { 28, 0 }, { 28, 17 }
};

static const row_col_t mario_up_pipe_positions[] = {
    { 22, 20 }, { 22, 23 }, { 22, 26 },
    { 24, 29 }, { 24, 32 }, { 26, 22 }
};

static const row_col_t mario_dw_pipe_positions[] = {
    { 22, 19 }, { 22, 22 }, { 22, 25 },
    { 24, 28 }, { 24, 31 }, { 26, 23 } 
};

static const row_col_t mario_lt_pipe_positions[] = {
    { 12, 38 }, { 12, 44 }, { 12, 50 },
    { 13, 56 }, { 13, 62 }, { 13, 38 }
};

static const row_col_t mario_rt_pipe_positions[] = {
    { 12, 42 }, { 12, 48 }, { 12, 54 },
    { 13, 60 }, { 13, 66 }, { 13, 42 }
};

/*************************************************************************************************/
GYDM::MarioGroundAtlas::MarioGroundAtlas(size_t slot, int row, int col, float tile_size)
        : GridAtlas(MARIO_WORLD_MAP, 46, 68), slot(slot) {
    this->map_row = row;
    this->map_col = col;
    this->map_tile_width = tile_size;
    this->map_tile_height = tile_size;
}

int GYDM::MarioGroundAtlas::get_atlas_tile_index(size_t map_idx, int& xoff, int& yoff) {
    row_col_t pos0 = mario_ground_positions[slot % (sizeof(mario_ground_positions) / sizeof(row_col_t))];
    int subrow = 0;
    int subcol = 0;

    this->feed_ground_tile_index(map_idx, subrow, subcol);
    subrow += pos0.first;
    subcol += pos0.second;

    return subrow * this->atlas_col + subcol;
}

void GYDM::MarioGroundAtlas::feed_ground_tile_index(size_t map_idx, int& subrow, int& subcol) {
    size_t col = map_idx % this->map_col;
    size_t row = map_idx / this->map_col;

    if ((col > 0) && (col < this->map_col - 1)) {
        subrow = 0;
        subcol = ((row == 0) ? 10 : 6) + col % 4;
    } else {
        subrow = (row == 0) ? 3 : 4;
        subcol = (col == 0) ? 0 : 2;
    }
}

/*************************************************************************************************/
GYDM::MarioPipe::MarioPipe(int row, int col, MarioPipeColor color, float tile_size)
        : GridAtlas(MARIO_WORLD_MAP, row, col), color_idx(static_cast<int>(color)) {
    this->map_row = 1;
    this->map_col = 1;

    this->map_tile_width = tile_size;
    this->map_tile_height = tile_size;
    this->camouflage(false);
}

void GYDM::MarioPipe::set_color(MarioPipeColor color) {
    int idx = static_cast<int>(color);

    if (idx != this->color_idx) {
        this->color_idx = idx;
        this->notify_updated();
    }
}

MarioPipeColor GYDM::MarioPipe::get_color() {
    return static_cast<MarioPipeColor>(this->color_idx);
}

GYDM::MarioVPipe::MarioVPipe(int length, MarioVPipeDirection dir, MarioPipeColor color, float tile_size)
        : MarioPipe(46, 34, color, tile_size), direction(dir) {
    this->map_row = length;
    this->map_tile_width *= 2.0F;
}

int GYDM::MarioVPipe::get_atlas_tile_index(size_t map_idx, int& xoff, int& yoff) {
    row_col_t pos;

    if (map_idx == 0) {
        if (this->direction == MarioVPipeDirection::Down) {
            pos = mario_dw_pipe_positions[this->color_idx];
        } else {
            pos = mario_up_pipe_positions[this->color_idx];
        }
    } else if (map_idx == this->map_row - 1) {
        if (this->direction == MarioVPipeDirection::Up) {
            pos = mario_up_pipe_positions[this->color_idx];
        } else {
            pos = mario_dw_pipe_positions[this->color_idx];
        }
        pos.first += 1;
    } else {
        /** NOTE
         * using `mario_dw_pipe_positions` here requires annoying adjustment of `yoff`
         **/
        pos = mario_up_pipe_positions[this->color_idx];
        pos.first += 1;
    }

    return pos.first * this->atlas_col + pos.second;
}

GYDM::MarioHPipe::MarioHPipe(int length, MarioHPipeDirection dir, MarioPipeColor color, float tile_size)
        : MarioPipe(23, 68, color, tile_size), direction(dir) {
    this->map_col = length;
    this->map_tile_height *= 2.0F;
}

int GYDM::MarioHPipe::get_atlas_tile_index(size_t map_idx, int& xoff, int& yoff) {
    row_col_t pos;

    if (map_idx == 0) {
        if (this->direction == MarioHPipeDirection::Right) {
            pos = mario_rt_pipe_positions[this->color_idx];
        } else {
            pos = mario_lt_pipe_positions[this->color_idx];
        }
    } else if (map_idx == this->map_col - 1) {
        if (this->direction == MarioHPipeDirection::Left) {
            pos = mario_lt_pipe_positions[this->color_idx];
        } else {
            pos = mario_rt_pipe_positions[this->color_idx];
        }
        pos.second += 1;
    } else {
        pos = mario_rt_pipe_positions[this->color_idx];
    }

    yoff = 1;

    return pos.first * this->atlas_col + pos.second;
}
