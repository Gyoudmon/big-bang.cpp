#pragma once

#include "../graphlet.hpp"
#include "../../graphics/colorspace.hpp"

namespace WarGrey::STEM {
    class Chromalet : public WarGrey::STEM::IGraphlet {
    public:
        Chromalet(float width, float height = 0.0F, WarGrey::STEM::CIE_Standard std = CIE_Standard::Primary, double Y = 1.0);
        virtual ~Chromalet() { this->invalidate_diagram(); }

    public:
        void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
        void draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) override;

    public:
        void set_standard(WarGrey::STEM::CIE_Standard std = CIE_Standard::Primary);
        WarGrey::STEM::CIE_Standard get_standard() { return this->standard; }
        void set_luminance(double Y = 1.0);

    protected:
        void on_resize(float new_width, float new_height, float old_width, float old_height) override;
    
    private:
        void draw_point(SDL_Renderer* renderer, double x, double y, int dx = 0, int dy = 0);
        void draw_color_map(SDL_Renderer* renderer, int width, int height, int dx = 0, int dy = 0);
        void draw_spectral_locus(SDL_Renderer* renderer, int width, int height, int dx = 0, int dy = 0);
        void draw_chromaticity(SDL_Renderer* renderer, int width, int height, int dx = 0, int dy = 0);

    private:
        void invalidate_diagram();

    private:
        SDL_Texture* diagram;
        float width;
        float height;

    private:
        WarGrey::STEM::CIE_Standard standard;
        double luminance = 1.0;
    };
}
