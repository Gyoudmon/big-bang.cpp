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
WarGrey::STEM::IShapelet::IShapelet(int64_t color, int64_t bcolor) {
    this->set_fill_color(color);
    this->set_pen_color(bcolor);
}

void WarGrey::STEM::IShapelet::draw_on_canvas(SDL_Renderer* renderer, float flwidth, float flheight) {
    int width = fl2fxi(flwidth);
    int height = fl2fxi(flheight);
    int64_t fcolor = this->get_fill_color();
    int64_t pcolor = this->get_pen_color();
    uint8_t r, g, b;

    if (fcolor >= 0) {
        RGB_From_Hexadecimal(static_cast<uint32_t>(fcolor), &r, &g, &b);
        this->fill_shape(renderer, width, height, r, g, b, color_component_to_byte(this->get_fill_alpha()));
    }

    if (pcolor >= 0) {
        RGB_From_Hexadecimal(static_cast<uint32_t>(pcolor), &r, &g, &b);
        this->draw_shape(renderer, width, height, r, g, b, color_component_to_byte(this->get_pen_alpha()));
    }
}

/*************************************************************************************************/
WarGrey::STEM::Linelet::Linelet(float ex, float ey, int64_t color) : IShapelet(color, -1), epx(ex), epy(ey) {}
WarGrey::STEM::Linelet::Linelet(float ex, float ey, uint32_t color) : Linelet(ex, ey, static_cast<int64_t>(color)) {}

WarGrey::STEM::Linelet::Linelet(float ex, float ey, double hue, double saturation, double brightness)
    : Linelet(ex, ey, Hexadecimal_From_HSV(hue, saturation, brightness)) {}

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
WarGrey::STEM::Rectanglet::Rectanglet(float edge_size, int64_t color, int64_t border_color)
	: Rectanglet(edge_size, edge_size, color, border_color) {}

WarGrey::STEM::Rectanglet::Rectanglet(float edge_size, uint32_t color, int64_t border_color)
	: Rectanglet(edge_size, static_cast<int64_t>(color), border_color) {}

WarGrey::STEM::Rectanglet::Rectanglet(float edge_size, double hue, double saturation, double brightness, int64_t border_color)
    : Rectanglet(edge_size, Hexadecimal_From_HSV(hue, saturation, brightness), border_color) {}

WarGrey::STEM::Rectanglet::Rectanglet(float width, float height, int64_t color, int64_t border_color)
	: IShapelet(color, border_color), width(width), height(height) {}

WarGrey::STEM::Rectanglet::Rectanglet(float width, float height, uint32_t color, int64_t border_color)
	: Rectanglet(width, height, static_cast<int64_t>(color), border_color) {}

WarGrey::STEM::Rectanglet::Rectanglet(float width, float height, double hue, double saturation, double brightness, int64_t border_color)
    : Rectanglet(width, height, Hexadecimal_From_HSV(hue, saturation, brightness), border_color) {}

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
WarGrey::STEM::RoundedRectanglet::RoundedRectanglet(float edge_size, float radius, int64_t color, int64_t border_color)
	: RoundedRectanglet(edge_size, edge_size, radius, color, border_color) {}

WarGrey::STEM::RoundedRectanglet::RoundedRectanglet(float edge_size, float radius, uint32_t color, int64_t border_color)
	: RoundedRectanglet(edge_size, radius, static_cast<int64_t>(color), border_color) {}

WarGrey::STEM::RoundedRectanglet::RoundedRectanglet(float edge_size, float radius, double hue, double saturation, double brightness, int64_t border_color)
    : RoundedRectanglet(edge_size, radius, Hexadecimal_From_HSV(hue, saturation, brightness), border_color) {}

WarGrey::STEM::RoundedRectanglet::RoundedRectanglet(float width, float height, float radius, int64_t color, int64_t border_color)
	: IShapelet(color, border_color), width(width), height(height), radius(radius) {}

WarGrey::STEM::RoundedRectanglet::RoundedRectanglet(float width, float height, float radius, uint32_t color, int64_t border_color)
	: RoundedRectanglet(width, height, radius, static_cast<int64_t>(color), border_color) {}

WarGrey::STEM::RoundedRectanglet::RoundedRectanglet(float width, float height, float radius, double hue, double saturation, double brightness, int64_t border_color)
    : RoundedRectanglet(width, height, radius, Hexadecimal_From_HSV(hue, saturation, brightness), border_color) {}

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
WarGrey::STEM::Ellipselet::Ellipselet(float radius, int64_t color, int64_t border_color)
	: Ellipselet(radius, radius, color, border_color) {}

WarGrey::STEM::Ellipselet::Ellipselet(float radius, uint32_t color, int64_t border_color)
	: Ellipselet(radius, radius, static_cast<int64_t>(color), border_color) {}

WarGrey::STEM::Ellipselet::Ellipselet(float radius, double hue, double saturation, double brightness, int64_t border_color)
    : Ellipselet(radius, Hexadecimal_From_HSV(hue, saturation, brightness), border_color) {}

WarGrey::STEM::Ellipselet::Ellipselet(float a, float b, int64_t color, int64_t border_color)
	: IShapelet(color, border_color), aradius(a), bradius(b) {}

WarGrey::STEM::Ellipselet::Ellipselet(float a, float b, uint32_t color, int64_t border_color)
	: Ellipselet(a, b, static_cast<int64_t>(color), border_color) {}

WarGrey::STEM::Ellipselet::Ellipselet(float aradius, float bradius, double hue, double saturation, double brightness, int64_t border_color)
    : Ellipselet(aradius, bradius, Hexadecimal_From_HSV(hue, saturation, brightness), border_color) {}

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
WarGrey::STEM::Trianglet::Trianglet(float x2, float y2, float x3, float y3, int64_t color, int64_t border_color)
	: IShapelet(color, border_color), x2(x2), y2(y2), x3(x3), y3(y3) {}

WarGrey::STEM::Trianglet::Trianglet(float x2, float y2, float x3, float y3, uint32_t color, int64_t border_color)
	: Trianglet(x2, y2, x3, y3, static_cast<int64_t>(color), border_color) {}

WarGrey::STEM::Trianglet::Trianglet(float x2, float y2, float x3, float y3, double hue, double saturation, double brightness, int64_t border_color)
    : Trianglet(x2, y2, x3, y3, Hexadecimal_From_HSV(hue, saturation, brightness), border_color) {}

void WarGrey::STEM::Trianglet::on_resize(float w, float h, float width, float height) {
    IShapelet::on_resize(w, h, width, height);
    
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

void WarGrey::STEM::Trianglet::draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    int x = -fl2fxi(flmin(0.0F, this->x2, this->x3));
    int y = -fl2fxi(flmin(0.0F, this->y2, this->y3));

    aatrigonRGBA(renderer, x, y, fl2fxi(this->x2) + x, fl2fxi(this->y2) + y, fl2fxi(this->x3) + x, fl2fxi(this->y3) + y, r, g, b, a);
}

void WarGrey::STEM::Trianglet::fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    int x = -fl2fxi(flmin(0.0F, this->x2, this->x3));
    int y = -fl2fxi(flmin(0.0F, this->y2, this->y3));
    
    filledTrigonRGBA(renderer, x, y, fl2fxi(this->x2) + x, fl2fxi(this->y2) + y, fl2fxi(this->x3) + x, fl2fxi(this->y3) + y, r, g, b, a);
    aatrigonRGBA(renderer, x, y, fl2fxi(this->x2) + x, fl2fxi(this->y2) + y, fl2fxi(this->x3) + x, fl2fxi(this->y3) + y, r, g, b, a);
}

/*************************************************************************************************/
WarGrey::STEM::Polygonlet::Polygonlet(const polygon_vertices& vertices, uint32_t color, int64_t border_color)
	: Polygonlet(vertices, static_cast<int64_t>(color), border_color) {}

WarGrey::STEM::Polygonlet::Polygonlet(const polygon_vertices& vertices, double hue, double saturation, double brightness, int64_t border_color)
    : Polygonlet(vertices, Hexadecimal_From_HSV(hue, saturation, brightness), border_color) {}

WarGrey::STEM::Polygonlet::Polygonlet(const polygon_vertices& vertices, int64_t color, int64_t border_color) : IShapelet(color, border_color) {
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
    SET_BOX(w, flfloor(this->rx - this->lx) + 1.0F);
    SET_BOX(h, flfloor(this->by - this->ty) + 1.0F);
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
WarGrey::STEM::RegularPolygonlet::RegularPolygonlet(size_t n, float radius, int64_t color, int64_t border_color)
	: RegularPolygonlet(n, radius, 0.0F, color, border_color) {}

WarGrey::STEM::RegularPolygonlet::RegularPolygonlet(size_t n, float radius, uint32_t color, int64_t border_color)
	: RegularPolygonlet(n, radius, static_cast<int64_t>(color), border_color) {}

WarGrey::STEM::RegularPolygonlet::RegularPolygonlet(size_t n, float radius, double hue, double saturation, double brightness, int64_t border_color)
    : RegularPolygonlet(n, radius, Hexadecimal_From_HSV(hue, saturation, brightness), border_color) {}

WarGrey::STEM::RegularPolygonlet::RegularPolygonlet(size_t n, float radius, float rotation, uint32_t color, int64_t border_color)
	: RegularPolygonlet(n, radius, rotation, static_cast<int64_t>(color), border_color) {}

WarGrey::STEM::RegularPolygonlet::RegularPolygonlet(size_t n, float radius, float rotation, double hue, double saturation, double brightness, int64_t border_color)
    : RegularPolygonlet(n, radius, rotation, Hexadecimal_From_HSV(hue, saturation, brightness), border_color) {}

WarGrey::STEM::RegularPolygonlet::RegularPolygonlet(size_t n, float radius, float rotation, int64_t color, int64_t border_color)
	: Polygonlet(regular_polygon_vertices(n, radius, rotation), color, border_color), radius(radius) {}
