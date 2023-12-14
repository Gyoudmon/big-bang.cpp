#include "brush.hpp"
#include "colorspace.hpp"

#include "../physics/mathematics.hpp"

#include "../datum/flonum.hpp"
#include "../datum/fixnum.hpp"
#include "../datum/box.hpp"

#include <SDL2/SDL2_gfxPrimitives.h>

using namespace WarGrey::STEM;

/**************************************************************************************************/
#define FILL_BOX(box, px, py, width, height) { box.x = px; box.y = py; box.w = width; box.h = height; }

static void pen_draw_circle(SDL_Renderer* renderer, int cx, int cy, int radius) {
    int err = 2 - 2 * radius;
    int x = -radius;
    int y = 0;
    
    do {
        SDL_RenderDrawPoint(renderer, cx + x, cy - y);
        SDL_RenderDrawPoint(renderer, cx - x, cy + y);
        SDL_RenderDrawPoint(renderer, cx + y, cy + x);
        SDL_RenderDrawPoint(renderer, cx - y, cy - x);

        radius = err;
        if (radius <= y) {
            err += ++y * 2 + 1;
        }

        if ((radius > x) || (err > y)) {
            err += ++x * 2 ;
        }
    } while (x < 0);
}

static void pen_draw_filled_circle(SDL_Renderer* renderer, int cx, int cy, int radius) {
    int err = 2 - 2 * radius;
    int x = -radius;
    int y = 0;
    
    do {
        SDL_RenderDrawLine(renderer, cx + x, cy + y, cx - x, cy + y); // Q III, Q IV
        SDL_RenderDrawLine(renderer, cx + x, cy,     cx + x, cy - y); // Q II
        SDL_RenderDrawLine(renderer, cx - x, cy - y, cx,     cy - y); // Q I

        radius = err;
        if (radius <= y) {
            err += ++y * 2 + 1;
        }

        if ((radius > x) || (err > y)) {
            err += ++x * 2 + 1;
        }
    } while (x < 0);
}

static void pen_draw_ellipse(SDL_Renderer* renderer, int cx, int cy, int ar, int br) {
    /* II. quadrant from bottom left to top right */
    long x = -ar;
    long y = 0;
    long a2 = ar * ar;
    long b2 = br * br;
    long e2 = br;
    long dx = (1 + 2 * x) * e2 * e2;
    long dy = x * x;
    long err = dx + dy;

    do {
        SDL_RenderDrawPoint(renderer, cx - x, cy + y);
        SDL_RenderDrawPoint(renderer, cx + x, cy + y);
        SDL_RenderDrawPoint(renderer, cx + x, cy - y);
        SDL_RenderDrawPoint(renderer, cx - x, cy - y);

        e2 = 2 * err;
        if (e2 >= dx) { x++; err += dx += 2 * b2; }    /* x step */
        if (e2 <= dy) { y++; err += dy += 2 * a2; }    /* y step */
    } while (x <= 0);

    /* to early stop for flat ellipses with a = 1, finish tip of ellipse */
    while (y++ < br) {
        SDL_RenderDrawPoint(renderer, cx, cy + y);
        SDL_RenderDrawPoint(renderer, cx, cy - y);
    }
}

static void pen_draw_filled_ellipse(SDL_Renderer* renderer, int cx, int cy, int ar, int br) {
    /* Q II. from bottom left to top right */
    long x = -ar;
    long y = 0;
    long a2 = ar * ar;
    long b2 = br * br;
    long e2 = br;
    long dx = (1 + 2 * x) * e2 * e2;
    long dy = x * x;
    long err = dx + dy;

    do {
        SDL_RenderDrawLine(renderer, cx + x, cy + y, cx - x, cy + y); // Q III, Q IV
        SDL_RenderDrawLine(renderer, cx + x, cy,     cx + x, cy - y); // Q II
        SDL_RenderDrawLine(renderer, cx - x, cy - y, cx,     cy - y); // Q I

        e2 = 2 * err;
        if (e2 >= dx) { x++; err += dx += 2 * b2; }     /* x step */
        if (e2 <= dy) { y++; err += dy += 2 * a2; }     /* y step */
    } while (x <= 0);

    /* to early stop for flat ellipses with a = 1, finish tip of ellipse */
    while (y++ < br) {
        SDL_RenderDrawLine(renderer, cx, cy + y, cx, cy - y);
    }
}

static void pen_draw_regular_polygon(SDL_Renderer* renderer, int n, float cx, float cy, float r, float rotation) {
    // for inscribed regular polygon, the radius should be `Rcos(pi/n)`
    float start = degrees_to_radians(rotation);
    float delta = 2.0F * pi_f / float(n);
    float x0, y0, px, py;

    x0 = px = r * flcos(start) + cx;
    y0 = py = r * flsin(start) + cy;

    for (int idx = 1; idx < n; idx++) {
        float theta = start + delta * float(idx);
        float sx = r * flcos(theta) + cx;
        float sy = r * flsin(theta) + cy;

        SDL_RenderDrawLineF(renderer, px, py, sx, sy);
        px = sx;
        py = sy;
    }

    if (px != x0) {
        SDL_RenderDrawLineF(renderer, px, py, x0, y0);
    } else {
        SDL_RenderDrawPointF(renderer, cx, cy);
    }
}

static void pen_fill_regular_polygon(SDL_Renderer* renderer, int n, float cx, float cy, float r, float rotation) {
    // for inscribed regular polygon, the radius should be `Rcos(pi/n)`
    float start = degrees_to_radians(rotation);
    float delta = 2.0F * pi_f / float(n);
    float xmin = cx - r;
    float xmax = cx + r;
    float ymin = +r + cy;
    float ymax = -r + cy;

#ifndef __windows__
    SDL_FPoint pts[n + 1];
#else
    SDL_FPoint* pts = new SDL_FPoint[n + 1];
#endif

    for (int idx = 0; idx < n; idx ++) {
        float theta = start + delta * float(idx);
        float sx = r * flcos(theta) + cx;
        float sy = r * flsin(theta) + cy;

        pts[idx].x = sx;
        pts[idx].y = sy;

        if (sy < ymin) ymin = sy;
        if (sy > ymax) ymax = sy;
    }
    
    pts[n] = pts[0];

    for (float y = ymin; y < ymax + 1.0; y += 1.0) {
        int pcount = 0;
        float px[2];
        float py, t;

        for (int i = 0; i < n / 2; i ++) {
            SDL_FPoint spt = pts[i];
            SDL_FPoint ept = pts[i + 1];

            if (lines_intersect(spt.x, spt.y, ept.x, ept.y, xmin, y, xmax, y, px + pcount, &py, &t)) {
                if (flin(0.0, t, 1.0)) pcount += 1;
            } else if (pcount == 0) {
                px[0] = spt.x;
                px[1] = ept.x;
                pcount = 2;
            }
            
            if (pcount == 2) break;

            spt = pts[n - i];
            ept = pts[n - i - 1];

            if (lines_intersect(spt.x, spt.y, ept.x, ept.y, xmin, y, xmax, y, px + pcount, &py, &t)) {
                if (flin(0.0, t, 1.0)) pcount += 1;
            } else if (pcount == 0) {
                px[0] = spt.x;
                px[1] = ept.x;
                pcount = 2;
            }
            
            if (pcount == 2) break;
        }

        if (pcount == 2) {
            SDL_RenderDrawLineF(renderer, px[0], y, px[1], y);
        } else if (n == 2) {
            SDL_RenderDrawPointF(renderer, px[0], py);
        } else if (n <= 1) {
            SDL_RenderDrawPointF(renderer, cx, cy);
        }
    }

#ifdef __windows__
    delete [] pts;
#endif
}

/*************************************************************************************************/
void WarGrey::STEM::Brush::draw_frame(SDL_Renderer* renderer, int x, int y, int width, int height) {
    SDL_Rect box;

    FILL_BOX(box, x - 1, y - 1, width + 3, height + 3);
    SDL_RenderDrawRect(renderer, &box);
}

void WarGrey::STEM::Brush::clear(SDL_Renderer* renderer, const RGBA& color) {
    SDL_SetRenderDrawColor(renderer, color.R(), color.G(), color.B(), color.A());
    SDL_RenderClear(renderer);
}

void WarGrey::STEM::Brush::draw_grid(SDL_Renderer* renderer, int row, int col, int cell_width, int cell_height, int xoff, int yoff) {
    int xend = xoff + col * cell_width;
    int yend = yoff + row * cell_height;

    for (int c = 0; c <= col; c++) {
        int x = xoff + c * cell_width;

        for (int r = 0; r <= row; r++) {
            int y = yoff + r * cell_height;

            SDL_RenderDrawLine(renderer, xoff, y, xend, y);
        }

        SDL_RenderDrawLine(renderer, x, yoff, x, yend);
    }
}

void WarGrey::STEM::Brush::fill_grid(SDL_Renderer* renderer, int* grids[], int row, int col, int cell_width, int cell_height, int xoff, int yoff) {
    SDL_Rect cell_self;

    cell_self.w = cell_width;
    cell_self.h = cell_height;

    for (int c = 0; c < col; c++) {
        for (int r = 0; r < row; r++) {
            if (grids[r][c] > 0) {
                cell_self.x = xoff + c * cell_self.w;
                cell_self.y = yoff + r * cell_self.h;
                SDL_RenderFillRect(renderer, &cell_self);
            }
        }
    }
}

void WarGrey::STEM::Brush::stamp(SDL_Renderer* target, SDL_Surface* surface, int x, int y, SDL_RendererFlip flip, double angle) {
    Brush::stamp(target, surface, x, y, surface->w, surface->h, flip, angle);
}

void WarGrey::STEM::Brush::stamp(SDL_Renderer* target, SDL_Surface* surface, int x, int y, int width, int height, SDL_RendererFlip flip, double angle) {
    SDL_Rect box;

    FILL_BOX(box, x, y, width, height);
    Brush::stamp(target, surface, &box, flip, angle);
}

void WarGrey::STEM::Brush::stamp(SDL_Renderer* target, SDL_Surface* surface, SDL_Rect* region, SDL_RendererFlip flip, double angle) {
    Brush::stamp(target, surface, nullptr, region, flip, angle);
}

void WarGrey::STEM::Brush::stamp(SDL_Renderer* target, SDL_Surface* surface, SDL_Rect* src, SDL_Rect* dst, SDL_RendererFlip flip, double angle) {
    SDL_Texture* texture = SDL_CreateTextureFromSurface(target, surface);

    if (texture != nullptr) {
        Brush::stamp(target, texture, src, dst, flip, angle);
        SDL_DestroyTexture(texture);
    }
}

int WarGrey::STEM::Brush::stamp(SDL_Renderer* target, SDL_Texture* texture, int x, int y, SDL_RendererFlip flip, double angle) {
    int width, height;

    SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
    
    return Brush::stamp(target, texture, x, y, width, height, flip, angle);
}

int WarGrey::STEM::Brush::stamp(SDL_Renderer* target, SDL_Texture* texture, int x, int y, int width, int height, SDL_RendererFlip flip, double angle) {
    SDL_Rect box;
    
    FILL_BOX(box, x, y, width, height);
    
    return Brush::stamp(target, texture, &box, flip, angle);
}

int WarGrey::STEM::Brush::stamp(SDL_Renderer* target, SDL_Texture* texture, SDL_Rect* region, SDL_RendererFlip flip, double angle) {
    return Brush::stamp(target, texture, nullptr, region, flip, angle);
}

int WarGrey::STEM::Brush::stamp(SDL_Renderer* target, SDL_Texture* texture, SDL_Rect* src, SDL_Rect* dst, SDL_RendererFlip flip, double angle) {
    if ((flip == SDL_FLIP_NONE) && (angle == 0.0)) {
        return SDL_RenderCopy(target, texture, src, dst);
    } else {
        return SDL_RenderCopyEx(target, texture, src, dst, angle, nullptr, flip);
    }
}

/**************************************************************************************************/
void WarGrey::STEM::Brush::draw_point(SDL_Renderer* renderer, int x, int y, const RGBA& color) {
    SDL_SetRenderDrawColor(renderer, color.R(), color.G(), color.B(), color.A());
    SDL_RenderDrawPoint(renderer, x, y);
}

void WarGrey::STEM::Brush::draw_line(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, const RGBA& color) {
    SDL_SetRenderDrawColor(renderer, color.R(), color.G(), color.B(), color.A());
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void WarGrey::STEM::Brush::draw_hline(SDL_Renderer* renderer, int x, int y, int length, const RGBA& color) {
    SDL_SetRenderDrawColor(renderer, color.R(), color.G(), color.B(), color.A());
    SDL_RenderDrawLine(renderer, x, y, x + length, y);
}

void WarGrey::STEM::Brush::draw_vline(SDL_Renderer* renderer, int x, int y, int length, const RGBA& color) {
    SDL_SetRenderDrawColor(renderer, color.R(), color.G(), color.B(), color.A());
    SDL_RenderDrawLine(renderer, x, y, x, y + length);
}

void WarGrey::STEM::Brush::draw_rect(SDL_Renderer* renderer, SDL_Rect* box, const RGBA& color) {
    SDL_SetRenderDrawColor(renderer, color.R(), color.G(), color.B(), color.A());
    SDL_RenderDrawRect(renderer, box);
}

void WarGrey::STEM::Brush::fill_rect(SDL_Renderer* renderer, SDL_Rect* box, const RGBA& color) {
    SDL_SetRenderDrawColor(renderer, color.R(), color.G(), color.B(), color.A());
    SDL_RenderFillRect(renderer, box);
}

void WarGrey::STEM::Brush::draw_rect(SDL_Renderer* renderer, int x, int y, int width, int height, const RGBA& color) {
    SDL_Rect box;

    FILL_BOX(box, x, y, width, height);
    Brush::draw_rect(renderer, &box, color);
}

void WarGrey::STEM::Brush::fill_rect(SDL_Renderer* renderer, int x, int y, int width, int height, const RGBA& color) {
    SDL_Rect box;

    FILL_BOX(box, x, y, width, height);
    Brush::fill_rect(renderer, &box, color);
}

void WarGrey::STEM::Brush::draw_rounded_rect(SDL_Renderer* renderer, SDL_Rect* box, float rad, const RGBA& color) {
    Brush::draw_rounded_rect(renderer, box->x, box->y, box->w, box->h, rad, color);
}

void WarGrey::STEM::Brush::fill_rounded_rect(SDL_Renderer* renderer, SDL_Rect* box, float rad, const RGBA& color) {
    Brush::fill_rounded_rect(renderer, box->x, box->y, box->w, box->h, rad, color);
}

void WarGrey::STEM::Brush::draw_rounded_rect(SDL_Renderer* renderer, int x, int y, int width, int height, float radius, const RGBA& color) {
    Brush::draw_rounded_rect(renderer, float(x), float(y), float(width), float(height), radius, color);
}

void WarGrey::STEM::Brush::fill_rounded_rect(SDL_Renderer* renderer, int x, int y, int width, int height, float radius, const RGBA& color) {
    Brush::fill_rounded_rect(renderer, float(x), float(y), float(width), float(height), radius, color);
}

void WarGrey::STEM::Brush::draw_square(SDL_Renderer* renderer, int cx, int cy, int apothem, const RGBA& color) {
    Brush::draw_rect(renderer, cx - apothem, cy - apothem, apothem * 2, apothem * 2, color);
}

void WarGrey::STEM::Brush::fill_square(SDL_Renderer* renderer, int cx, int cy, int apothem, const RGBA& color) {
    Brush::fill_rect(renderer, cx - apothem, cy - apothem, apothem * 2, apothem * 2, color);
}

void WarGrey::STEM::Brush::draw_rounded_square(SDL_Renderer* renderer, int cx, int cy, int apothem, float rad, const RGBA& color) {
    Brush::draw_rounded_rect(renderer, cx - apothem, cy - apothem, apothem * 2, apothem * 2, rad, color);
}

void WarGrey::STEM::Brush::fill_rounded_square(SDL_Renderer* renderer, int cx, int cy, int apothem, float rad, const RGBA& color) {
    Brush::fill_rounded_rect(renderer, cx - apothem, cy - apothem, apothem * 2, apothem * 2, rad, color);
}

void WarGrey::STEM::Brush::draw_circle(SDL_Renderer* renderer, int cx, int cy, int radius, const RGBA& color) {
    SDL_SetRenderDrawColor(renderer, color.R(), color.G(), color.B(), color.A());
    pen_draw_circle(renderer, cx, cy, radius);
}

void WarGrey::STEM::Brush::fill_circle(SDL_Renderer* renderer, int cx, int cy, int radius, const RGBA& color) {
    SDL_SetRenderDrawColor(renderer, color.R(), color.G(), color.B(), color.A());
    pen_draw_filled_circle(renderer, cx, cy, radius);
}

void WarGrey::STEM::Brush::draw_ellipse(SDL_Renderer* renderer, int cx, int cy, int ar, int br, const RGBA& color) {
    SDL_SetRenderDrawColor(renderer, color.R(), color.G(), color.B(), color.A());
    pen_draw_ellipse(renderer, cx, cy, ar, br);
}

void WarGrey::STEM::Brush::fill_ellipse(SDL_Renderer* renderer, int cx, int cy, int ar, int br, const RGBA& color) {
    SDL_SetRenderDrawColor(renderer, color.R(), color.G(), color.B(), color.A());
    pen_draw_filled_ellipse(renderer, cx, cy, ar, br);
}

void WarGrey::STEM::Brush::draw_regular_polygon(SDL_Renderer* renderer, int n, int cx, int cy, int radius, float rotation, const RGBA& color) {
    Brush::draw_regular_polygon(renderer, n, float(cx), float(cy), float(radius), rotation, color);
}

void WarGrey::STEM::Brush::fill_regular_polygon(SDL_Renderer* renderer, int n, int cx, int cy, int radius, float rotation, const RGBA& color) {
    Brush::fill_regular_polygon(renderer, n, float(cx), float(cy), float(radius), rotation, color);
}

/*************************************************************************************************/
void WarGrey::STEM::Brush::draw_frame(SDL_Renderer* renderer, float x, float y, float width, float height) {
    SDL_FRect box;

    FILL_BOX(box, x - 1.0F, y - 1.0F, width + 3.0F, height + 3.0F);
    SDL_RenderDrawRectF(renderer, &box);
}

void WarGrey::STEM::Brush::draw_grid(SDL_Renderer* renderer, int row, int col, float cell_width, float cell_height, float xoff, float yoff) {
    float xend = xoff + col * cell_width;
    float yend = yoff + row * cell_height;

    for (int c = 0; c <= col; c++) {
        float x = xoff + float(c) * cell_width;

        for (int r = 0; r <= row; r++) {
            float y = yoff + float(r) * cell_height;

            SDL_RenderDrawLineF(renderer, xoff, y, xend, y);
        }

        SDL_RenderDrawLineF(renderer, x, yoff, x, yend);
    }
}

void WarGrey::STEM::Brush::fill_grid(SDL_Renderer* renderer, int* grids[], int row, int col, float cell_width, float cell_height, float xoff, float yoff) {
    SDL_FRect cell_self;

    cell_self.w = cell_width;
    cell_self.h = cell_height;

    for (int c = 0; c < col; c++) {
        for (int r = 0; r < row; r++) {
            if (grids[r][c] > 0) {
                cell_self.x = xoff + float(c) * cell_self.w;
                cell_self.y = yoff + float(r) * cell_self.h;
                SDL_RenderFillRectF(renderer, &cell_self);
            }
        }
    }
}

void WarGrey::STEM::Brush::stamp(SDL_Renderer* target, SDL_Surface* surface, float x, float y, SDL_RendererFlip flip, double angle) {
    Brush::stamp(target, surface, x, y, float(surface->w), float(surface->h), flip, angle);
}

void WarGrey::STEM::Brush::stamp(SDL_Renderer* target, SDL_Surface* surface, float x, float y, float width, float height, SDL_RendererFlip flip, double angle) {
    SDL_FRect box;

    FILL_BOX(box, x, y, width, height);
    Brush::stamp(target, surface, &box, flip, angle);
}

void WarGrey::STEM::Brush::stamp(SDL_Renderer* target, SDL_Surface* surface, SDL_FRect* region, SDL_RendererFlip flip, double angle) {
    Brush::stamp(target, surface, nullptr, region, flip, angle);
}

void WarGrey::STEM::Brush::stamp(SDL_Renderer* target, SDL_Surface* surface, SDL_Rect* src, SDL_FRect* dst, SDL_RendererFlip flip, double angle) {
    SDL_Texture* texture = SDL_CreateTextureFromSurface(target, surface);

    if (texture != nullptr) {
        Brush::stamp(target, texture, src, dst, flip, angle);
        SDL_DestroyTexture(texture);
    }
}

int WarGrey::STEM::Brush::stamp(SDL_Renderer* target, SDL_Texture* texture, float x, float y, SDL_RendererFlip flip, double angle) {
    int width, height;

    SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
    
    return Brush::stamp(target, texture, x, y, float(width), float(height), flip, angle);
}

int WarGrey::STEM::Brush::stamp(SDL_Renderer* target, SDL_Texture* texture, float x, float y, float width, float height, SDL_RendererFlip flip, double angle) {
    SDL_FRect box;
    
    FILL_BOX(box, x, y, width, height);
    
    return Brush::stamp(target, texture, &box, flip, angle);
}

int WarGrey::STEM::Brush::stamp(SDL_Renderer* target, SDL_Texture* texture, SDL_FRect* region, SDL_RendererFlip flip, double angle) {
    return Brush::stamp(target, texture, nullptr, region, flip, angle);
}

int WarGrey::STEM::Brush::stamp(SDL_Renderer* target, SDL_Texture* texture, SDL_Rect* src, SDL_FRect* dst, SDL_RendererFlip flip, double angle) {
    if ((flip == SDL_FLIP_NONE) && (angle == 0.0)) {
        return SDL_RenderCopyF(target, texture, src, dst);
    } else {
        return SDL_RenderCopyExF(target, texture, src, dst, angle, nullptr, flip);
    }
}

/**************************************************************************************************/
void WarGrey::STEM::Brush::draw_point(SDL_Renderer* renderer, float x, float y, const RGBA& color) {
    SDL_SetRenderDrawColor(renderer, color.R(), color.G(), color.B(), color.A());
    SDL_RenderDrawPointF(renderer, x, y);
}

void WarGrey::STEM::Brush::draw_line(SDL_Renderer* renderer, float x1, float y1, float x2, float y2, const RGBA& color) {
    aalineRGBA(renderer, fl2fx<int16_t>(x1), fl2fx<int16_t>(y1), fl2fx<int16_t>(x2), fl2fx<int16_t>(y2),
                color.R(), color.G(), color.B(), color.A());
}

void WarGrey::STEM::Brush::draw_hline(SDL_Renderer* renderer, float x, float y, float length, const RGBA& color) {
    Brush::draw_hline(renderer, fl2fx<int>(x), fl2fx<int>(y), fl2fx<int>(length), color);
}

void WarGrey::STEM::Brush::draw_vline(SDL_Renderer* renderer, float x, float y, float length, const RGBA& color) {
    Brush::draw_vline(renderer, fl2fx<int>(x), fl2fx<int>(y), fl2fx<int>(length), color);
}

void WarGrey::STEM::Brush::draw_points(SDL_Renderer* renderer, const SDL_FPoint* pts, int size, const RGBA& color) {
    SDL_SetRenderDrawColor(renderer, color.R(), color.G(), color.B(), color.A());
    SDL_RenderDrawPointsF(renderer, pts, size);
}

void WarGrey::STEM::Brush::draw_lines(SDL_Renderer* renderer, const SDL_FPoint* pts, int size, const RGBA& color) {
    SDL_SetRenderDrawColor(renderer, color.R(), color.G(), color.B(), color.A());
    SDL_RenderDrawLinesF(renderer, pts, size);
}

void WarGrey::STEM::Brush::draw_rect(SDL_Renderer* renderer, SDL_FRect* box, const RGBA& color) {
    SDL_SetRenderDrawColor(renderer, color.R(), color.G(), color.B(), color.A());
    SDL_RenderDrawRectF(renderer, box);
}

void WarGrey::STEM::Brush::fill_rect(SDL_Renderer* renderer, SDL_FRect* box, const RGBA& color) {
    SDL_SetRenderDrawColor(renderer, color.R(), color.G(), color.B(), color.A());
    SDL_RenderFillRectF(renderer, box);
}

void WarGrey::STEM::Brush::draw_rect(SDL_Renderer* renderer, float x, float y, float width, float height, const RGBA& color) {
    SDL_FRect box;

    FILL_BOX(box, x, y, width, height);
    Brush::draw_rect(renderer, &box, color);
}

void WarGrey::STEM::Brush::fill_rect(SDL_Renderer* renderer, float x, float y, float width, float height, const RGBA& color) {
    SDL_FRect box;

    FILL_BOX(box, x, y, width, height);
    Brush::fill_rect(renderer, &box, color);
}

void WarGrey::STEM::Brush::draw_rounded_rect(SDL_Renderer* renderer, SDL_FRect* box, float rad, const RGBA& color) {
    Brush::draw_rounded_rect(renderer, box->x, box->y, box->w, box->h, rad, color);
}

void WarGrey::STEM::Brush::fill_rounded_rect(SDL_Renderer* renderer, SDL_FRect* box, float rad, const RGBA& color) {
    Brush::fill_rounded_rect(renderer, box->x, box->y, box->w, box->h, rad, color);
}

void WarGrey::STEM::Brush::draw_rounded_rect(SDL_Renderer* renderer, float x, float y, float width, float height, float radius, const RGBA& color) {
    int16_t X1 = fl2fx<int16_t>(x);
    int16_t Y1 = fl2fx<int16_t>(y);
    int16_t X2 = fl2fx<int16_t>(x + width);
    int16_t Y2 = fl2fx<int16_t>(y + height);
    int16_t rad;

    if (radius < 0.0) {
        radius = fxmin(width, height) * (-radius);
    }

    rad = fl2fx<int16_t>(flmin(radius, height * 0.5F));
    roundedRectangleRGBA(renderer, X1, Y1, X2, Y2, rad, color.R(), color.G(), color.B(), color.A());
}

void WarGrey::STEM::Brush::fill_rounded_rect(SDL_Renderer* renderer, float x, float y, float width, float height, float radius, const RGBA& color) {
    int16_t X1 = fl2fx<int16_t>(x);
    int16_t Y1 = fl2fx<int16_t>(y);
    int16_t X2 = fl2fx<int16_t>(x + width);
    int16_t Y2 = fl2fx<int16_t>(y + height);
    int16_t rad;

    if (radius < 0.0) {
        radius = fxmin(width, height) * (-radius);
    }

    rad = fl2fx<int16_t>(flmin(radius, height * 0.5F));
    roundedBoxRGBA(renderer, X1, Y1, X2, Y2, rad, color.R(), color.G(), color.B(), color.A());
}

void WarGrey::STEM::Brush::draw_square(SDL_Renderer* renderer, float cx, float cy, float apothem, const RGBA& color) {
    Brush::draw_rect(renderer, cx - apothem, cy - apothem, apothem * 2.0F, apothem * 2.0F, color);
}

void WarGrey::STEM::Brush::fill_square(SDL_Renderer* renderer, float cx, float cy, float apothem, const RGBA& color) {
    Brush::fill_rect(renderer, cx - apothem, cy - apothem, apothem * 2.0F, apothem * 2.0F, color);
}

void WarGrey::STEM::Brush::draw_rounded_square(SDL_Renderer* renderer, float cx, float cy, float apothem, float rad, const RGBA& color) {
    Brush::draw_rounded_rect(renderer, cx - apothem, cy - apothem, apothem * 2.0F, apothem * 2.0F, rad, color);
}

void WarGrey::STEM::Brush::fill_rounded_square(SDL_Renderer* renderer, float cx, float cy, float apothem, float rad, const RGBA& color) {
    Brush::fill_rounded_rect(renderer, cx - apothem, cy - apothem, apothem * 2.0F, apothem * 2.0F, rad, color);
}

void WarGrey::STEM::Brush::draw_circle(SDL_Renderer* renderer, float cx, float cy, float radius, const RGBA& color) {
    int16_t CX = fl2fx<int16_t>(cx);
    int16_t CY = fl2fx<int16_t>(cy);
    int16_t R = fl2fx<int16_t>(radius);
    
    aacircleRGBA(renderer, CX, CY, R, color.R(), color.G(), color.B(), color.A());
}

void WarGrey::STEM::Brush::fill_circle(SDL_Renderer* renderer, float cx, float cy, float radius, const RGBA& color) {
    int16_t CX = fl2fx<int16_t>(cx);
    int16_t CY = fl2fx<int16_t>(cy);
    int16_t R = fl2fx<int16_t>(radius);
    uint8_t r = color.R();
    uint8_t g = color.G();
    uint8_t b = color.B();
    uint8_t a = color.A();
    
    filledCircleRGBA(renderer, CX, CY, R, r, g, b, a);
    aacircleRGBA(renderer, CX, CY, R, r, g, b, a);
}

void WarGrey::STEM::Brush::draw_ellipse(SDL_Renderer* renderer, float cx, float cy, float ar, float br, const RGBA& color) {
    int16_t CX = fl2fx<int16_t>(cx);
    int16_t CY = fl2fx<int16_t>(cy);
    int16_t AR = fl2fx<int16_t>(ar);
    int16_t BR = fl2fx<int16_t>(br);
    uint8_t r = color.R();
    uint8_t g = color.G();
    uint8_t b = color.B();
    uint8_t a = color.A();
    
    if (AR == BR) {
        aacircleRGBA(renderer, CX, CY, AR, r, g, b, a);
    } else {
        aaellipseRGBA(renderer, CX, CY, AR, BR, r, g, b, a);
    }
}

void WarGrey::STEM::Brush::fill_ellipse(SDL_Renderer* renderer, float cx, float cy, float ar, float br, const RGBA& color) {
    int16_t CX = fl2fx<int16_t>(cx);
    int16_t CY = fl2fx<int16_t>(cy);
    int16_t AR = fl2fx<int16_t>(ar);
    int16_t BR = fl2fx<int16_t>(br);
    uint8_t r = color.R();
    uint8_t g = color.G();
    uint8_t b = color.B();
    uint8_t a = color.A();
    
    if (AR == BR) {
        filledCircleRGBA(renderer, CX, CY, AR, r, g, b, a);
        aacircleRGBA(renderer, CX, CY, BR, r, g, b, a);
    } else {
        filledEllipseRGBA(renderer, CX, CY, AR, BR, r, g, b, a);
        aaellipseRGBA(renderer, CX, CY, AR, BR, r, g, b, a);
    }
}

void WarGrey::STEM::Brush::draw_regular_polygon(SDL_Renderer* renderer, int n, float cx, float cy, float radius, float rotation, const RGBA& color) {
    SDL_SetRenderDrawColor(renderer, color.R(), color.G(), color.B(), color.A());
    pen_draw_regular_polygon(renderer, n, cx, cy, radius, rotation);
}

void WarGrey::STEM::Brush::fill_regular_polygon(SDL_Renderer* renderer, int n, float cx, float cy, float radius, float rotation, const RGBA& color) {
    SDL_SetRenderDrawColor(renderer, color.R(), color.G(), color.B(), color.A());
    pen_fill_regular_polygon(renderer, n, cx, cy, radius, rotation);
}
