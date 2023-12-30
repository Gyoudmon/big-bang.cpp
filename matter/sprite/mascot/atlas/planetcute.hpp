#pragma once

#include "../../folder.hpp"
#include "../../../atlas.hpp"

#include "../../../../physics/geometry/margin.hpp"

#include <utility>

namespace GYDM {
    // WARNING: Order Matters
    enum class GroundBlockType { Soil, Wood, Water, Stone, Grass, Dirt, Wall, Plain };

    /*********************************************************************************************/
    class __lambda__ PlanetCuteAtlas : public GYDM::GridAtlas {
    public:
        PlanetCuteAtlas(int row, int col, GroundBlockType default_type = GroundBlockType::Plain);
        virtual ~PlanetCuteAtlas();

    public:
        GYDM::Margin get_original_margin() override;

    public:
        void set_tile_type(int r, int c, GroundBlockType type);
        GroundBlockType get_tile_type(int r, int c) { return this->tiles[r][c]; }

    protected:
        void on_tilemap_load(GYDM::shared_texture_t atlas) override;
        int get_atlas_tile_index(size_t map_idx, int& xoff, int& yoff) override;
        GYDM::Margin get_original_map_overlay() override;

    private:
        GroundBlockType** tiles = nullptr;
        GroundBlockType default_type;
    };

    /*********************************************************************************************/
    class __lambda__ PlanetCuteTile : public GYDM::GridAtlas {
    public:
        PlanetCuteTile(GroundBlockType default_type, int row = 1, int col = 1);

    public:
        GYDM::Margin get_original_margin() override;

    public:
        void set_type(GroundBlockType type);
        GroundBlockType get_type() { return this->type; }

    protected:
        int get_atlas_tile_index(size_t map_idx, int& xoff, int& yoff) override;
        GYDM::Margin get_original_map_overlay() override;

    private:
        GroundBlockType type;
    };
}
