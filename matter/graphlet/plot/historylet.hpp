#pragma once

#include "../../canvaslet.hpp"
#include "../../../physics/geometry/aabox.hpp"

#include <vector>

namespace GYDM {
    class __lambda__ Historylet : public GYDM::ICanvaslet {
    public:
        Historylet(float size, const GYDM::RGBA& line_color) : Historylet(size, size, line_color) {}
        Historylet(float width, float height, const GYDM::RGBA& line_color);

    public:
        GYDM::Box get_bounding_box() override;

    public:
        void push_back_datum(float x, float y);
        void set_capacity(size_t n);
        void clear();

    protected:
        void draw_on_canvas(GYDM::dc_t* dc, float Width, float Height) override;
        
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
