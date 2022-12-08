#include "shapelet.hpp"

#include "../../graphics/colorspace.hpp"
#include "../../physics/mathematics.hpp"

#include "../../datum/box.hpp"
#include "../../datum/flonum.hpp"

// https://www.ferzkopp.net/Software/SDL2_gfx/Docs/html/index.html
#include <SDL2/SDL2_gfxPrimitives.h>

using namespace WarGrey::STEM;

/**
 * WARNING
 *   SDL_Surface needs special proceeding as it might cause weird distorted shapes
 **/

/*************************************************************************************************/
WarGrey::STEM::IShapelet::IShapelet(int32_t color, int32_t bcolor) : color(color), border_color(bcolor) {
    this->_sprite = this;
    this->enable_resizing(true);
    this->last_x = flnan_f;
    this->last_y = flnan_f;
}

void WarGrey::STEM::IShapelet::set_border_color(int32_t color) {
    if (this->border_color != color) {
        this->border_color = color;
        this->notify_updated();
    }
}

void WarGrey::STEM::IShapelet::set_color(int32_t color) {
    if (this->color != color) {
        this->color = color;
        this->notify_updated();
    }
}

void WarGrey::STEM::IShapelet::draw(SDL_Renderer* renderer, float flx, float fly, float flwidth, float flheight) {
    int x = fl2fxi(flx);
    int y = fl2fxi(fly);
    int width = fl2fxi(flwidth);
    int height = fl2fxi(flheight);
    uint8_t r, g, b;

    if ((this->last_x != flx) || (this->last_y != fly)) {
        this->last_x = flx;
        this->last_y = fly;
        this->on_moved(this->last_x, this->last_y);
    }

    if (this->color >= 0) {
        RGB_FromHexadecimal(this->color, &r, &g, &b);
        this->fill_shape(renderer, x, y, width, height, r, g, b, 0xFFU);
    }

    if (this->border_color >= 0) {
        RGB_FromHexadecimal(this->border_color, &r, &g, &b);
        this->draw_shape(renderer, x, y, width, height, r, g, b, 0xFFU);
    }
}

void WarGrey::STEM::IShapelet::dirty_cached_position() {
    this->last_x = flnan_f;
}

/*************************************************************************************************/
WarGrey::STEM::Linelet::Linelet(float ex, float ey, int32_t color) : IShapelet(color, -1), epx(ex), epy(ey) {}

void WarGrey::STEM::Linelet::on_resize(float w, float h, float width, float height) {
    this->epx *= w / width;
    this->epy *= h / height;
}

void WarGrey::STEM::Linelet::feed_extent(float x, float y, float* width, float* height) {
    SET_VALUES(width, flabs(this->epx), height, flabs(this->epy));
}

void WarGrey::STEM::Linelet::fill_shape(SDL_Renderer* renderer, int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
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
WarGrey::STEM::Rectanglet::Rectanglet(float edge_size, int32_t color, int32_t border_color)
	: Rectanglet(edge_size, edge_size, color, border_color) {}

WarGrey::STEM::Rectanglet::Rectanglet(float width, float height, int32_t color, int32_t border_color)
	: IShapelet(color, border_color), width(width), height(height) {}

void WarGrey::STEM::Rectanglet::on_resize(float w, float h, float width, float height) {
    this->width = w;
    this->height = h;
}

void WarGrey::STEM::Rectanglet::feed_extent(float x, float y, float* w, float* h) {
    SET_VALUES(w, this->width, h, this->height);
}

void WarGrey::STEM::Rectanglet::draw_shape(SDL_Renderer* renderer, int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    rectangleRGBA(renderer, x + width, y, x, y + height, r, g, b, a);
}

void WarGrey::STEM::Rectanglet::fill_shape(SDL_Renderer* renderer, int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    boxRGBA(renderer, x + width, y, x, y + height, r, g, b, a);
}

/*************************************************************************************************/
WarGrey::STEM::RoundedRectanglet::RoundedRectanglet(float edge_size, float radius, int32_t color, int32_t border_color)
	: RoundedRectanglet(edge_size, edge_size, radius, color, border_color) {}

WarGrey::STEM::RoundedRectanglet::RoundedRectanglet(float width, float height, float radius, int32_t color, int32_t border_color)
	: IShapelet(color, border_color), width(width), height(height), radius(radius) {}

void WarGrey::STEM::RoundedRectanglet::on_resize(float w, float h, float width, float height) {
    this->width = w;
    this->height = h;
}

void WarGrey::STEM::RoundedRectanglet::feed_extent(float x, float y, float* w, float* h) {
    SET_VALUES(w, this->width, h, this->height);
}

void WarGrey::STEM::RoundedRectanglet::draw_shape(SDL_Renderer* renderer, int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    float rad = this->radius;

    if (rad < 0.0F) {
        rad = -flmin(this->width, this->height) * rad;
    }

    roundedRectangleRGBA(renderer, x, y, x + width, y + height, fl2fxi(rad), r, g, b, a);
}

void WarGrey::STEM::RoundedRectanglet::fill_shape(SDL_Renderer* renderer, int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    float rad = this->radius;

    if (rad < 0.0F) {
        rad = -flmin(this->width, this->height) * rad;
    }
    
    roundedBoxRGBA(renderer, x, y, x + width, y + height, fl2fxi(rad), r, g, b, a);
}

/*************************************************************************************************/
WarGrey::STEM::Ellipselet::Ellipselet(float radius, int32_t color, int32_t border_color)
	: Ellipselet(radius, radius, color, border_color) {}

WarGrey::STEM::Ellipselet::Ellipselet(float a, float b, int32_t color, int32_t border_color)
	: IShapelet(color, border_color), aradius(a), bradius(b) {}

void WarGrey::STEM::Ellipselet::on_resize(float w, float h, float width, float height) {
    this->aradius = w * 0.5F;
    this->bradius = h * 0.5F;
}

void WarGrey::STEM::Ellipselet::feed_extent(float x, float y, float* w, float* h) {
    SET_VALUES(w, this->aradius * 2.0F, h, this->bradius * 2.0F);
}

void WarGrey::STEM::Ellipselet::draw_shape(SDL_Renderer* renderer, int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    short rx = fl2fx<short>(this->aradius) - 1;
    short ry = fl2fx<short>(this->bradius) - 1;
    short cx = short(rx + x);
    short cy = short(ry + y);

    if (rx == ry) {
        aacircleRGBA(renderer, cx, cy, rx, r, g, b, a);
    } else {
        aaellipseRGBA(renderer, cx, cy, rx, ry, r, g, b, a);
    }
}

void WarGrey::STEM::Ellipselet::fill_shape(SDL_Renderer* renderer, int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    short rx = fl2fx<short>(this->aradius) - 1;
    short ry = fl2fx<short>(this->bradius) - 1;
    short cx = short(rx + x);
    short cy = short(ry + y);

    if (rx == ry) {
        filledCircleRGBA(renderer, cx, cy, rx, r, g, b, a);
        aacircleRGBA(renderer, cx, cy, rx, r, g, b, a);
    } else {
        filledEllipseRGBA(renderer, cx, cy, rx, ry, r, g, b, a);
        aaellipseRGBA(renderer, cx, cy, rx, ry, r, g, b, a);
    }
}

/*************************************************************************************************/
WarGrey::STEM::Trianglet::Trianglet(float x2, float y2, float x3, float y3, int32_t color, int32_t border_color)
	: IShapelet(color, border_color), x2(x2), y2(y2), x3(x3), y3(y3) {}

void WarGrey::STEM::Trianglet::on_resize(float w, float h, float width, float height) {
    float xratio = w / width;
    float yratio = h / height;

    this->x2 *= xratio;
    this->y2 *= yratio;
    this->x3 *= xratio;
    this->y3 *= yratio;
}

void WarGrey::STEM::Trianglet::feed_extent(float x, float y, float* w, float* h) {
    float xmin = flmin(0.0F, this->x2, this->x3);
    float ymin = flmin(0.0F, this->y2, this->y3);
    float xmax = flmax(0.0F, this->x2, this->x3);
    float ymax = flmax(0.0F, this->y2, this->y3);

    SET_VALUES(w, xmax - xmin + 1.0F, h, ymax - ymin + 1.0F);
}

void WarGrey::STEM::Trianglet::draw_shape(SDL_Renderer* renderer, int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    x -= fl2fxi(flmin(0.0F, this->x2, this->x3));
    y -= fl2fxi(flmin(0.0F, this->y2, this->y3));

    aatrigonRGBA(renderer, x, y, fl2fxi(this->x2) + x, fl2fxi(this->y2) + y, fl2fxi(this->x3) + x, fl2fxi(this->y3) + y, r, g, b, a);
}

void WarGrey::STEM::Trianglet::fill_shape(SDL_Renderer* renderer, int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    x -= fl2fxi(flmin(0.0F, this->x2, this->x3));
    y -= fl2fxi(flmin(0.0F, this->y2, this->y3));
    
    filledTrigonRGBA(renderer, x, y, fl2fxi(this->x2) + x, fl2fxi(this->y2) + y, fl2fxi(this->x3) + x, fl2fxi(this->y3) + y, r, g, b, a);
    aatrigonRGBA(renderer, x, y, fl2fxi(this->x2) + x, fl2fxi(this->y2) + y, fl2fxi(this->x3) + x, fl2fxi(this->y3) + y, r, g, b, a);
}

/*************************************************************************************************/
WarGrey::STEM::RegularPolygonlet::RegularPolygonlet(int n, float radius, int32_t color, int32_t border_color)
	: RegularPolygonlet(n, radius, 0.0F, color, border_color) {}

WarGrey::STEM::RegularPolygonlet::RegularPolygonlet(int n, float radius, float rotation, int32_t color, int32_t border_color)
	: IShapelet(color, border_color), n(n), aradius(radius), bradius(radius), rotation(rotation) {
    this->initialize_vertice();
}

WarGrey::STEM::RegularPolygonlet::~RegularPolygonlet() {
    if (this->xs != nullptr) {
        delete [] this->xs;
        delete [] this->txs;
    }

    if (this->ys != nullptr) { 
        delete [] this->ys;
        delete [] this->tys;
    }
}

void WarGrey::STEM::RegularPolygonlet::initialize_vertice() {
    // for inscribed regular polygon, the radius should be `Rcos(pi/n)`
    float start = degrees_to_radians(this->rotation);
    float delta = 2.0F * pi_f / float(this->n);
    
    if (this->xs == nullptr) {
        this->xs = new float[this->n];
        this->txs = new short[this->n];
        this->ys = new float[this->n];
        this->tys = new short[this->n];
    }
    
    this->lx = this->aradius;
    this->ty = this->bradius;
    this->rx = -this->lx;
    this->by = -this->ty;

    for (int idx = 0; idx < this->n; idx++) {
        float theta = start + delta * float(idx);
        
        this->xs[idx] = this->aradius * flcos(theta);
        this->ys[idx] = this->bradius * flsin(theta);

        if (this->rx < this->xs[idx]) {
            this->rx = this->xs[idx];
        } else if (this->lx > this->xs[idx]) {
            this->lx = this->xs[idx];
        }

        if (this->by < this->ys[idx]) {
            this->by = this->ys[idx];
        } else if (this->ty > this->ys[idx]) {
            this->ty = this->ys[idx];
        }
    }
}

void WarGrey::STEM::RegularPolygonlet::on_moved(float new_x, float new_y) {
    float xoff = new_x - this->lx;
    float yoff = new_y - this->ty;

    for (int idx = 0; idx < this->n; idx ++) {
        this->txs[idx] = fl2fx<short>(this->xs[idx] + xoff);
        this->tys[idx] = fl2fx<short>(this->ys[idx] + yoff);
    }
}

void WarGrey::STEM::RegularPolygonlet::on_resize(float w, float h, float width, float height) {
    this->aradius *= (w / width);
    this->bradius *= (h / height);
    this->initialize_vertice();
    this->dirty_cached_position();
}

void WarGrey::STEM::RegularPolygonlet::feed_extent(float x, float y, float* w, float* h) {
    SET_VALUES(w, this->rx - this->lx + 1.0F, h, this->by - this->ty + 1.0F);
}

void WarGrey::STEM::RegularPolygonlet::draw_shape(SDL_Renderer* renderer, int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    aapolygonRGBA(renderer, this->txs, this->tys, this->n, r, g, b, a);
}

void WarGrey::STEM::RegularPolygonlet::fill_shape(SDL_Renderer* renderer, int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    filledPolygonRGBA(renderer, this->txs, this->tys, this->n, r, g, b, a);
    aapolygonRGBA(renderer, this->txs, this->tys, this->n, r, g, b, a);
}
