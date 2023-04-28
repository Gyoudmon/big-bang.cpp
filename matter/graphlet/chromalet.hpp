#pragma once

#include "../graphlet.hpp"
#include "../../graphics/texture.hpp"
#include "../../graphics/colorspace.hpp"

namespace WarGrey::STEM {
    class Chromalet : public WarGrey::STEM::IGraphlet {
    public:
        Chromalet(float width, float height = 0.0F, WarGrey::STEM::CIE_Standard std = CIE_Standard::Primary, double Y = 1.0);
        virtual ~Chromalet() { this->invalidate_geometry(); }

    public:
        void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
        void draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) override;
        bool is_colliding_with_mouse(float x, float y) override;

    public:
        void set_standard(WarGrey::STEM::CIE_Standard std = CIE_Standard::Primary);
        WarGrey::STEM::CIE_Standard get_standard() { return this->standard; }
        void set_luminance(double Y = 1.0);

    public:
        void recalculate_primary_colors(int idx = -1);
        void set_pseudo_primary_triangle_color(uint32_t color, double alpha = 1.0);
        void set_pseudo_primary_triangle_alpha(double alpha);
        void set_pseudo_primary_color(uint32_t hex, size_t idx);
        void feed_pseudo_primary_color_location(size_t idx, float* x, float* y, float* fx = nullptr, float* fy = nullptr);
        void feed_pseudo_primary_color_location(size_t idx, double* x, double* y, double* fx = nullptr, double* fy = nullptr);
        void feed_color_location(uint32_t color, float* x, float* y, float* fx = nullptr, float* fy = nullptr);
        void feed_color_location(uint32_t color, double* x, double* y, double* fx = nullptr, double* fy = nullptr);
        uint32_t get_color_at(float mx, float my, bool after_special = false) { return this->get_color_at(double(mx), double(my), after_special); }
        uint32_t get_color_at(double mx, double my, bool after_special = false);
        bool is_point_inside_the_spectrum(double x, double y);
        
    protected:
        void on_resize(float new_width, float new_height, float old_width, float old_height) override;
    
    private:
        void draw_color_triangle(SDL_Renderer* renderer, double dx = 0.0, double dy = 0.0);
        void draw_color_map(SDL_Renderer* renderer, double width, double height, double dx = 0.0, double dy = 0.0);
        void draw_spectral_locus(SDL_Renderer* renderer, double width, double height, double dx = 0.0, double dy = 0.0);
        void draw_chromaticity(SDL_Renderer* renderer, double width, double height, double dx = 0.0, double dy = 0.0);

    private:
        void fix_render_location(double* x, double* y);
        void make_locus_polygon(double width, double height);
        void spectrum_intersection_vpoints(double x, double flheight, int& slt_idx, int& slb_idx, double* ty, double* by);
        void render_special_dot(SDL_Renderer* renderer, uint32_t c, float x, float y);
        void render_dot(SDL_Renderer* renderer, double x, double y, double width, double height,
                            double R, double G, double B, double dx, double dy, double A = 1.0);

    private:
        void invalidate_geometry();
        void invalidate_diagram();
        void invalidate_locus();

    private:
        shared_texture_t diagram = nullptr;
        float width;
        float height;

    private:
        WarGrey::STEM::CIE_Standard standard;
        double luminance;
        uint32_t pseudo_primaries[3] = {};
        uint32_t pseudo_primary_triangle_color;
        unsigned char pseudo_primary_triangle_alpha = 0;

    private:
        double* locus_xs = nullptr;
        double* locus_ys = nullptr;
        size_t locus_count = 0U;
        double scanline_start = 0.0;
        double scanline_end = 0.0;
        int scanline_idx0 = 0U;
    };
}
