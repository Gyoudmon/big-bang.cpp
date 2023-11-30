#pragma once

#include "graphlet.hpp"

#include "../graphics/texture.hpp"
#include "../graphics/colorspace.hpp"

namespace WarGrey::STEM {
    class __lambda__ ICanvaslet : public WarGrey::STEM::IGraphlet {
	public:
		ICanvaslet() { this->enable_resize(true); }
		virtual ~ICanvaslet() { this->invalidate_canvas(); }

	public:
		void draw(SDL_Renderer* ds, float x, float y, float Width, float Height) override;

	public:
		void dirty_canvas(uint32_t bg_color = 0U, double bg_alpha = 0.0);
    
	    void set_color_mixture(WarGrey::STEM::ColorMixture mixture);
        void set_alpha(uint8_t alpha) { this->set_alpha(double(alpha) / 255.0); }
        void set_alpha(double alpha);
        double get_alpha() { return this->canvas_background_alpha; }

        void set_fill_color(int64_t color, uint8_t alpha) { this->set_fill_color(color, double(alpha) / 255.0); }
        void set_fill_color(int64_t color, double alpha = 1.0);
        void set_fill_color_hsv(double hue, double saturation, double value);
        void set_fill_color_hsl(double hue, double saturation, double lightness);
        void set_fill_color_hsi(double hue, double saturation, double intensity);
        int64_t get_fill_color() { return this->fill_color; }
		double get_fill_alpha() { return this->fill_alpha; }
        double get_fill_hue();

        void set_pen_color(int64_t color, uint8_t alpha) { this->set_pen_color(color, double(alpha) / 255.0); }
        void set_pen_color(int64_t color, double alpha = 1.0);
        void set_pen_color_hsv(double hue, double saturation, double value);
        void set_pen_color_hsl(double hue, double saturation, double lightness);
        void set_pen_color_hsi(double hue, double saturation, double intensity);
        int64_t get_pen_color() { return this->pen_color; }
		double get_pen_alpha() { return this->pen_alpha; }
        double get_pen_hue();

	protected:
		void on_resize(float w, float h, float width, float height) override;

	protected:
		virtual void draw_on_canvas(SDL_Renderer* ds, float Width, float Height) {}
		virtual void draw_before_canvas(SDL_Renderer* ds, float x, float y, float Width, float Height) {}
		virtual void draw_after_canvas(SDL_Renderer* ds, float x, float y, float Width, float Height) {}
		virtual void invalidate_canvas();
		virtual void on_canvas_invalidated() {}

    protected:
		shared_texture_t canvas = nullptr;

	private:
        bool needs_refresh_canvas = true;
		uint32_t canvas_background_color = 0;
		double canvas_background_alpha = 0.0;

	private:
		ColorMixture mixture = ColorMixture::Alpha;
		int64_t fill_color = -1;
		double fill_alpha = 1.0F;
        int64_t pen_color = -1;
		double pen_alpha = 1.0F;
	};
}
