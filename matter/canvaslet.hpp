#pragma once

#include "graphlet.hpp"

#include "../graphics/texture.hpp"
#include "../graphics/renderer.hpp"
#include "../physics/color/rgba.hpp"

namespace WarGrey::STEM {
    class __lambda__ ICanvaslet : public WarGrey::STEM::IGraphlet {
	public:
		ICanvaslet() { this->enable_resize(true); }
		virtual ~ICanvaslet() { this->invalidate_canvas(); }

	public:
		void draw(SDL_Renderer* ds, float x, float y, float Width, float Height) override;

	public:
		void dirty_canvas(const RGBA& c);
		void dirty_canvas() { this->dirty_canvas(this->canvas_background_color); }
		void dirty_canvas(uint32_t bg_color, double bg_alpha) { this->dirty_canvas(RGBA(bg_color, bg_alpha)); }
    
	    void set_color_mixture(WarGrey::STEM::ColorMixture mixture);
        WarGrey::STEM::RGBA get_canvas_color() { return this->canvas_background_color; }
		void set_canvas_alpha(uint8_t alpha) { this->set_canvas_alpha(double(alpha) / 255.0); }
        void set_canvas_alpha(double alpha);
		
		bool brush_okay(uint8_t* r = nullptr, uint8_t* g = nullptr, uint8_t* b = nullptr, uint8_t* a = nullptr);
		WarGrey::STEM::RGBA get_brush_color() { return this->brush_color; }
		void set_brush_color(const RGBA& c);
        void set_brush_alpha(uint8_t alpha) { this->set_brush_alpha(double(alpha) / 255.0); }
		void set_brush_alpha(double alpha);
        
		bool pen_okay(uint8_t* r = nullptr, uint8_t* g = nullptr, uint8_t* b = nullptr, uint8_t* a = nullptr);
        WarGrey::STEM::RGBA get_pen_color() { return this->pen_color; }
		void set_pen_color(const RGBA& c);
        void set_pen_alpha(uint8_t alpha) { this->set_pen_alpha(double(alpha) / 255.0); }
		void set_pen_alpha(double alpha);
        
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
		WarGrey::STEM::RGBA canvas_background_color;

	private:
		WarGrey::STEM::ColorMixture mixture = ColorMixture::Alpha;
		WarGrey::STEM::RGBA brush_color;
		WarGrey::STEM::RGBA pen_color;
	};
}
