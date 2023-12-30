#pragma once

#include "../../forward.hpp"

#include "../canvaslet.hpp"
#include "../../physics/geometry/aabox.hpp"

namespace GYDM {
    class __lambda__ Tracklet : public GYDM::ICanvaslet {
    public:
        Tracklet(float size, uint32_t hex = 0x0U, double alpha = 1.0) : Tracklet(size, size, hex, alpha) {}
        Tracklet(float width, float height, uint32_t hex = 0x0U, double alpha = 1.0);

    public:
        GYDM::Box get_bounding_box() override;
        
    public:
        void pen_down() { this->in_drawing = true; }
        void pen_up() { this->in_drawing = false; }
        void set_drawing(bool yes_or_no) { this->in_drawing = yes_or_no; }
        bool is_drawing() { return this->in_drawing; }
        void set_pen_width(uint8_t width);
        uint8_t get_pen_width() { return this->line_width; }

    public:
        void add_line(float x1, float y1, float x2, float y2);
        void stamp(GYDM::IMatter* matter, float x, float y);
        void erase();

    private:
        void resolve_boundary(float x, float y);

    private:
        bool in_drawing = false;
        float xmin;
        float xmax;
        float ymin;
        float ymax;

    private:
        float width;
        float height;
        uint8_t line_width;
    };
}
