#include "geometry.hpp"
#include "colorspace.hpp"
#include "mathematics.hpp"

#include "datum/flonum.hpp"
#include "datum/fixnum.hpp"

using namespace WarGrey::STEM;

/**************************************************************************************************/
#define FILL_BOX(box, px, py, width, height) { box.x = px; box.y = py; box.w = width; box.h = height; }

static void draw_circle(SDL_Renderer* renderer, int cx, int cy, int radius) {
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

static void draw_filled_circle(SDL_Renderer* renderer, int cx, int cy, int radius) {
    int err = 2 - 2 * radius;
    int x = -radius;
    int y = 0;
    
    do {
        SDL_RenderDrawLine(renderer, cx + x, cy + y, cx - x, cy + y); // Q I, Q II
        SDL_RenderDrawLine(renderer, cx + x, cy,     cx + x, cy - y); // Q III
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

static void draw_ellipse(SDL_Renderer* renderer, int cx, int cy, int ar, int br) {
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

static void draw_filled_ellipse(SDL_Renderer* renderer, int cx, int cy, int ar, int br) {
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
        SDL_RenderDrawLine(renderer, cx + x, cy + y, cx - x, cy + y); // Q I, Q II
        SDL_RenderDrawLine(renderer, cx + x, cy,     cx + x, cy - y); // Q III
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

static void draw_regular_polygon(SDL_Renderer* renderer, int n, float cx, float cy, float r, float rotation) {
    // for inscribed regular polygon, the radius should be `Rcos(pi/n)`
    float start = degrees_to_radians(rotation);
    float delta = 2.0 * pi_f / float(n);
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

static void fill_regular_polygon(SDL_Renderer* renderer, int n, float cx, float cy, float r, float rotation) {
    // for inscribed regular polygon, the radius should be `Rcos(pi/n)`
    float start = degrees_to_radians(rotation);
    float delta = 2.0 * pi_f / float(n);
    float xmin = cx - r;
    float xmax = cx + r;
    float ymin = +r + cy;
    float ymax = -r + cy;
    SDL_FPoint pts[n + 1];

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

            if (lines_intersection(spt.x, spt.y, ept.x, ept.y, xmin, y, xmax, y, px + pcount, &py, &t)) {
                if (flin(0.0, t, 1.0)) pcount += 1;
            } else if (pcount == 0) {
                px[0] = spt.x;
                px[1] = ept.x;
                pcount = 2;
            }
            
            if (pcount == 2) break;

            spt = pts[n - i];
            ept = pts[n - i - 1];

            if (lines_intersection(spt.x, spt.y, ept.x, ept.y, xmin, y, xmax, y, px + pcount, &py, &t)) {
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
}

/*************************************************************************************************/
void WarGrey::STEM::game_draw_frame(SDL_Renderer* renderer, int x, int y, int width, int height) {
    SDL_Rect box;

    FILL_BOX(box, x - 1, y - 1, width + 3, height + 3);
    SDL_RenderDrawRect(renderer, &box);
}

void WarGrey::STEM::game_draw_grid(SDL_Renderer* renderer, int nx, int ny, int grid_size, int xoff, int yoff) {
    int xend = xoff + nx * grid_size;
    int yend = yoff + ny * grid_size;

    for (int i = 0; i <= nx; i++) {
        int x = xoff + i * grid_size;

        for (int j = 0; j <= ny; j++) {
            int y = yoff + j * grid_size;

            SDL_RenderDrawLine(renderer, xoff, y, xend, y);
        }

        SDL_RenderDrawLine(renderer, x, yoff, x, yend);
    }
}

void WarGrey::STEM::game_fill_grid(SDL_Renderer* renderer, int* grids[], int nx, int ny, int grid_size, int xoff, int yoff) {
    SDL_Rect grid_self;

    grid_self.w = grid_size;
    grid_self.h = grid_size;

    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            if (grids[i][j] > 0) {
                grid_self.x = xoff + i * grid_self.w;
                grid_self.y = yoff + j * grid_self.h;
                SDL_RenderFillRect(renderer, &grid_self);
            }
        }
    }
}

void WarGrey::STEM::game_render_surface(SDL_Renderer* target, SDL_Surface* surface, int x, int y) {
    SDL_Rect box;

    FILL_BOX(box, x, y, surface->w, surface->h);
    game_render_surface(target, surface, &box);
}

void WarGrey::STEM::game_render_surface(SDL_Renderer* target, SDL_Surface* surface, SDL_Rect* region) {
    SDL_Texture* texture = SDL_CreateTextureFromSurface(target, surface);

    if (texture != nullptr) {
        SDL_RenderCopy(target, texture, nullptr, region);
        SDL_DestroyTexture(texture);
    }
}

/**************************************************************************************************/
void WarGrey::STEM::game_draw_point(SDL_Renderer* renderer, int x, int y, uint32_t rgb, float alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    SDL_RenderDrawPoint(renderer, x, y);
}

void WarGrey::STEM::game_draw_point(SDL_Renderer* renderer, int x, int y, float hue, float saturation, float value, float alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    SDL_RenderDrawPoint(renderer, x, y);
}

void WarGrey::STEM::game_draw_line(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, uint32_t rgb, float alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void WarGrey::STEM::game_draw_line(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, float hue, float saturation, float value, float alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void WarGrey::STEM::game_draw_rect(SDL_Renderer* renderer, SDL_Rect* box, uint32_t rgb, float alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    SDL_RenderDrawRect(renderer, box);
}

void WarGrey::STEM::game_draw_rect(SDL_Renderer* renderer, SDL_Rect* box, float hue, float saturation, float value, float alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    SDL_RenderDrawRect(renderer, box);
}

void WarGrey::STEM::game_fill_rect(SDL_Renderer* renderer, SDL_Rect* box, uint32_t rgb, float alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    SDL_RenderFillRect(renderer, box);
}

void WarGrey::STEM::game_fill_rect(SDL_Renderer* renderer, SDL_Rect* box, float hue, float saturation, float value, float alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    SDL_RenderFillRect(renderer, box);
}

void WarGrey::STEM::game_draw_rect(SDL_Renderer* renderer, int x, int y, int width, int height, uint32_t rgb, float alpha) {
    SDL_Rect box;

    FILL_BOX(box, x, y, width, height);
    game_draw_rect(renderer, &box, rgb, alpha);
}

void WarGrey::STEM::game_draw_rect(SDL_Renderer* renderer, int x, int y, int width, int height, float hue, float saturation, float value, float alpha) {
    SDL_Rect box;

    FILL_BOX(box, x, y, width, height);
    game_draw_rect(renderer, &box, hue, saturation, value, alpha);
}

void WarGrey::STEM::game_fill_rect(SDL_Renderer* renderer, int x, int y, int width, int height, uint32_t rgb, float alpha) {
    SDL_Rect box;

    FILL_BOX(box, x, y, width, height);
    game_fill_rect(renderer, &box, rgb, alpha);
}

void WarGrey::STEM::game_fill_rect(SDL_Renderer* renderer, int x, int y, int width, int height, float hue, float saturation, float value, float alpha) {
    SDL_Rect box;

    FILL_BOX(box, x, y, width, height);
    game_fill_rect(renderer, &box, hue, saturation, value, alpha);
}

void WarGrey::STEM::game_draw_square(SDL_Renderer* renderer, int cx, int cy, int apothem, uint32_t rgb, float alpha) {
    game_draw_rect(renderer, cx - apothem, cy - apothem, apothem * 2, apothem * 2, rgb, alpha);
}

void WarGrey::STEM::game_draw_square(SDL_Renderer* renderer, int cx, int cy, int apothem, float hue, float saturation, float value, float alpha) {
    game_draw_rect(renderer, cx - apothem, cy - apothem, apothem * 2, apothem * 2, hue, saturation, value, alpha);
}

void WarGrey::STEM::game_fill_square(SDL_Renderer* renderer, int cx, int cy, int apothem, uint32_t rgb, float alpha) {
    game_fill_rect(renderer, cx - apothem, cy - apothem, apothem * 2, apothem * 2, rgb, alpha);
}

void WarGrey::STEM::game_fill_square(SDL_Renderer* renderer, int cx, int cy, int apothem, float hue, float saturation, float value, float alpha) {
    game_fill_rect(renderer, cx - apothem, cy - apothem, apothem * 2, apothem * 2, hue, saturation, value, alpha);
}

void WarGrey::STEM::game_draw_circle(SDL_Renderer* renderer, int cx, int cy, int radius, uint32_t rgb, float alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    draw_circle(renderer, cx, cy, radius);
}

void WarGrey::STEM::game_draw_circle(SDL_Renderer* renderer, int cx, int cy, int radius, float hue, float saturation, float value, float alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    draw_circle(renderer, cx, cy, radius);
}

void WarGrey::STEM::game_fill_circle(SDL_Renderer* renderer, int cx, int cy, int radius, uint32_t rgb, float alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    draw_filled_circle(renderer, cx, cy, radius);
}

void WarGrey::STEM::game_fill_circle(SDL_Renderer* renderer, int cx, int cy, int radius, float hue, float saturation, float value, float alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    draw_filled_circle(renderer, cx, cy, radius);
}

void WarGrey::STEM::game_draw_ellipse(SDL_Renderer* renderer, int cx, int cy, int ar, int br, uint32_t rgb, float alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    draw_ellipse(renderer, cx, cy, ar, br);
}

void WarGrey::STEM::game_draw_ellipse(SDL_Renderer* renderer, int cx, int cy, int ar, int br, float hue, float saturation, float value, float alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    draw_ellipse(renderer, cx, cy, ar, br);
}

void WarGrey::STEM::game_fill_ellipse(SDL_Renderer* renderer, int cx, int cy, int ar, int br, uint32_t rgb, float alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    draw_filled_ellipse(renderer, cx, cy, ar, br);
}

void WarGrey::STEM::game_fill_ellipse(SDL_Renderer* renderer, int cx, int cy, int ar, int br, float hue, float saturation, float value, float alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    draw_filled_ellipse(renderer, cx, cy, ar, br);
}

void WarGrey::STEM::game_draw_regular_polygon(SDL_Renderer* renderer, int n, int cx, int cy, int radius, float rotation, uint32_t rgb, float alpha) {
    game_draw_regular_polygon(renderer, n, float(cx), float(cy), float(radius), rotation, rgb, alpha);
}

void WarGrey::STEM::game_draw_regular_polygon(SDL_Renderer* renderer, int n, int cx, int cy, int radius, float rotation, float hue, float saturation, float value, float alpha) {
    game_draw_regular_polygon(renderer, n, float(cx), float(cy), float(radius), rotation, hue, saturation, value, alpha);
}

void WarGrey::STEM::game_fill_regular_polygon(SDL_Renderer* renderer, int n, int cx, int cy, int radius, float rotation, uint32_t rgb, float alpha) {
    game_fill_regular_polygon(renderer, n, float(cx), float(cy), float(radius), rotation, rgb, alpha);
}

void WarGrey::STEM::game_fill_regular_polygon(SDL_Renderer* renderer, int n, int cx, int cy, int radius, float rotation, float hue, float saturation, float value, float alpha) {
    game_fill_regular_polygon(renderer, n, float(cx), float(cy), float(radius), rotation, hue, saturation, value, alpha);
}

/*************************************************************************************************/
void WarGrey::STEM::game_draw_frame(SDL_Renderer* renderer, float x, float y, float width, float height) {
    SDL_FRect box;

    FILL_BOX(box, x - 1.0F, y - 1.0F, width + 3.0F, height + 3.0F);
    SDL_RenderDrawRectF(renderer, &box);
}

void WarGrey::STEM::game_draw_grid(SDL_Renderer* renderer, int nx, int ny, float grid_size, float xoff, float yoff) {
    float xend = xoff + nx * grid_size;
    float yend = yoff + ny * grid_size;

    for (int i = 0; i <= nx; i++) {
        float x = xoff + i * grid_size;

        for (int j = 0; j <= ny; j++) {
            float y = yoff + j * grid_size;

            SDL_RenderDrawLineF(renderer, xoff, y, xend, y);
        }

        SDL_RenderDrawLineF(renderer, x, yoff, x, yend);
    }
}

void WarGrey::STEM::game_fill_grid(SDL_Renderer* renderer, int* grids[], int nx, int ny, float grid_size, float xoff, float yoff) {
    SDL_FRect grid_self;

    grid_self.w = grid_size;
    grid_self.h = grid_size;

    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            if (grids[i][j] > 0) {
                grid_self.x = xoff + i * grid_self.w;
                grid_self.y = yoff + j * grid_self.h;
                SDL_RenderFillRectF(renderer, &grid_self);
            }
        }
    }
}

void WarGrey::STEM::game_render_surface(SDL_Renderer* target, SDL_Surface* surface, float x, float y) {
    SDL_FRect box;

    FILL_BOX(box, x, y, float(surface->w), float(surface->h));
    game_render_surface(target, surface, &box);
}

void WarGrey::STEM::game_render_surface(SDL_Renderer* target, SDL_Surface* surface, SDL_FRect* region) {
    SDL_Texture* texture = SDL_CreateTextureFromSurface(target, surface);

    if (texture != nullptr) {
        SDL_RenderCopyF(target, texture, nullptr, region);
        SDL_DestroyTexture(texture);
    }
}

/**************************************************************************************************/
void WarGrey::STEM::game_draw_point(SDL_Renderer* renderer, float x, float y, uint32_t rgb, float alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    SDL_RenderDrawPointF(renderer, x, y);
}

void WarGrey::STEM::game_draw_point(SDL_Renderer* renderer, float x, float y, float hue, float saturation, float value, float alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    SDL_RenderDrawPointF(renderer, x, y);
}

void WarGrey::STEM::game_draw_line(SDL_Renderer* renderer, float x1, float y1, float x2, float y2, uint32_t rgb, float alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    SDL_RenderDrawLineF(renderer, x1, y1, x2, y2);
}

void WarGrey::STEM::game_draw_line(SDL_Renderer* renderer, float x1, float y1, float x2, float y2, float hue, float saturation, float value, float alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    SDL_RenderDrawLineF(renderer, x1, y1, x2, y2);
}

void WarGrey::STEM::game_draw_rect(SDL_Renderer* renderer, SDL_FRect* box, uint32_t rgb, float alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    SDL_RenderDrawRectF(renderer, box);
}

void WarGrey::STEM::game_draw_rect(SDL_Renderer* renderer, SDL_FRect* box, float hue, float saturation, float value, float alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    SDL_RenderDrawRectF(renderer, box);
}

void WarGrey::STEM::game_fill_rect(SDL_Renderer* renderer, SDL_FRect* box, uint32_t rgb, float alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    SDL_RenderFillRectF(renderer, box);
}

void WarGrey::STEM::game_fill_rect(SDL_Renderer* renderer, SDL_FRect* box, float hue, float saturation, float value, float alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    SDL_RenderFillRectF(renderer, box);
}

void WarGrey::STEM::game_draw_rect(SDL_Renderer* renderer, float x, float y, float width, float height, uint32_t rgb, float alpha) {
    SDL_FRect box;

    FILL_BOX(box, x, y, width, height);
    game_draw_rect(renderer, &box, rgb, alpha);
}

void WarGrey::STEM::game_draw_rect(SDL_Renderer* renderer, float x, float y, float width, float height, float hue, float saturation, float value, float alpha) {
    SDL_FRect box;

    FILL_BOX(box, x, y, width, height);
    game_draw_rect(renderer, &box, hue, saturation, value, alpha);
}

void WarGrey::STEM::game_fill_rect(SDL_Renderer* renderer, float x, float y, float width, float height, uint32_t rgb, float alpha) {
    SDL_FRect box;

    FILL_BOX(box, x, y, width, height);
    game_fill_rect(renderer, &box, rgb, alpha);
}

void WarGrey::STEM::game_fill_rect(SDL_Renderer* renderer, float x, float y, float width, float height, float hue, float saturation, float value, float alpha) {
    SDL_FRect box;

    FILL_BOX(box, x, y, width, height);
    game_fill_rect(renderer, &box, hue, saturation, value, alpha);
}

void WarGrey::STEM::game_draw_square(SDL_Renderer* renderer, float cx, float cy, float apothem, uint32_t rgb, float alpha) {
    game_draw_rect(renderer, cx - apothem, cy - apothem, apothem * 2.0F, apothem * 2.0F, rgb, alpha);
}

void WarGrey::STEM::game_draw_square(SDL_Renderer* renderer, float cx, float cy, float apothem, float hue, float saturation, float value, float alpha) {
    game_draw_rect(renderer, cx - apothem, cy - apothem, apothem * 2.0F, apothem * 2.0F, hue, saturation, value, alpha);
}

void WarGrey::STEM::game_fill_square(SDL_Renderer* renderer, float cx, float cy, float apothem, uint32_t rgb, float alpha) {
    game_fill_rect(renderer, cx - apothem, cy - apothem, apothem * 2.0F, apothem * 2.0F, rgb, alpha);
}

void WarGrey::STEM::game_fill_square(SDL_Renderer* renderer, float cx, float cy, float apothem, float hue, float saturation, float value, float alpha) {
    game_fill_rect(renderer, cx - apothem, cy - apothem, apothem * 2.0F, apothem * 2.0F, hue, saturation, value, alpha);
}

void WarGrey::STEM::game_draw_circle(SDL_Renderer* renderer, float cx, float cy, float radius, uint32_t rgb, float alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    draw_circle(renderer, fl2fxi(cx), fl2fxi(cy), fl2fxi(radius));
}

void WarGrey::STEM::game_draw_circle(SDL_Renderer* renderer, float cx, float cy, float radius, float hue, float saturation, float value, float alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    draw_circle(renderer, fl2fxi(cx), fl2fxi(cy), fl2fxi(radius));
}

void WarGrey::STEM::game_fill_circle(SDL_Renderer* renderer, float cx, float cy, float radius, uint32_t rgb, float alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    draw_filled_circle(renderer, fl2fxi(cx), fl2fxi(cy), fl2fxi(radius));
}

void WarGrey::STEM::game_fill_circle(SDL_Renderer* renderer, float cx, float cy, float radius, float hue, float saturation, float value, float alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    draw_filled_circle(renderer, fl2fxi(cx), fl2fxi(cy), fl2fxi(radius));
}

void WarGrey::STEM::game_draw_ellipse(SDL_Renderer* renderer, float cx, float cy, float ar, float br, uint32_t rgb, float alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    draw_ellipse(renderer, fl2fxi(cx), fl2fxi(cy), fl2fxi(ar), fl2fxi(br));
}

void WarGrey::STEM::game_draw_ellipse(SDL_Renderer* renderer, float cx, float cy, float ar, float br, float hue, float saturation, float value, float alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    draw_ellipse(renderer, fl2fxi(cx), fl2fxi(cy), fl2fxi(ar), fl2fxi(br));
}

void WarGrey::STEM::game_fill_ellipse(SDL_Renderer* renderer, float cx, float cy, float ar, float br, uint32_t rgb, float alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    draw_filled_ellipse(renderer, fl2fxi(cx), fl2fxi(cy), fl2fxi(ar), fl2fxi(br));
}

void WarGrey::STEM::game_fill_ellipse(SDL_Renderer* renderer, float cx, float cy, float ar, float br, float hue, float saturation, float value, float alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    draw_filled_ellipse(renderer, fl2fxi(cx), fl2fxi(cy), fl2fxi(ar), fl2fxi(br));
}

void WarGrey::STEM::game_draw_regular_polygon(SDL_Renderer* renderer, int n, float cx, float cy, float radius, float rotation, uint32_t rgb, float alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    draw_regular_polygon(renderer, n, cx, cy, radius, rotation);
}

void WarGrey::STEM::game_draw_regular_polygon(SDL_Renderer* renderer, int n, float cx, float cy, float radius, float rotation, float hue, float saturation, float value, float alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    draw_regular_polygon(renderer, n, cx, cy, radius, rotation);
}

void WarGrey::STEM::game_fill_regular_polygon(SDL_Renderer* renderer, int n, float cx, float cy, float radius, float rotation, uint32_t rgb, float alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    fill_regular_polygon(renderer, n, cx, cy, radius, rotation);
}

void WarGrey::STEM::game_fill_regular_polygon(SDL_Renderer* renderer, int n, float cx, float cy, float radius, float rotation, float hue, float saturation, float value, float alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    fill_regular_polygon(renderer, n, cx, cy, radius, rotation);
}

