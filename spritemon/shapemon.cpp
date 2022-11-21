#include "shapemon.hpp"

#include "../image.hpp"
#include "../geometry.hpp"
#include "../colorspace.hpp"
#include "../mathematics.hpp"

#include "../datum/box.hpp"
#include "../datum/flonum.hpp"
#include "../datum/fixnum.hpp"

// https://www.ferzkopp.net/Software/SDL2_gfx/Docs/html/index.html
#include <SDL2/SDL2_gfxPrimitives.h>

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::IShapemon::IShapemon(int32_t color, int32_t bcolor) : color(color), border_color(bcolor) {
    this->enable_resizing(true);
}

WarGrey::STEM::IShapemon::~IShapemon() {
    if (this->geometry != nullptr) {
        SDL_FreeSurface(this->geometry);
    }
}

void WarGrey::STEM::IShapemon::construct() {
    SDL_Surface* geometry = nullptr;
    float width, height;

    this->fill_shape_extent(&width, &height);
    geometry = game_blank_image(width, height, this->alpha_color_key);
        
    if (geometry != nullptr) {
        SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(geometry);
        
        if (renderer != nullptr) {
            uint8_t r, g, b;

            RGB_SetRenderDrawColor(renderer, this->alpha_color_key);
            SDL_RenderClear(renderer);
    
            if (color >= 0) {
                RGB_FromHexadecimal(color, &r, &g, &b);
                this->fill_shape(renderer, width, height, r, g, b, 0xFFU);
            }

            if (border_color >= 0) {
                RGB_FromHexadecimal(border_color, &r, &g, &b);
                this->draw_shape(renderer, width, height, r, g, b, 0xFFU);
            }
        
            SDL_RenderPresent(renderer);
            SDL_DestroyRenderer(renderer);
        }
    }
    
    this->on_shape_changed(geometry);
}

void WarGrey::STEM::IShapemon::fill_extent(float x, float y, float* w, float* h) {
    if (this->geometry == nullptr) {
	SET_VALUES(w, 0.0F, h, 0.0F);
    } else {
	SET_VALUES(w, this->geometry->w, h, this->geometry->h);
    }
}

void WarGrey::STEM::IShapemon::fill_shape_origin(float* x, float* y) {
    SET_VALUES(x, 0.0F, y, 0.0F);
}

void WarGrey::STEM::IShapemon::set_alpha_key_color(uint32_t color) {
    if (this->alpha_color_key != color) {
        this->alpha_color_key = color;
        this->construct();
    }
}

void WarGrey::STEM::IShapemon::set_border_color(int32_t color) {
    if (this->border_color != color) {
        this->border_color = color;
        this->construct();
    }
}

void WarGrey::STEM::IShapemon::set_color(int32_t color) {
    if (this->color != color) {
        this->color = color;
        this->construct();
    }
}

void WarGrey::STEM::IShapemon::draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) {
    if (this->geometry != nullptr) {
        float ox, oy;

        this->fill_shape_origin(&ox, &oy);
	game_render_surface(renderer, this->geometry, x - ox, y - oy);
    }
}

/*************************************************************************************************/
void WarGrey::STEM::IShapemon::on_shape_changed(SDL_Surface* g) {
    if (this->geometry != nullptr) {
        SDL_FreeSurface(this->geometry);
    }

    this->geometry = g;
    this->notify_updated();
}

/*************************************************************************************************/
WarGrey::STEM::Linemon::Linemon(float ex, float ey, int32_t color) : IShapemon(color, -1), epx(ex), epy(ey) {}

void WarGrey::STEM::Linemon::resize(float w, float h) {
    if ((w > 0.0F) && (h > 0.0F)) {
        float width, height;
        
        this->fill_shape_extent(&width, &height);

	if ((width != w) || (height != h)) {
            this->epx *= w / width;
            this->epy *= h / height;
	    this->construct();
	}
    }
}

void WarGrey::STEM::Linemon::fill_shape_extent(float* width, float* height) {
    SET_VALUES(width, flmax(flabs(this->epx), 1.0F), height, flmax(flabs(this->epy), 1.0F));
}

void WarGrey::STEM::Linemon::fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    float x0 = 0.0F;
    float y0 = 0.0F;

    if (this->epx < 0.0F) {
        x0 = -this->epx;
    }

    if (this->epy < 0.0F) {
        y0 = -this->epy;
    }

    aalineRGBA(renderer, x0, y0, x0 + this->epx, y0 + this->epy, r, g, b, a);
}

/*************************************************************************************************/
WarGrey::STEM::Rectanglemon::Rectanglemon(float edge_size, int32_t color, int32_t border_color)
	: Rectanglemon(edge_size, edge_size, color, border_color) {}

WarGrey::STEM::Rectanglemon::Rectanglemon(float width, float height, int32_t color, int32_t border_color)
	: IShapemon(color, border_color), width(width), height(height) {}

void WarGrey::STEM::Rectanglemon::resize(float w, float h) {
    if ((w > 0.0F) && (h > 0.0F)) {
	if ((this->width != w) || (this->height != h)) {
            this->width = w;
            this->height = h;
	    this->construct();
	}
    }
}

void WarGrey::STEM::Rectanglemon::fill_shape_extent(float* w, float* h) {
    SET_VALUES(w, this->width, h, this->height);
}

void WarGrey::STEM::Rectanglemon::draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    rectangleRGBA(renderer, 1, 1, width - 1, height - 1, r, g, b, a);
}

void WarGrey::STEM::Rectanglemon::fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    boxRGBA(renderer, 1, 1, width - 1, height - 1, r, g, b, a);
}

/*************************************************************************************************/
WarGrey::STEM::RoundedRectanglemon::RoundedRectanglemon(float edge_size, float radius, int32_t color, int32_t border_color)
	: RoundedRectanglemon(edge_size, edge_size, radius, color, border_color) {}

WarGrey::STEM::RoundedRectanglemon::RoundedRectanglemon(float width, float height, float radius, int32_t color, int32_t border_color)
	: IShapemon(color, border_color), width(width), height(height), radius(radius) {}

void WarGrey::STEM::RoundedRectanglemon::resize(float w, float h) {
    if ((w > 0.0F) && (h > 0.0F)) {
	if ((this->width != w) || (this->height != h)) {
            this->width = w;
            this->height = h;
	    this->construct();
	}
    }
}

void WarGrey::STEM::RoundedRectanglemon::fill_shape_extent(float* w, float* h) {
    SET_VALUES(w, this->width, h, this->height);
}

void WarGrey::STEM::RoundedRectanglemon::draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    float rad = this->radius;

    if (rad < 0.0F) {
        rad = -flmin(this->width, this->height) * rad;
    }

    roundedRectangleRGBA(renderer, 1, 1, width - 1, height - 1, fl2fxi(rad), r, g, b, a);
}

void WarGrey::STEM::RoundedRectanglemon::fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    float rad = this->radius;

    if (rad < 0.0F) {
        rad = -flmin(this->width, this->height) * rad;
    }
    
    roundedBoxRGBA(renderer, 1, 1, width - 1, height - 1, fl2fxi(this->radius), r, g, b, a);
}

/*************************************************************************************************/
WarGrey::STEM::Ellipsemon::Ellipsemon(float radius, int32_t color, int32_t border_color)
	: Ellipsemon(radius, radius, color, border_color) {}

WarGrey::STEM::Ellipsemon::Ellipsemon(float a, float b, int32_t color, int32_t border_color)
	: IShapemon(color, border_color), aradius(a), bradius(b) {}

void WarGrey::STEM::Ellipsemon::resize(float w, float h) {
    if ((w > 0.0F) && (h > 0.0F)) {
        float a = w * 0.5F;
        float b = h * 0.5F;

	if ((this->aradius != a) || (this->bradius != b)) {
            this->aradius = a;
            this->bradius = b;
	    this->construct();
	}
    }
}

void WarGrey::STEM::Ellipsemon::fill_shape_extent(float* w, float* h) {
    SET_VALUES(w, this->aradius * 2.0F, h, this->bradius * 2.0F);
}

void WarGrey::STEM::Ellipsemon::draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    short rx = fl2fx<short>(this->aradius) - 1;
    short ry = fl2fx<short>(this->bradius) - 1;

    if (rx == ry) {
        aacircleRGBA(renderer, rx, ry, rx, r, g, b, a);
    } else {
        aaellipseRGBA(renderer, rx, ry, rx, ry, r, g, b, a);
    }
}

void WarGrey::STEM::Ellipsemon::fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    short rx = fl2fx<short>(this->aradius) - 1;
    short ry = fl2fx<short>(this->bradius) - 1;

    if (rx == ry) {
        filledCircleRGBA(renderer, rx, ry, rx, r, g, b, a);
        aacircleRGBA(renderer, rx, ry, rx, r, g, b, a);
    } else {
        filledEllipseRGBA(renderer, rx, ry, rx, ry, r, g, b, a);
        aaellipseRGBA(renderer, rx, ry, rx, ry, r, g, b, a);
    }
}

/*************************************************************************************************/
WarGrey::STEM::RegularPolygonmon::RegularPolygonmon(int n, float radius, int32_t color, int32_t border_color)
	: RegularPolygonmon(n, radius, 0.0F, color, border_color) {}

WarGrey::STEM::RegularPolygonmon::RegularPolygonmon(int n, float radius, float rotation, int32_t color, int32_t border_color)
	: IShapemon(color, border_color), n(n), aradius(radius), bradius(radius), rotation(rotation) {
    this->initialize_vertice();
}

WarGrey::STEM::RegularPolygonmon::~RegularPolygonmon() {
    if (this->xs != nullptr) {
        delete [] this->xs;
    }

    if (this->ys != nullptr) { 
        delete [] this->ys;
    }
}

void WarGrey::STEM::RegularPolygonmon::initialize_vertice() {
    // for inscribed regular polygon, the radius should be `Rcos(pi/n)`
    float start = degrees_to_radians(rotation);
    float delta = 2.0 * pi_f / float(n);
    float rx = this->aradius - 0.5F;
    float ry = this->bradius - 0.5F;

    if (this->xs == nullptr) {
        this->xs = new short[this->n];
        this->ys = new short[this->n];
    }
    
    for (int idx = 0; idx < n; idx++) {
        float theta = start + delta * float(idx);
        
        this->xs[idx] = rx * flcos(theta) + rx;
        this->ys[idx] = ry * flsin(theta) + ry;
    }
}

void WarGrey::STEM::RegularPolygonmon::resize(float w, float h) {
    if ((w > 0.0F) && (h > 0.0F)) {
        float a = w * 0.5F;
        float b = h * 0.5F;

	if ((this->aradius != a) || (this->bradius != b)) {
            this->aradius = a;
            this->bradius = b;
            this->initialize_vertice();
	    this->construct();
	}
    }
}

void WarGrey::STEM::RegularPolygonmon::fill_shape_extent(float* w, float* h) {
    SET_VALUES(w, this->aradius * 2.0F, h, this->bradius * 2.0F);
}

void WarGrey::STEM::RegularPolygonmon::draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    aapolygonRGBA(renderer, this->xs, this->ys, this->n, r, g, b, a);
}

void WarGrey::STEM::RegularPolygonmon::fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    filledPolygonRGBA(renderer, this->xs, this->ys, this->n, r, g, b, a);
    aapolygonRGBA(renderer, this->xs, this->ys, this->n, r, g, b, a);
}

