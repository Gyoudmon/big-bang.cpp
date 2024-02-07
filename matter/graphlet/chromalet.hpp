#pragma once

#include "../canvaslet.hpp"
#include "../../physics/color/CIE.hpp"
#include "../../physics/algebra/point.hpp"
#include "../../physics/geometry/aabox.hpp"

namespace GYDM {
    class __lambda__ Chromalet : public GYDM::ICanvaslet {
    public:
        Chromalet(float width, float height = 0.0F, GYDM::CIE_Standard std = CIE_Standard::Primary, double Y = 1.0);
        
    public:
        GYDM::Box get_bounding_box() override;
        bool is_colliding(const GYDM::Dot& local_pt) override;

    public:
        void set_standard(GYDM::CIE_Standard std = CIE_Standard::Primary);
        GYDM::CIE_Standard get_standard() { return this->standard; }
        void set_luminance(double Y = 1.0);

    public:
        void recalculate_primary_colors(int idx = -1);
        void set_pseudo_primary_triangle_color(const GYDM::RGBA& color);
        void set_pseudo_primary_triangle_alpha(double alpha);
        void set_pseudo_primary_color(const GYDM::RGBA& color, size_t idx);
        void feed_pseudo_primary_color_location(size_t idx, float* x, float* y, float* fx = nullptr, float* fy = nullptr);
        void feed_pseudo_primary_color_location(size_t idx, double* x, double* y, double* fx = nullptr, double* fy = nullptr);
        void feed_color_location(const GYDM::RGBA& color, float* x, float* y, float* fx = nullptr, float* fy = nullptr);
        void feed_color_location(const GYDM::RGBA& color, double* x, double* y, double* fx = nullptr, double* fy = nullptr);
        GYDM::RGBA get_color_at(float mx, float my, bool after_special = false) { return this->get_color_at(double(mx), double(my), after_special); }
        GYDM::RGBA get_color_at(double mx, double my, bool after_special = false);
        bool is_point_inside_the_spectrum(double x, double y);
        
    protected:
        void on_resize(float new_width, float new_height, float old_width, float old_height) override;
        
    protected:
        void draw_before_canvas(GYDM::dc_t* dc, float x, float y, float Width, float Height) override;
        void draw_on_canvas(GYDM::dc_t* dc, float Width, float Height) override;
        void draw_after_canvas(GYDM::dc_t* dc, float x, float y, float Width, float Height) override;
        void on_canvas_invalidated() override;
    
    private:
        void draw_color_triangle(GYDM::dc_t* dc, double dx = 0.0, double dy = 0.0);
        void draw_color_map(GYDM::dc_t* dc, double width, double height, double dx = 0.0, double dy = 0.0);
        void draw_spectral_locus(GYDM::dc_t* dc, double width, double height, double dx = 0.0, double dy = 0.0);
        void draw_chromaticity(GYDM::dc_t* dc, double width, double height, double dx = 0.0, double dy = 0.0);

    private:
        void fix_render_location(double* x, double* y);
        void make_locus_polygon(double width, double height);
        void spectrum_intersection_vpoints(double x, double flheight, int& slt_idx, int& slb_idx, double* ty, double* by);
        void render_special_dot(GYDM::dc_t* dc, const GYDM::RGBA& c, float x, float y);
        void render_dot(GYDM::dc_t* dc, double x, double y, double width, double height,
                            double R, double G, double B, double dx, double dy, double A = 1.0);

    private:
        void invalidate_locus();

    private:
        float width;
        float height;

    private:
        GYDM::CIE_Standard standard;
        GYDM::RGBA pseudo_primary_triangle_color;
        GYDM::RGBA pseudo_primaries[3];
        double luminance;
        
    private:
        double* locus_xs = nullptr;
        double* locus_ys = nullptr;
        size_t locus_count = 0U;
        double scanline_start = 0.0;
        double scanline_end = 0.0;
        int scanline_idx0 = 0U;
    };
}
