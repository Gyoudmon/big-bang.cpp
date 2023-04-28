#include "shapelet.hpp"

#include "../../graphics/image.hpp"
#include "../../graphics/geometry.hpp"
#include "../../physics/mathematics.hpp"

#include "../../datum/box.hpp"
#include "../../datum/flonum.hpp"

// https://www.ferzkopp.net/Software/SDL2_gfx/Docs/html/index.html
#include <SDL2/SDL2_gfxPrimitives.h>

using namespace WarGrey::STEM;

// WARNING: SDL_Surface needs special proceeding as it might cause weird distorted shapes

/*************************************************************************************************/
WarGrey::STEM::IShapelet::IShapelet(int32_t color, int32_t bcolor) : color(color), border_color(bcolor) {
    this->enable_resize(true);
}

void WarGrey::STEM::IShapelet::set_color_mixture(ColorMixture mixture) {
    if (this->mixture != mixture) {
        this->mixture = mixture;
        this->invalidate_geometry();
        this->notify_updated();
    }
}

void WarGrey::STEM::IShapelet::set_alpha(unsigned char a) {
    if (this->alpha != a) {
        this->alpha = a;
        this->invalidate_geometry();
        this->notify_updated();
    }
}

void WarGrey::STEM::IShapelet::set_alpha(double a) {
    if (a >= 1.0) {
        this->set_alpha(static_cast<unsigned char>(0xFFU));
    } else if (a <= 0.0) {
        this->set_alpha(static_cast<unsigned char>(0x00U));
    } else {
        this->set_alpha(fl2fx<unsigned char>(a * 255.0));
    }
}

void WarGrey::STEM::IShapelet::set_border_color(int32_t color) {
    if (this->border_color != color) {
        this->border_color = color;
        this->invalidate_geometry();
        this->notify_updated();
    }
}

void WarGrey::STEM::IShapelet::set_border_color_hsv(double hue, double saturation, double value) {
    this->set_border_color(Hexadecimal_From_HSV(hue, saturation, value));
}

void WarGrey::STEM::IShapelet::set_border_color_hsl(double hue, double saturation, double lightness) {
    this->set_border_color(Hexadecimal_From_HSL(hue, saturation, lightness));
}

void WarGrey::STEM::IShapelet::set_border_color_hsi(double hue, double saturation, double intensity) {
    this->set_border_color(Hexadecimal_From_HSI(hue, saturation, intensity));
}

double WarGrey::STEM::IShapelet::get_border_hsb_hue() {
    double hue = flnan;

    if (this->border_color >= 0) {
        hue = HSB_Hue_From_Hexadecimal(static_cast<uint32_t>(this->border_color));
    }

    return hue;
}

void WarGrey::STEM::IShapelet::set_color(int32_t color) {
    if (this->color != color) {
        this->color = color;
        this->invalidate_geometry();
        this->notify_updated();
    }
}

void WarGrey::STEM::IShapelet::set_color_hsv(double hue, double saturation, double value) {
    this->set_color(Hexadecimal_From_HSV(hue, saturation, value));
}

void WarGrey::STEM::IShapelet::set_color_hsl(double hue, double saturation, double lightness) {
    this->set_color(Hexadecimal_From_HSL(hue, saturation, lightness));
}

void WarGrey::STEM::IShapelet::set_color_hsi(double hue, double saturation, double intensity) {
    this->set_color(Hexadecimal_From_HSI(hue, saturation, intensity));
}

double WarGrey::STEM::IShapelet::get_color_hue() {
    double hue = flnan;

    if (this->color >= 0) {
        hue = HSB_Hue_From_Hexadecimal(static_cast<uint32_t>(this->color));
    }

    return hue;
}

void WarGrey::STEM::IShapelet::draw(SDL_Renderer* renderer, float flx, float fly, float flwidth, float flheight) {
    int width = fl2fxi(flwidth);
    int height = fl2fxi(flheight);
    uint8_t r, g, b;

    if (this->geometry.use_count() == 0U) {
        this->geometry = std::make_shared<Texture>(game_blank_image(renderer, width + 1, height + 1));

        if (this->geometry->okay()) {
            SDL_Texture* origin = SDL_GetRenderTarget(renderer);

            SDL_SetRenderTarget(renderer, this->geometry->self());

            if (this->color >= 0) {
                RGB_From_Hexadecimal(this->color, &r, &g, &b);
                this->fill_shape(renderer, width, height, r, g, b, this->alpha);
            }

            if (this->border_color >= 0) {
                RGB_From_Hexadecimal(this->border_color, &r, &g, &b);
                this->draw_shape(renderer, width, height, r, g, b, this->alpha);
            }

            SDL_SetRenderTarget(renderer, origin);
            SDL_SetTextureBlendMode(this->geometry->self(), color_mixture_to_blend_mode(this->mixture));
        } else {
            fprintf(stderr, "无法绘制几何图形：%s\n", SDL_GetError());
        }
    }

    if (this->geometry->okay()) {
        game_render_texture(renderer, this->geometry->self(), flx, fly, flwidth, flheight);
    }
}

void WarGrey::STEM::IShapelet::invalidate_geometry() {
    this->geometry.reset();
}

/*************************************************************************************************/
WarGrey::STEM::Linelet::Linelet(float ex, float ey, int32_t color) : IShapelet(color, -1), epx(ex), epy(ey) {}
WarGrey::STEM::Linelet::Linelet(float ex, float ey, uint32_t color) : Linelet(ex, ey, static_cast<int32_t>(color)) {}

WarGrey::STEM::Linelet::Linelet(float ex, float ey, double hue, double saturation, double brightness)
    : Linelet(ex, ey, Hexadecimal_From_HSV(hue, saturation, brightness)) {}

void WarGrey::STEM::Linelet::on_resize(float w, float h, float width, float height) {
    this->epx *= w / width;
    this->epy *= h / height;
    this->invalidate_geometry();
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
WarGrey::STEM::Rectanglet::Rectanglet(float edge_size, int32_t color, int32_t border_color)
	: Rectanglet(edge_size, edge_size, color, border_color) {}

WarGrey::STEM::Rectanglet::Rectanglet(float edge_size, uint32_t color, int32_t border_color)
	: Rectanglet(edge_size, static_cast<int32_t>(color), border_color) {}

WarGrey::STEM::Rectanglet::Rectanglet(float edge_size, double hue, double saturation, double brightness, int32_t border_color)
    : Rectanglet(edge_size, Hexadecimal_From_HSV(hue, saturation, brightness), border_color) {}

WarGrey::STEM::Rectanglet::Rectanglet(float width, float height, int32_t color, int32_t border_color)
	: IShapelet(color, border_color), width(width), height(height) {}

WarGrey::STEM::Rectanglet::Rectanglet(float width, float height, uint32_t color, int32_t border_color)
	: Rectanglet(width, height, static_cast<int>(color), border_color) {}

WarGrey::STEM::Rectanglet::Rectanglet(float width, float height, double hue, double saturation, double brightness, int32_t border_color)
    : Rectanglet(width, height, Hexadecimal_From_HSV(hue, saturation, brightness), border_color) {}

void WarGrey::STEM::Rectanglet::on_resize(float w, float h, float width, float height) {
    this->width = w;
    this->height = h;
    this->invalidate_geometry();
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
WarGrey::STEM::RoundedRectanglet::RoundedRectanglet(float edge_size, float radius, int32_t color, int32_t border_color)
	: RoundedRectanglet(edge_size, edge_size, radius, color, border_color) {}

WarGrey::STEM::RoundedRectanglet::RoundedRectanglet(float edge_size, float radius, uint32_t color, int32_t border_color)
	: RoundedRectanglet(edge_size, radius, static_cast<int32_t>(color), border_color) {}

WarGrey::STEM::RoundedRectanglet::RoundedRectanglet(float edge_size, float radius, double hue, double saturation, double brightness, int32_t border_color)
    : RoundedRectanglet(edge_size, radius, Hexadecimal_From_HSV(hue, saturation, brightness), border_color) {}

WarGrey::STEM::RoundedRectanglet::RoundedRectanglet(float width, float height, float radius, int32_t color, int32_t border_color)
	: IShapelet(color, border_color), width(width), height(height), radius(radius) {}

WarGrey::STEM::RoundedRectanglet::RoundedRectanglet(float width, float height, float radius, uint32_t color, int32_t border_color)
	: RoundedRectanglet(width, height, radius, static_cast<int32_t>(color), border_color) {}

WarGrey::STEM::RoundedRectanglet::RoundedRectanglet(float width, float height, float radius, double hue, double saturation, double brightness, int32_t border_color)
    : RoundedRectanglet(width, height, radius, Hexadecimal_From_HSV(hue, saturation, brightness), border_color) {}

void WarGrey::STEM::RoundedRectanglet::on_resize(float w, float h, float width, float height) {
    this->width = w;
    this->height = h;
    this->invalidate_geometry();
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
WarGrey::STEM::Ellipselet::Ellipselet(float radius, int32_t color, int32_t border_color)
	: Ellipselet(radius, radius, color, border_color) {}

WarGrey::STEM::Ellipselet::Ellipselet(float radius, uint32_t color, int32_t border_color)
	: Ellipselet(radius, radius, static_cast<int32_t>(color), border_color) {}

WarGrey::STEM::Ellipselet::Ellipselet(float radius, double hue, double saturation, double brightness, int32_t border_color)
    : Ellipselet(radius, Hexadecimal_From_HSV(hue, saturation, brightness), border_color) {}

WarGrey::STEM::Ellipselet::Ellipselet(float a, float b, int32_t color, int32_t border_color)
	: IShapelet(color, border_color), aradius(a), bradius(b) {}

WarGrey::STEM::Ellipselet::Ellipselet(float a, float b, uint32_t color, int32_t border_color)
	: Ellipselet(a, b, static_cast<int32_t>(color), border_color) {}

WarGrey::STEM::Ellipselet::Ellipselet(float aradius, float bradius, double hue, double saturation, double brightness, int32_t border_color)
    : Ellipselet(aradius, bradius, Hexadecimal_From_HSV(hue, saturation, brightness), border_color) {}

void WarGrey::STEM::Ellipselet::on_resize(float w, float h, float width, float height) {
    this->aradius = w * 0.5F;
    this->bradius = h * 0.5F;
    this->invalidate_geometry();
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
WarGrey::STEM::Trianglet::Trianglet(float x2, float y2, float x3, float y3, int32_t color, int32_t border_color)
	: IShapelet(color, border_color), x2(x2), y2(y2), x3(x3), y3(y3) {}

WarGrey::STEM::Trianglet::Trianglet(float x2, float y2, float x3, float y3, uint32_t color, int32_t border_color)
	: Trianglet(x2, y2, x3, y3, static_cast<int32_t>(color), border_color) {}

WarGrey::STEM::Trianglet::Trianglet(float x2, float y2, float x3, float y3, double hue, double saturation, double brightness, int32_t border_color)
    : Trianglet(x2, y2, x3, y3, Hexadecimal_From_HSV(hue, saturation, brightness), border_color) {}

void WarGrey::STEM::Trianglet::on_resize(float w, float h, float width, float height) {
    float xratio = w / width;
    float yratio = h / height;

    this->x2 *= xratio;
    this->y2 *= yratio;
    this->x3 *= xratio;
    this->y3 *= yratio;

    this->invalidate_geometry();
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
WarGrey::STEM::RegularPolygonlet::RegularPolygonlet(int n, float radius, int32_t color, int32_t border_color)
	: RegularPolygonlet(n, radius, 0.0F, color, border_color) {}

WarGrey::STEM::RegularPolygonlet::RegularPolygonlet(int n, float radius, uint32_t color, int32_t border_color)
	: RegularPolygonlet(n, radius, static_cast<int32_t>(color), border_color) {}

WarGrey::STEM::RegularPolygonlet::RegularPolygonlet(int n, float radius, double hue, double saturation, double brightness, int32_t border_color)
    : RegularPolygonlet(n, radius, Hexadecimal_From_HSV(hue, saturation, brightness), border_color) {}

WarGrey::STEM::RegularPolygonlet::RegularPolygonlet(int n, float radius, float rotation, int32_t color, int32_t border_color)
	: IShapelet(color, border_color), n(n), aradius(radius), bradius(radius), rotation(rotation) {
    this->initialize_vertice();
}

WarGrey::STEM::RegularPolygonlet::RegularPolygonlet(int n, float radius, float rotation, uint32_t color, int32_t border_color)
	: RegularPolygonlet(n, radius, rotation, static_cast<int32_t>(color), border_color) {}

WarGrey::STEM::RegularPolygonlet::RegularPolygonlet(int n, float radius, float rotation, double hue, double saturation, double brightness, int32_t border_color)
    : RegularPolygonlet(n, radius, rotation, Hexadecimal_From_HSV(hue, saturation, brightness), border_color) {}

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
        this->ys = new float[this->n];
        this->txs = new short[this->n];
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

    for (int idx = 0; idx < this->n; idx ++) {
        this->txs[idx] = fl2fx<short>(this->xs[idx] - this->lx);
        this->tys[idx] = fl2fx<short>(this->ys[idx] - this->ty);
    }
}

void WarGrey::STEM::RegularPolygonlet::on_resize(float w, float h, float width, float height) {
    this->aradius *= (w / width);
    this->bradius *= (h / height);
    this->initialize_vertice();
    this->invalidate_geometry();
}

void WarGrey::STEM::RegularPolygonlet::feed_extent(float x, float y, float* w, float* h) {
    SET_VALUES(w, this->rx - this->lx + 1.0F, h, this->by - this->ty + 1.0F);
}

void WarGrey::STEM::RegularPolygonlet::draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    aapolygonRGBA(renderer, this->txs, this->tys, this->n, r, g, b, a);
}

void WarGrey::STEM::RegularPolygonlet::fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    filledPolygonRGBA(renderer, this->txs, this->tys, this->n, r, g, b, a);
    aapolygonRGBA(renderer, this->txs, this->tys, this->n, r, g, b, a);
}
