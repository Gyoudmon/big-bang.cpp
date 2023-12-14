#pragma once

#include "../../canvaslet.hpp"

#include <vector>

namespace WarGrey::STEM {
    class __lambda__ Historylet : public WarGrey::STEM::ICanvaslet {
    public:
        Historylet(float size, const WarGrey::STEM::RGBA& line_color) : Historylet(size, size, line_color) {}
        Historylet(float width, float height, const WarGrey::STEM::RGBA& line_color);

    public:
        void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;

    public:
        void push_back_datum(float x, float y);
        void set_capacity(size_t n);
        void clear();

    protected:
        void draw_on_canvas(SDL_Renderer* renderer, float Width, float Height) override;
        
    protected:
        void on_resize(float new_width, float new_height, float old_width, float old_height) override;

    private:
        std::vector<std::pair<float, float>> raw_dots;
        size_t capacity;
        float xmin;
        float xmax;
        float ymin;
        float ymax;

    private:
        float width;
        float height;
    };
}
