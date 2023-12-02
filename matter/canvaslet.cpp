#include "canvaslet.hpp"

#include "../datum/box.hpp"
#include "../datum/flonum.hpp"

#include "../graphics/image.hpp"
#include "../graphics/brush.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
void WarGrey::STEM::ICanvaslet::on_resize(float w, float h, float width, float height) {
    this->invalidate_canvas();
}

void WarGrey::STEM::ICanvaslet::draw(SDL_Renderer* renderer, float flx, float fly, float flwidth, float flheight) {
    if (this->canvas.use_count() == 0) {
        int width = fl2fxi(flwidth) + 1;
        int height = fl2fxi(flheight) + 1;
    
        this->canvas = std::make_shared<Texture>(game_blank_image(renderer, width, height));

        if (!this->canvas->okay()) {
            fprintf(stderr, "failed to refresh the canvas of %s: %s\n", this->name(), SDL_GetError());
            fflush(stderr);
        }
    }

    if (this->canvas->okay()) {
        if (this->needs_refresh_canvas) {
            SDL_Texture* origin = SDL_GetRenderTarget(renderer);
            
            SDL_SetRenderTarget(renderer, this->canvas->self());

            if ((this->canvas_background_alpha >= 0.0) && (this->canvas_background_alpha <= 1.0)) {
                Brush::clear(renderer, this->canvas_background_color, this->canvas_background_alpha);
            }

            this->draw_on_canvas(renderer, flwidth, flheight);

            SDL_SetRenderTarget(renderer, origin);
            SDL_SetTextureBlendMode(this->canvas->self(), color_mixture_to_blend_mode(this->mixture));

            this->needs_refresh_canvas = false;
        }

        this->draw_before_canvas(renderer, flx, fly, flwidth, flheight);
        Brush::stamp(renderer, this->canvas->self(), flx, fly, flwidth, flheight);
        this->draw_after_canvas(renderer, flx, fly , flwidth, flheight);
    }
}

/*************************************************************************************************/
void WarGrey::STEM::ICanvaslet::invalidate_canvas() {
    if (this->canvas.use_count() > 0) {
        this->canvas.reset();
    }

    this->needs_refresh_canvas = true;
    this->on_canvas_invalidated();
}

void WarGrey::STEM::ICanvaslet::dirty_canvas() {
    this->dirty_canvas(this->canvas_background_color, this->canvas_background_alpha);
}

void WarGrey::STEM::ICanvaslet::dirty_canvas(uint32_t color, double alpha) {
    if ((this->canvas_background_color != color) || (this->canvas_background_alpha != alpha)) {
        this->canvas_background_color = color;
        this->canvas_background_alpha = alpha;
        this->dirty_canvas();
    }

    if (!this->needs_refresh_canvas) {
        this->needs_refresh_canvas = true;
        this->notify_updated();
    }
}

/*************************************************************************************************/
void WarGrey::STEM::ICanvaslet::set_color_mixture(ColorMixture mixture) {
    if (this->mixture != mixture) {
        this->mixture = mixture;
        this->dirty_canvas();
    }
}

void WarGrey::STEM::ICanvaslet::set_canvas_alpha(double alpha) {
    if (this->canvas_background_alpha != alpha) {
        this->fill_alpha = alpha;
        this->dirty_canvas();
    }
}

double WarGrey::STEM::ICanvaslet::get_canvas_alpha(uint8_t* a) {
    SET_BOX(a, color_component_clamp_to_byte(this->canvas_background_alpha));

    return this->canvas_background_alpha;
}

void WarGrey::STEM::ICanvaslet::set_pen_color(uint32_t color, double alpha) {
    if ((this->pen_color != color) || (this->pen_alpha != alpha)) {
        this->pen_color = color;
        this->pen_alpha = alpha;
        this->dirty_canvas();
    }
}

void WarGrey::STEM::ICanvaslet::set_pen_color_hsv(double hue, double saturation, double value) {
    this->set_pen_color(Hexadecimal_From_HSV(hue, saturation, value));
}

void WarGrey::STEM::ICanvaslet::set_pen_color_hsl(double hue, double saturation, double lightness) {
    this->set_pen_color(Hexadecimal_From_HSL(hue, saturation, lightness));
}

void WarGrey::STEM::ICanvaslet::set_pen_color_hsi(double hue, double saturation, double intensity) {
    this->set_pen_color(Hexadecimal_From_HSI(hue, saturation, intensity));
}

double WarGrey::STEM::ICanvaslet::get_pen_hue() {
    double hue = flnan;

    if (this->pen_color >= 0) {
        hue = HSB_Hue_From_Hexadecimal(static_cast<uint32_t>(this->pen_color));
    }

    return hue;
}

double WarGrey::STEM::ICanvaslet::get_pen_alpha(uint8_t* a) {
    SET_BOX(a, color_component_clamp_to_byte(this->pen_alpha));

    return this->pen_alpha;
}

void WarGrey::STEM::ICanvaslet::set_fill_color(uint32_t color, double alpha) {
    if ((this->fill_color != color) || (this->fill_alpha != alpha)) {
        this->fill_color = color;
        this->fill_alpha = alpha;
        this->dirty_canvas();
    }
}

void WarGrey::STEM::ICanvaslet::set_fill_color_hsv(double hue, double saturation, double value) {
    this->set_fill_color(Hexadecimal_From_HSV(hue, saturation, value));
}

void WarGrey::STEM::ICanvaslet::set_fill_color_hsl(double hue, double saturation, double lightness) {
    this->set_fill_color(Hexadecimal_From_HSL(hue, saturation, lightness));
}

void WarGrey::STEM::ICanvaslet::set_fill_color_hsi(double hue, double saturation, double intensity) {
    this->set_fill_color(Hexadecimal_From_HSI(hue, saturation, intensity));
}

double WarGrey::STEM::ICanvaslet::get_fill_hue() {
    double hue = flnan;

    if (this->fill_color >= 0) {
        hue = HSB_Hue_From_Hexadecimal(static_cast<uint32_t>(this->fill_color));
    }

    return hue;
}

double WarGrey::STEM::ICanvaslet::get_fill_alpha(uint8_t* a) {
    SET_BOX(a, color_component_clamp_to_byte(this->fill_alpha));

    return this->fill_alpha;
}
