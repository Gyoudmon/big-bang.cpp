#include "shapelet.hpp"

#include "../../graphics/image.hpp"
#include "../../physics/mathematics.hpp"

#include "../../datum/box.hpp"
#include "../../datum/flonum.hpp"

// https://www.ferzkopp.net/Software/SDL2_gfx/Docs/html/index.html
#include <SDL2/SDL2_gfxPrimitives.h>

using namespace GYDM;

// WARNING: SDL_Surface needs special proceeding as it might cause weird distorted shapes

/*************************************************************************************************/
GYDM::IShapelet::IShapelet(const RGBA& color, const RGBA& bcolor) {
    this->set_brush_color(color);
    this->set_pen_color(bcolor);
}

void GYDM::IShapelet::draw_on_canvas(GYDM::dc_t* dc, float flwidth, float flheight) {
    int width = fl2fxi(flwidth);
    int height = fl2fxi(flheight);
    uint8_t r, g, b, a;

    if (this->brush_okay(&r, &g, &b, &a)) {
        this->fill_shape(dc, width, height, r, g, b, a);
    }

    if (this->pen_okay(&r, &g, &b, &a)) {
        this->draw_shape(dc, width, height, r, g, b, a);
    }
}

/*************************************************************************************************/
GYDM::Linelet::Linelet(float ex, float ey, const RGBA& color) : IShapelet(color), epx(ex), epy(ey) {}

void GYDM::Linelet::on_resize(float w, float h, float width, float height) { 
    IShapelet::on_resize(w, h, width, height);
    
    this->epx *= w / width;
    this->epy *= h / height;
}

Box GYDM::Linelet::get_bounding_box() {
    return { flmax(flabs(this->epx), 1.0F), flmax(flabs(this->epy), 1.0F) };
}

void GYDM::Linelet::fill_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
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

    aalineRGBA(dc->self(), x, y, x + xn, y + yn, r, g, b, a);
}

/*************************************************************************************************/
GYDM::Rectanglet::Rectanglet(float edge_size, const RGBA& color, const RGBA& border_color)
	: Rectanglet(edge_size, edge_size, color, border_color) {}

GYDM::Rectanglet::Rectanglet(float width, float height, const RGBA& color, const RGBA& border_color)
	: IShapelet(color, border_color), width(width), height(height) {}

void GYDM::Rectanglet::on_resize(float w, float h, float width, float height) {
    IShapelet::on_resize(w, h, width, height);
    
    this->width = w;
    this->height = h;
}

Box GYDM::Rectanglet::get_bounding_box() {
    return { this->width, this->height };
}

void GYDM::Rectanglet::draw_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    rectangleRGBA(dc->self(), width, 0, 0, height, r, g, b, a);
}

void GYDM::Rectanglet::fill_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    boxRGBA(dc->self(), width, 0, 0, height, r, g, b, a);
}

/*************************************************************************************************/
GYDM::RoundedRectanglet::RoundedRectanglet(float edge_size, float radius, const RGBA& color, const RGBA& border_color)
	: RoundedRectanglet(edge_size, edge_size, radius, color, border_color) {}

GYDM::RoundedRectanglet::RoundedRectanglet(float width, float height, float radius, const RGBA& color, const RGBA& border_color)
	: IShapelet(color, border_color), width(width), height(height), radius(radius) {}

void GYDM::RoundedRectanglet::on_resize(float w, float h, float width, float height) {
    IShapelet::on_resize(w, h, width, height);
    
    this->width = w;
    this->height = h;
}

Box GYDM::RoundedRectanglet::get_bounding_box() {
    return { this->width, this->height };
}

void GYDM::RoundedRectanglet::draw_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    float rad = this->radius;

    if (rad < 0.0F) {
        rad = -flmin(this->width, this->height) * rad;
    }

    roundedRectangleRGBA(dc->self(), 0, 0, width, height, fl2fxi(rad), r, g, b, a);
}

void GYDM::RoundedRectanglet::fill_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    float rad = this->radius;

    if (rad < 0.0F) {
        rad = -flmin(this->width, this->height) * rad;
    }
    
    roundedBoxRGBA(dc->self(), 0, 0, width, height, fl2fxi(rad), r, g, b, a);
}

/*************************************************************************************************/
GYDM::Ellipselet::Ellipselet(float radius, const RGBA& color, const RGBA& border_color)
	: Ellipselet(radius, radius, color, border_color) {}

GYDM::Ellipselet::Ellipselet(float a, float b, const RGBA& color, const RGBA& border_color)
	: IShapelet(color, border_color), aradius(a), bradius(b) {}

void GYDM::Ellipselet::on_resize(float w, float h, float width, float height) {
    IShapelet::on_resize(w, h, width, height);
    
    this->aradius = w * 0.5F;
    this->bradius = h * 0.5F;
}

Box GYDM::Ellipselet::get_bounding_box() {
    return { this->aradius * 2.0F, this->bradius * 2.0F };
}

void GYDM::Ellipselet::draw_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    short rx = fl2fx<short>(this->aradius) - 1;
    short ry = fl2fx<short>(this->bradius) - 1;
    short cx = short(rx) + 1;
    short cy = short(ry) + 1;

    if (rx == ry) {
        aacircleRGBA(dc->self(), cx, cy, rx, r, g, b, a);
    } else {
        aaellipseRGBA(dc->self(), cx, cy, rx, ry, r, g, b, a);
    }
}

void GYDM::Ellipselet::fill_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    short rx = fl2fx<short>(this->aradius) - 1;
    short ry = fl2fx<short>(this->bradius) - 1;
    short cx = short(rx) + 1;
    short cy = short(ry) + 1;

    if (rx == ry) {
        filledCircleRGBA(dc->self(), cx, cy, rx, r, g, b, a);
        aacircleRGBA(dc->self(), cx, cy, rx, r, g, b, a);
    } else {
        filledEllipseRGBA(dc->self(), cx, cy, rx, ry, r, g, b, a);
        aaellipseRGBA(dc->self(), cx, cy, rx, ry, r, g, b, a);
    }
}

/*************************************************************************************************/
GYDM::Polygonlet::Polygonlet(const polygon_vertices& vertices, const RGBA& color, const RGBA& border_color)
    : IShapelet(color, border_color) {
    this->n = vertices.size();

    if (this->n > 0) {
        this->xs = new float[this->n];
        this->ys = new float[this->n];
        this->txs = new short[this->n];
        this->tys = new short[this->n];

        for (size_t idx = 0; idx < this->n; idx++) {
            this->xs[idx] = vertices[idx].x;
            this->ys[idx] = vertices[idx].y;
        }

        this->initialize_vertices(1.0F, 1.0F);
    }
}

GYDM::Polygonlet::~Polygonlet() {
    if (this->xs != nullptr) {
        delete [] this->xs;
        delete [] this->txs;
    }

    if (this->ys != nullptr) { 
        delete [] this->ys;
        delete [] this->tys;
    }
}

void GYDM::Polygonlet::initialize_vertices(float xscale, float yscale) {
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

void GYDM::Polygonlet::on_resize(float w, float h, float width, float height) {
    IShapelet::on_resize(w, h, width, height);
    this->initialize_vertices((w / width), (h / height));
}

Box GYDM::Polygonlet::get_bounding_box() {
    return { this->rx - this->lx + 1.0F,
             this->by - this->ty + 1.0F };
}

void GYDM::Polygonlet::draw_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (this->n > 2) {
        aapolygonRGBA(dc->self(), this->txs, this->tys, int(this->n), r, g, b, a);
    } else {
        // line and dot have no borders
    }
}

void GYDM::Polygonlet::fill_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (this->n > 2) {
        filledPolygonRGBA(dc->self(), this->txs, this->tys, int(this->n), r, g, b, a);
        aapolygonRGBA(dc->self(), this->txs, this->tys, int(this->n), r, g, b, a);
    } else if (this->n == 2) {
        aalineRGBA(dc->self(), this->txs[0], this->tys[0], this->txs[1], this->tys[1], r, g, b, a);
    } else if (this->n == 1) {
        pixelRGBA(dc->self(), this->txs[0], this->tys[0], r, g, b, a);
    }
}

/*************************************************************************************************/
GYDM::RegularPolygonlet::RegularPolygonlet(size_t n, float radius, const RGBA& color, const RGBA& border_color)
	: RegularPolygonlet(n, radius, 0.0F, color, border_color) {}

GYDM::RegularPolygonlet::RegularPolygonlet(size_t n, float radius, float rotation, const RGBA& color, const RGBA& border_color)
	: Polygonlet(regular_polygon_vertices(n, radius, rotation), color, border_color), _radius(radius) {}

GYDM::Trianglet::Trianglet(float side_length, const RGBA& color, const RGBA& border_color)
	: Trianglet(side_length, 0.0F, color, border_color) {}

GYDM::Trianglet::Trianglet(float side_length, float rotation, const RGBA& color, const RGBA& border_color)
	: RegularPolygonlet(3, side_length / (2.0F * flsin(pi_f / 3.0F)), rotation, color, border_color) {}
