#pragma once

#include "../../graphlet.hpp"
#include "../../../graphics/texture.hpp"

#include <vector>

namespace WarGrey::STEM {
    class __lambda__ ConveyerBeltlet : public WarGrey::STEM::IGraphlet {
    public:
        ConveyerBeltlet(float radius, float belt_length, double hue);
        virtual ~ConveyerBeltlet() { this->invalidate_geometry(); }

    public:
        void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
        void draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) override;

    public:
        void set_color_hue(double hue);
        
    protected:
        void on_resize(float new_width, float new_height, float old_width, float old_height) override;

    private:
        void clear_geometry();
        void invalidate_geometry();

    private:
        shared_texture_t sketch = nullptr;
        bool needs_refresh_sketch = true;

    private:
        float radius;
        float belt_length;
        double hue;
    };
}
