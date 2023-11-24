#include "tracklet.hpp"

#include "../../datum/box.hpp"
#include "../../datum/flonum.hpp"

#include "../../graphics/image.hpp"
#include "../../graphics/pen.hpp"
#include "../../graphics/colorspace.hpp"

// https://www.ferzkopp.net/Software/SDL2_gfx/Docs/html/index.html
#include <SDL2/SDL2_gfxPrimitives.h>

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::Tracklet::Tracklet(float width, float height, uint32_t hex, double alpha)
        : width(flabs(width)), height(flabs(height)), line_width(1), color(hex), alpha(alpha) {
    if (this->height == 0.0F) {
        this->height = this->width;
    }

    this->erase();
    this->enable_resize(false);
    this->camouflage(true);
}

void WarGrey::STEM::Tracklet::construct(SDL_Renderer* renderer) {
    int fxwidth = fl2fxi(this->width) + 1;
    int fxheight = fl2fxi(this->height) + 1;

    this->master = renderer;
    this->diagram = std::make_shared<Texture>(game_blank_image(this->master, fxwidth, fxheight));
}

void WarGrey::STEM::Tracklet::feed_extent(float x, float y, float* w, float* h) {
    SET_VALUES(w, this->width, h, this->height);
}

void WarGrey::STEM::Tracklet::draw(SDL_Renderer* renderer, float flx, float fly, float flwidth, float flheight) {
    if (this->diagram->okay()) {
        Pen::stamp(renderer, this->diagram->self(), flx, fly, flwidth, flheight);
    }
}

void WarGrey::STEM::Tracklet::add_line(float x1, float y1, float x2, float y2) {
    if (this->is_drawing()) {
        if (this->diagram->okay()) {
            if (this->master != nullptr) {
                SDL_Texture* origin = SDL_GetRenderTarget(this->master);
                short fx1 = fl2fx<short>(x1);
                short fy1 = fl2fx<short>(y1);
                short fx2 = fl2fx<short>(x2);
                short fy2 = fl2fx<short>(y2);
                uint8_t a = color_component_to_byte(this->alpha);
                uint8_t r, g, b;

                SDL_SetRenderTarget(this->master, this->diagram->self());

                RGB_From_Hexadecimal(this->color, &r, &g, &b);

                if (this->line_width <= 1) {
                    aalineRGBA(this->master, fx1, fy1, fx2, fy2, r, g, b, a);
                } else {
                    int radius = this->line_width / 2;

                    filledCircleRGBA(this->master, fx1, fy1, radius, r, g, b, a);
                    filledCircleRGBA(this->master, fx2, fy2, radius, r, g, b, a);
                    thickLineRGBA(this->master, fx1, fy1, fx2, fy2, this->line_width, r, g, b, a);
                }

                SDL_SetRenderTarget(this->master, origin);

                this->resolve_boundary(x1, y1);
                this->resolve_boundary(x2, y2);
            }
        }
    }
}

void WarGrey::STEM::Tracklet::stamp(WarGrey::STEM::IMatter* matter, float x, float y) {
    if (this->diagram->okay()) {
        if (this->master != nullptr) {
            SDL_Texture* origin = SDL_GetRenderTarget(this->master);
            float mwidth, mheight;
                
            SDL_SetRenderTarget(this->master, this->diagram->self());

            matter->feed_extent(x, y, &mwidth, &mheight);
            matter->draw(this->master, x, y, mwidth, mheight);

            SDL_SetRenderTarget(this->master, origin);

            this->resolve_boundary(x, y);
            this->resolve_boundary(x + mwidth, mheight);
        }
    }
}

/*************************************************************************************************/
void WarGrey::STEM::Tracklet::invalidate_geometry() {
    if (this->diagram.use_count() > 0) {
        this->diagram.reset();
    }
}

void WarGrey::STEM::Tracklet::clear_geometry() {
    if (this->diagram.use_count() > 0) {
        if (this->master != nullptr) {
            game_clear_image(this->master, this->diagram->self());
        }
    }
}

void WarGrey::STEM::Tracklet::erase() {
    if (this->xmax != -infinity) {
        this->xmax = this->ymax = -infinity;
        this->xmin = this->ymin = +infinity;
        this->clear_geometry();
        this->notify_updated();
    }
}

void WarGrey::STEM::Tracklet::set_pen_color(uint32_t hex, double alpha) {
    if ((this->color != hex) || (this->alpha != alpha)) {
        this->color = hex;
        this->alpha = alpha;
    }
}

void WarGrey::STEM::Tracklet::set_pen_color(double hue, double saturation, double brightness, double alpha) {
    this->set_pen_color(Hexadecimal_From_HSV(hue, saturation, brightness), alpha);
}

double WarGrey::STEM::Tracklet::get_pen_color_hue() {
    return HSB_Hue_From_Hexadecimal(this->color);
}

void WarGrey::STEM::Tracklet::set_pen_width(uint8_t lwidth) {
    if (this->line_width != lwidth) {
        this->line_width = lwidth;
    }
}

void WarGrey::STEM::Tracklet::resolve_boundary(float x, float y) {
    // don't merge with `else if`
    if (x < this->xmin) this->xmin = x;
    if (x > this->xmax) this->xmax = x;
    if (y < this->ymin) this->ymin = y;
    if (y > this->ymax) this->ymax = y;
}
