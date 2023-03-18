#pragma once

#include "../graphlet.hpp"
#include "../../graphics/colorspace.hpp"

namespace WarGrey::STEM {
    class Chromalet : public WarGrey::STEM::IGraphlet {
    public:
        Chromalet(float width, float height = 0.0F, WarGrey::STEM::CIE_Standard std = CIE_Standard::Primary, double Y = 1.0);
        virtual ~Chromalet() { this->invalidate_geometry(); }

    public:
        void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
        void draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) override;

    public:
        void set_standard(WarGrey::STEM::CIE_Standard std = CIE_Standard::Primary);
        WarGrey::STEM::CIE_Standard get_standard() { return this->standard; }
        void set_luminance(double Y = 1.0);

        void set_primary_color(uint32_t hex, size_t idx);
        uint32_t get_color(float mx, float my);

    protected:
        void on_resize(float new_width, float new_height, float old_width, float old_height) override;
    
    private:
        void draw_color_triangle(SDL_Renderer* renderer, int width, int height, double dx = 0.0, double dy = 0.0);
        void draw_color_map(SDL_Renderer* renderer, int width, int height, double dx = 0.0, double dy = 0.0);
        void draw_spectral_locus(SDL_Renderer* renderer, int width, int height, double dx = 0.0, double dy = 0.0);
        void draw_chromaticity(SDL_Renderer* renderer, int width, int height, double dx = 0.0, double dy = 0.0);

    private:
        void fix_render_location(double* x, double* y);
        void make_locus_polygon(double width, double height);
        void render_dot(SDL_Renderer* renderer, double x, double y, double width, double height,
                            double R, double G, double B, double dx, double dy, double A = 1.0);

    private:
        void invalidate_geometry();
        void invalidate_diagram();
        void invalidate_locus();

    private:
        SDL_Texture* diagram = nullptr;
        float width;
        float height;

    private:
        WarGrey::STEM::CIE_Standard standard;
        double luminance = 1.0;
        uint32_t primaries[3];

    private:
        double* locus_xs = nullptr;
        double* locus_ys = nullptr;
        size_t locus_count = 0U;
        double scanline_start = 0.0;
        double scanline_end = 0.0;
        int scanline_idx0 = 0U;
    };
}
