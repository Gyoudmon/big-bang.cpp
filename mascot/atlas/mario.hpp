#pragma once

#include "../../matter/atlas.hpp"

namespace WarGrey::STEM {
    class MarioWorldAtlas : public WarGrey::STEM::GridAtlas {
    public:
        MarioWorldAtlas(int row, int col, float tile_size);
    };
}
