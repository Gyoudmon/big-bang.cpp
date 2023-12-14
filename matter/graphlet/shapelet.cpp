#include "shapelet.hpp"

#include "../../graphics/brush.hpp"
#include "../../graphics/image.hpp"
#include "../../physics/mathematics.hpp"

#include "../../datum/box.hpp"
#include "../../datum/flonum.hpp"

// https://www.ferzkopp.net/Software/SDL2_gfx/Docs/html/index.html
#include <SDL2/SDL2_gfxPrimitives.h>

using namespace WarGrey::STEM;

// WARNING: SDL_Surface needs special proceeding as it might cause weird distorted shapes

/*************************************************************************************************/
WarGrey::STEM::IShapelet::IShapelet(const RGBA& color, const RGBA& bcolor) {
    this->set_brush_color(color);
    this->set_pen_color(bcolor);
}

void WarGrey::STEM::IShapelet::draw_on_canvas(SDL_Renderer* renderer, float flwidth, float flheight) {
    int width = fl2fxi(flwidth);
    int height = fl2fxi(flheight);
    uint8_t r, g, b, a;

    if (this->brush_okay(&r, &g, &b, &a)) {
        this->fill_shape(renderer, width, height, r, g, b, a);
    }

    if (this->pen_okay(&r, &g, &b, &a)) {
        this->draw_shape(renderer, width, height, r, g, b, a);
    }
}

/*************************************************************************************************/
WarGrey::STEM::Linelet::Linelet(float ex, float ey, const RGBA& color) : IShapelet(color), epx(ex), epy(ey) {}

void WarGrey::STEM::Linelet::on_resize(float w, float h, float width, float height) { 
    IShapelet::on_resize(w, h, width, height);
    
    this->epx *= w / width;
    this->epy *= h / height;
}

void WarGrey::STEM::Linelet::feed_extent(float x, float y, float* width, float* height) {
    SET_BOX(width, flmax(flabs(this->epx), 1.0F));
    SET_BOX(height, flmax(flabs(this->epy), 1.0F));
}

void WarGrey::STEM::Linelet::fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    int x = 0;
    int y = 0;
    int xn = fl2fxi(this->epx);
    int yn = fl2fxi(this->epy);

    if (xn < 0) {
        x = x - xn;
    }

    if (yn < 0) {
        y = y - yn;
    }

    aalineRGBA(renderer, x, y, x + xn, y + yn, r, g, b, a);
}

/*************************************************************************************************/
WarGrey::STEM::Rectanglet::Rectanglet(float edge_size, const RGBA& color, const RGBA& border_color)
	: Rectanglet(edge_size, edge_size, color, border_color) {}

WarGrey::STEM::Rectanglet::Rectanglet(float width, float height, const RGBA& color, const RGBA& border_color)
	: IShapelet(color, border_color), width(width), height(height) {}

void WarGrey::STEM::Rectanglet::on_resize(float w, float h, float width, float height) {
    IShapelet::on_resize(w, h, width, height);
    
    this->width = w;
    this->height = h;
}

void WarGrey::STEM::Rectanglet::feed_extent(float x, float y, float* w, float* h) {
    SET_VALUES(w, this->width, h, this->height);
}

void WarGrey::STEM::Rectanglet::draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    rectangleRGBA(renderer, width, 0, 0, height, r, g, b, a);
}

void WarGrey::STEM::Rectanglet::fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    boxRGBA(renderer, width, 0, 0, height, r, g, b, a);
}

/*************************************************************************************************/
WarGrey::STEM::RoundedRectanglet::RoundedRectanglet(float edge_size, float radius, const RGBA& color, const RGBA& border_color)
	: RoundedRectanglet(edge_size, edge_size, radius, color, border_color) {}

WarGrey::STEM::RoundedRectanglet::RoundedRectanglet(float width, float height, float radius, const RGBA& color, const RGBA& border_color)
	: IShapelet(color, border_color), width(width), height(height), radius(radius) {}

void WarGrey::STEM::RoundedRectanglet::on_resize(float w, float h, float width, float height) {
    IShapelet::on_resize(w, h, width, height);
    
    this->width = w;
    this->height = h;
}

void WarGrey::STEM::RoundedRectanglet::feed_extent(float x, float y, float* w, float* h) {
    SET_VALUES(w, this->width, h, this->height);
}

void WarGrey::STEM::RoundedRectanglet::draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    float rad = this->radius;

    if (rad < 0.0F) {
        rad = -flmin(this->width, this->height) * rad;
    }

    roundedRectangleRGBA(renderer, 0, 0, width, height, fl2fxi(rad), r, g, b, a);
}

void WarGrey::STEM::RoundedRectanglet::fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    float rad = this->radius;

    if (rad < 0.0F) {
        rad = -flmin(this->width, this->height) * rad;
    }
    
    roundedBoxRGBA(renderer, 0, 0, width, height, fl2fxi(rad), r, g, b, a);
}

/*************************************************************************************************/
WarGrey::STEM::Ellipselet::Ellipselet(float radius, const RGBA& color, const RGBA& border_color)
	: Ellipselet(radius, radius, color, border_color) {}

WarGrey::STEM::Ellipselet::Ellipselet(float a, float b, const RGBA& color, const RGBA& border_color)
	: IShapelet(color, border_color), aradius(a), bradius(b) {}

void WarGrey::STEM::Ellipselet::on_resize(float w, float h, float width, float height) {
    IShapelet::on_resize(w, h, width, height);
    
    this->aradius = w * 0.5F;
    this->bradius = h * 0.5F;
}

void WarGrey::STEM::Ellipselet::feed_extent(float x, float y, float* w, float* h) {
    SET_VALUES(w, this->aradius * 2.0F, h, this->bradius * 2.0F);
}

void WarGrey::STEM::Ellipselet::draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    short rx = fl2fx<short>(this->aradius) - 1;
    short ry = fl2fx<short>(this->bradius) - 1;
    short cx = short(rx) + 1;
    short cy = short(ry) + 1;

    if (rx == ry) {
        aacircleRGBA(renderer, cx, cy, rx, r, g, b, a);
    } else {
        aaellipseRGBA(renderer, cx, cy, rx, ry, r, g, b, a);
    }
}

void WarGrey::STEM::Ellipselet::fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    short rx = fl2fx<short>(this->aradius) - 1;
    short ry = fl2fx<short>(this->bradius) - 1;
    short cx = short(rx) + 1;
    short cy = short(ry) + 1;

    if (rx == ry) {
        filledCircleRGBA(renderer, cx, cy, rx, r, g, b, a);
        aacircleRGBA(renderer, cx, cy, rx, r, g, b, a);
    } else {
        filledEllipseRGBA(renderer, cx, cy, rx, ry, r, g, b, a);
        aaellipseRGBA(renderer, cx, cy, rx, ry, r, g, b, a);
    }
}

/*************************************************************************************************/
WarGrey::STEM::Polygonlet::Polygonlet(const polygon_vertices& vertices, const RGBA& color, const RGBA& border_color)
    : IShapelet(color, border_color) {
    this->n = vertices.size();

    if (this->n > 0) {
        this->xs = new float[this->n];
        this->ys = new float[this->n];
        this->txs = new short[this->n];
        this->tys = new short[this->n];

        for (size_t idx = 0; idx < this->n; idx++) {
            this->xs[idx] = vertices[idx].first;
            this->ys[idx] = vertices[idx].second;
        }

        this->initialize_vertices(1.0F, 1.0F);
    }
}

WarGrey::STEM::Polygonlet::~Polygonlet() {
    if (this->xs != nullptr) {
        delete [] this->xs;
        delete [] this->txs;
    }

    if (this->ys != nullptr) { 
        delete [] this->ys;
        delete [] this->tys;
    }
}

void WarGrey::STEM::Polygonlet::initialize_vertices(float xscale, float yscale) {
    if (this->n > 0) {
        this->lx = infinity_f;
        this->ty = infinity_f;
        this->rx = -infinity_f;
        this->by = -infinity_f;
    
        for (size_t idx = 0; idx < this->n; idx++) {
            float px = this->xs[idx] * xscale;
            float py = this->ys[idx] * yscale;

            if (this->lx > px) this->lx = px;
            if (this->rx < px) this->rx = px;

            if (this->ty > py) this->ty = py;
            if (this->by < py) this->by = py;
        }

        for (size_t idx = 0; idx < this->n; idx ++) {
            this->txs[idx] = fl2fx<short>(this->xs[idx] * xscale - this->lx);
            this->tys[idx] = fl2fx<short>(this->ys[idx] * yscale - this->ty);
        }
    }
}

void WarGrey::STEM::Polygonlet::on_resize(float w, float h, float width, float height) {
    IShapelet::on_resize(w, h, width, height);
    this->initialize_vertices((w / width), (h / height));
}

void WarGrey::STEM::Polygonlet::feed_extent(float x, float y, float* w, float* h) {
    SET_BOX(w, this->rx - this->lx + 1.0F);
    SET_BOX(h, this->by - this->ty + 1.0F);
}

void WarGrey::STEM::Polygonlet::draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (this->n > 2) {
        aapolygonRGBA(renderer, this->txs, this->tys, this->n, r, g, b, a);
    } else {
        // line and dot have no borders
    }
}

void WarGrey::STEM::Polygonlet::fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (this->n > 2) {
        filledPolygonRGBA(renderer, this->txs, this->tys, this->n, r, g, b, a);
        aapolygonRGBA(renderer, this->txs, this->tys, this->n, r, g, b, a);
    } else if (this->n == 2) {
        aalineRGBA(renderer, this->txs[0], this->tys[0], this->txs[1], this->tys[1], r, g, b, a);
    } else if (this->n == 1) {
        pixelRGBA(renderer, this->txs[0], this->tys[0], r, g, b, a);
    }
}

/*************************************************************************************************/
WarGrey::STEM::RegularPolygonlet::RegularPolygonlet(size_t n, float radius, const RGBA& color, const RGBA& border_color)
	: RegularPolygonlet(n, radius, 0.0F, color, border_color) {}

WarGrey::STEM::RegularPolygonlet::RegularPolygonlet(size_t n, float radius, float rotation, const RGBA& color, const RGBA& border_color)
	: Polygonlet(regular_polygon_vertices(n, radius, rotation), color, border_color), _radius(radius) {}

WarGrey::STEM::Trianglet::Trianglet(float side_length, const RGBA& color, const RGBA& border_color)
	: Trianglet(side_length, 0.0F, color, border_color) {}

WarGrey::STEM::Trianglet::Trianglet(float side_length, float rotation, const RGBA& color, const RGBA& border_color)
	: RegularPolygonlet(3, side_length / (2.0F * flsin(pi_f / 3.0F)), rotation, color, border_color) {}
