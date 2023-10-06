#pragma once

#include "../../forward.hpp"

#include "../graphlet.hpp"
#include "../../graphics/texture.hpp"

namespace WarGrey::STEM {
    class __lambda__ Tracklet : public WarGrey::STEM::IGraphlet {
    public:
        Tracklet(float size, uint32_t hex = 0x0U, double alpha = 1.0) : Tracklet(size, size, hex, alpha) {}
        Tracklet(float width, float height, uint32_t hex = 0x0U, double alpha = 1.0);
        virtual ~Tracklet() { this->invalidate_geometry(); }

        void construct(SDL_Renderer* renderer) override;

    public:
        void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
        void draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) override;

    public:
        void pen_down() { this->in_drawing = true; }
        void pen_up() { this->in_drawing = false; }
        void set_drawing(bool yes_or_no) { this->in_drawing = yes_or_no; }
        bool is_drawing() { return this->in_drawing; }
        void set_pen_color(uint32_t hex, double alpha = 1.0);
        void set_pen_color(double hue, double saturation = 1.0, double brightness = 1.0, double alpha = 1.0);
        uint32_t get_pen_color() { return this->color; }
        double get_pen_alpha() { return this->alpha; }
        double get_pen_color_hue();
        void set_pen_width(uint8_t width);
        uint8_t get_pen_width() { return this->line_width; }

    public:
        void add_line(float x1, float y1, float x2, float y2);
        void stamp(WarGrey::STEM::IMatter* matter, float x, float y);
        void erase();

    private:
        void invalidate_geometry();
        void clear_geometry();
        void resolve_boundary(float x, float y);

    private:
        SDL_Renderer* master = nullptr;
        shared_texture_t diagram = nullptr;
        bool in_drawing = false;
        float xmin;
        float xmax;
        float ymin;
        float ymax;

    private:
        float width;
        float height;
        uint8_t line_width;
        uint32_t color;
        double alpha;
    };
}
