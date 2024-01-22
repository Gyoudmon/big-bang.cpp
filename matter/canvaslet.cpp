#include "canvaslet.hpp"

#include "../datum/box.hpp"
#include "../datum/flonum.hpp"

#include "../graphics/image.hpp"

using namespace GYDM;

/*************************************************************************************************/
void GYDM::ICanvaslet::on_resize(float w, float h, float width, float height) {
    this->invalidate_canvas();
}

void GYDM::ICanvaslet::draw(GYDM::dc_t* dc, float flx, float fly, float flwidth, float flheight) {
    if (this->canvas.use_count() == 0) {
        int width = fl2fxi(flwidth) + 1;
        int height = fl2fxi(flheight) + 1;
    
        this->canvas = std::make_shared<Texture>(dc->create_blank_image(width, height));

        if (!this->canvas->okay()) {
            fprintf(stderr, "failed to refresh the canvas of %s: %s\n", this->name(), SDL_GetError());
            fflush(stderr);
        }
    }

    if (this->canvas->okay()) {
        if (this->needs_refresh_canvas) {
            SDL_Texture* origin = dc->get_target();
            
            dc->set_target(this->canvas->self());

            if (!this->canvas_background_color.is_transparent()) {
                dc->clear(this->canvas_background_color);
            }

            this->draw_on_canvas(dc, flwidth, flheight);

            dc->set_target(origin);
            SDL_SetTextureBlendMode(this->canvas->self(), color_mixture_to_blend_mode(this->mixture));

            this->needs_refresh_canvas = false;
        }

        this->draw_before_canvas(dc, flx, fly, flwidth, flheight);
        dc->stamp(this->canvas->self(), flx, fly, flwidth, flheight);
        this->draw_after_canvas(dc, flx, fly , flwidth, flheight);
    }
}

/*************************************************************************************************/
void GYDM::ICanvaslet::invalidate_canvas() {
    if (this->canvas.use_count() > 0) {
        this->canvas.reset();
    }

    this->needs_refresh_canvas = true;
    this->on_canvas_invalidated();
}

void GYDM::ICanvaslet::dirty_canvas(const RGBA& color) {
    if (this->canvas_background_color != color) {
        this->canvas_background_color = color;
        this->dirty_canvas();
    }

    if (!this->needs_refresh_canvas) {
        this->needs_refresh_canvas = true;
        this->notify_updated();
    }
}

/*************************************************************************************************/
bool GYDM::ICanvaslet::brush_okay(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) {
    this->brush_color.unbox(r, g, b, a);

    return !this->brush_color.is_transparent();
}

bool GYDM::ICanvaslet::pen_okay(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) {
    this->pen_color.unbox(r, g, b, a);
    
    return !this->pen_color.is_transparent();
}

void GYDM::ICanvaslet::set_color_mixture(ColorMixture mixture) {
    if (this->mixture != mixture) {
        this->mixture = mixture;
        this->dirty_canvas();
    }
}

void GYDM::ICanvaslet::set_canvas_alpha(double alpha) {
    if (this->canvas_background_color.alpha() != alpha) {
        this->canvas_background_color = RGBA(this->canvas_background_color, alpha);
        this->dirty_canvas();
    }
}

void GYDM::ICanvaslet::set_pen_color(const RGBA& c) {
    if (this->pen_color != c) {
        this->pen_color = c;
        this->dirty_canvas();
    }
}

void GYDM::ICanvaslet::set_pen_alpha(double alpha) {
    if (this->pen_color.alpha() != alpha) {
        this->pen_color = RGBA(this->pen_color, alpha);
        this->dirty_canvas();
    }
}

void GYDM::ICanvaslet::set_brush_color(const RGBA& c) {
    if (this->brush_color != c) {
        this->brush_color = c;
        this->dirty_canvas();
    }
}

void GYDM::ICanvaslet::set_brush_alpha(double alpha) {
    if (this->brush_color.alpha() != alpha) {
        this->brush_color = RGBA(this->brush_color, alpha);
        this->dirty_canvas();
    }
}
