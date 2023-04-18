#pragma once

#include "../../../sprite/folder.hpp"
#include "../../../atlas.hpp"

#include <utility>

namespace WarGrey::STEM {
    // WARNING: Order Matters
    enum class GroundBlockType { Soil, Wood, Water, Stone, Grass, Dirt, Wall, Plain };

    /*********************************************************************************************/
    class PlanetCuteTile : public WarGrey::STEM::GridAtlas {
    public:
        PlanetCuteTile(GroundBlockType default_type, int row = 1, int col = 1);

    public:
        void feed_original_margin(float x, float y, float* top, float* right, float* bottom, float* left) override;

    public:
        void set_type(GroundBlockType type);
        GroundBlockType get_type() { return this->type; }

    protected:
        int get_atlas_tile_index(size_t map_idx, int& xoff, int& yoff) override;
        void feed_original_map_overlay(float* top, float* right, float* bottom, float* left) override;

    private:
        GroundBlockType type;
    };
}
