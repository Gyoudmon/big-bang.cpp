#pragma once

#include "../graphlet.hpp"
#include "../../graphics/texture.hpp"

#include <vector>

namespace WarGrey::STEM {
    class Historylet : public WarGrey::STEM::IGraphlet {
    public:
        Historylet(float size, uint32_t hex, double alpha = 1.0) : Historylet(size, size, hex, alpha) {}
        Historylet(float width, float height, uint32_t hex, double alpha = 1.0);
        virtual ~Historylet() { this->invalidate_geometry(); }

    public:
        void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
        void draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) override;

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
