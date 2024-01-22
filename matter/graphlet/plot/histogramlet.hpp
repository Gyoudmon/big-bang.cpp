#pragma once

#include "../../graphlet.hpp"
#include "../../../graphics/texture.hpp"
#include "../../../physics/geometry/aabox.hpp"

#include <vector>

namespace GYDM {
    class __lambda__ Histogramlet : public GYDM::IGraphlet {
    public:
        Histogramlet(float size, uint32_t box_hex, uint32_t mark_hex) : Histogramlet(size, size, box_hex, mark_hex) {}
        Histogramlet(float width, float height, uint32_t box_hex, uint32_t mark_hex);
        virtual ~Histogramlet() { this->invalidate_geometry(); }

    public:
        GYDM::Box get_bounding_box() override;
        void draw(GYDM::dc_t* dc, float x, float y, float Width, float Height) override;

    public:
        void push_back_datum(float x, float y);
        void set_color(uint32_t hex, double alpha = 1.0);
        void set_capacity(size_t n);
        void clear();
        
    protected:
        void on_resize(float new_width, float new_height, float old_width, float old_height) override;

    private:
        void clear_geometry();
        void invalidate_geometry();

    private:
        shared_texture_t diagram = nullptr;
        std::vector<std::pair<float, float>> raw_dots;
        size_t capacity;
        bool needs_refresh_diagram;
        float xmin;
        float xmax;
        float ymin;
        float ymax;

    private:
        float width;
        float height;
        uint32_t color;
        double alpha;
    };
}
