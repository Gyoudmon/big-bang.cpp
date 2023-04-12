#pragma once

#include "../../../sprite/folder.hpp"

#include <utility>

namespace WarGrey::STEM {
    enum class GroundBlockType { Soil, Dirt, Grass, Stone, Water, Wood };

    /*********************************************************************************************/
    class PlanetCuteTile : public WarGrey::STEM::Sprite {
    public:
        PlanetCuteTile(GroundBlockType default_type);

    public:
        void feed_original_margin(float x, float y, float* top = nullptr, float* right = nullptr, float* bottom = nullptr, float* left = nullptr) override;
        float get_thickness();

    public:
        void set_type(GroundBlockType type);
        GroundBlockType get_type() { return this->type; }

    protected:
        int get_initial_costume_index() override;

    private:
        const char* type_to_name(GroundBlockType type);

    private:
        GroundBlockType type;
    };
}
