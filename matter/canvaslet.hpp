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
		void dirty_canvas();
		void dirty_canvas(uint32_t bg_color, double bg_alpha);
    
	    void set_color_mixture(WarGrey::STEM::ColorMixture mixture);
        void set_canvas_alpha(uint8_t alpha) { this->set_canvas_alpha(double(alpha) / 255.0); }
        void set_canvas_alpha(double alpha);
		uint16_t get_canvas_color() { return this->canvas_background_color; }
		double get_canvas_alpha() { return this->canvas_background_alpha; }
        double get_canvas_alpha(uint8_t* a);

		bool fill_okay() { return (this->fill_alpha > 0.0) && (this->fill_alpha <= 1.0); }
        void set_fill_color(uint32_t color, uint8_t alpha) { this->set_fill_color(color, double(alpha) / 255.0); }
        void set_fill_color(uint32_t color, double alpha = 1.0);
        void set_fill_color_hsv(double hue, double saturation, double value);
        void set_fill_color_hsl(double hue, double saturation, double lightness);
        void set_fill_color_hsi(double hue, double saturation, double intensity);
        uint32_t get_fill_color() { return this->fill_color; }
		double get_fill_alpha() { return this->fill_alpha; }
		double get_fill_alpha(uint8_t* a);
        double get_fill_hue();

		bool pen_okay() { return (this->pen_alpha > 0.0) && (this->pen_alpha <= 1.0); }
        void set_pen_color(uint32_t color, uint8_t alpha) { this->set_pen_color(color, double(alpha) / 255.0); }
        void set_pen_color(uint32_t color, double alpha = 1.0);
        void set_pen_color_hsv(double hue, double saturation, double value);
        void set_pen_color_hsl(double hue, double saturation, double lightness);
        void set_pen_color_hsi(double hue, double saturation, double intensity);
        uint32_t get_pen_color() { return this->pen_color; }
		double get_pen_alpha() { return this->pen_alpha; }
		double get_pen_alpha(uint8_t* a);
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
		uint32_t fill_color;
		double fill_alpha = 0.0F;
        uint32_t pen_color;
		double pen_alpha = 0.0F;
	};
}
