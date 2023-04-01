#pragma once

#include "../../../atlas.hpp"

#include <utility>

namespace WarGrey::STEM {
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
    class MarioGroundAtlas : public WarGrey::STEM::GridAtlas {
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
    class MarioPipe : public WarGrey::STEM::GridAtlas {
    public:
        MarioPipe(int row, int col,
            WarGrey::STEM::MarioPipeColor color = MarioPipeColor::Green,
            float tile_size = 0.0F);

    public:
        void set_color(WarGrey::STEM::MarioPipeColor color);
        WarGrey::STEM::MarioPipeColor get_color();

    protected:
        int color_idx;
    };

    class MarioVPipe : public WarGrey::STEM::MarioPipe {
    public:
        MarioVPipe(int length,
            WarGrey::STEM::MarioVPipeDirection dir = MarioVPipeDirection::Up,
            WarGrey::STEM::MarioPipeColor color = MarioPipeColor::Green,
            float tile_size = 0.0F);

    protected:
        int get_atlas_tile_index(size_t map_idx, int& xoff, int& yoff) override;
        
    private:
        WarGrey::STEM::MarioVPipeDirection direction;
    };

    class MarioHPipe : public WarGrey::STEM::MarioPipe {
    public:
        MarioHPipe(int length,
            WarGrey::STEM::MarioHPipeDirection dir = MarioHPipeDirection::Both,
            WarGrey::STEM::MarioPipeColor color = MarioPipeColor::Green,
            float tile_size = 0.0F);

    protected:
        int get_atlas_tile_index(size_t map_idx, int& xoff, int& yoff) override;
        
    private:
        WarGrey::STEM::MarioHPipeDirection direction;
    };
}
