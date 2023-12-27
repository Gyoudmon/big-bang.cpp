#pragma once

#include "../../../atlas.hpp"

#include <utility>

namespace GYDM {
    enum class MarioPipeColor {
        // Order matters
        Green = 0,
        Gray = 1,
        Red = 2,
        Steel = 3,
        Orange = 4,
        Blue = 5
    };

    enum class MarioVPipeDirection { Up, Down, Both };
    enum class MarioHPipeDirection { Left, Right, Both };

    /*********************************************************************************************/
    class __lambda__ MarioGroundAtlas : public GYDM::GridAtlas {
    public:
        MarioGroundAtlas(size_t slot, int row, int col, float tile_size = 0.0F);

    protected:
        int get_atlas_tile_index(size_t map_idx, int& xoff, int& yoff) override;
        
    protected:
        virtual void feed_ground_tile_index(size_t map_idx, int& row, int& col);
    
    private:
        size_t slot = 0;
    };

    /*********************************************************************************************/
    class __lambda__ MarioPipe : public GYDM::GridAtlas {
    public:
        MarioPipe(int row, int col,
            GYDM::MarioPipeColor color = MarioPipeColor::Green,
            float tile_size = 0.0F);

    public:
        void set_color(GYDM::MarioPipeColor color);
        GYDM::MarioPipeColor get_color();

    protected:
        int color_idx;
    };

    class __lambda__ MarioVPipe : public GYDM::MarioPipe {
    public:
        MarioVPipe(int length,
            GYDM::MarioVPipeDirection dir = MarioVPipeDirection::Up,
            GYDM::MarioPipeColor color = MarioPipeColor::Green,
            float tile_size = 0.0F);

    protected:
        int get_atlas_tile_index(size_t map_idx, int& xoff, int& yoff) override;
        
    private:
        GYDM::MarioVPipeDirection direction;
    };

    class __lambda__ MarioHPipe : public GYDM::MarioPipe {
    public:
        MarioHPipe(int length,
            GYDM::MarioHPipeDirection dir = MarioHPipeDirection::Both,
            GYDM::MarioPipeColor color = MarioPipeColor::Green,
            float tile_size = 0.0F);

    protected:
        int get_atlas_tile_index(size_t map_idx, int& xoff, int& yoff) override;
        
    private:
        GYDM::MarioHPipeDirection direction;
    };
}
