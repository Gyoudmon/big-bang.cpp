#include "geometry.hpp"
#include "colorspace.hpp"

#include <SDL2/SDL2_gfxPrimitives.h>

using namespace WarGrey::STEM;

/**************************************************************************************************/
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
    aaellipseRGBA(renderer, (int16_t)(cx), (int16_t)(cy), (int16_t)(ar), int16_t(br), 0x12, 0x34, 0x56, 0xFFU);
}

static void draw_filled_ellipse(SDL_Renderer* renderer, int cx, int cy, int ar, int br) {
    filledEllipseRGBA(renderer, (int16_t)(cx), (int16_t)(cy), (int16_t)(ar), int16_t(br), 0x12, 0x34, 0x56, 0xFFU);
}

/*************************************************************************************************/
void WarGrey::STEM::game_draw_frame(SDL_Renderer* renderer, int x, int y, int width, int height) {
    SDL_Rect box;

    box.x = x - 1;
    box.y = y - 1;
    box.w = width + 3;
    box.h = height + 3;

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

    box.x = x;
    box.y = y;
    box.w = surface->w;
    box.h = surface->h;

    game_render_surface(target, surface, &box);
}

void WarGrey::STEM::game_render_surface(SDL_Renderer* target, SDL_Surface* surface, SDL_Rect* region) {
    SDL_Texture* texture = SDL_CreateTextureFromSurface(target, surface);

    if (texture != NULL) {
        SDL_RenderCopy(target, texture, NULL, region);
        SDL_DestroyTexture(texture);
    }
}

/**************************************************************************************************/
void WarGrey::STEM::game_draw_point(SDL_Renderer* renderer, int x, int y, uint32_t rgb, uint8_t alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    SDL_RenderDrawPoint(renderer, x, y);
}

void WarGrey::STEM::game_draw_point(SDL_Renderer* renderer, int x, int y, float hue, float saturation, float value, uint8_t alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    SDL_RenderDrawPoint(renderer, x, y);
}

void WarGrey::STEM::game_draw_line(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, uint32_t rgb, uint8_t alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void WarGrey::STEM::game_draw_line(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, float hue, float saturation, float value, uint8_t alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void WarGrey::STEM::game_draw_rect(SDL_Renderer* renderer, SDL_Rect* box, uint32_t rgb, uint8_t alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    SDL_RenderDrawRect(renderer, box);
}

void WarGrey::STEM::game_draw_rect(SDL_Renderer* renderer, SDL_Rect* box, float hue, float saturation, float value, uint8_t alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    SDL_RenderDrawRect(renderer, box);
}

void WarGrey::STEM::game_fill_rect(SDL_Renderer* renderer, SDL_Rect* box, uint32_t rgb, uint8_t alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    SDL_RenderFillRect(renderer, box);
}

void WarGrey::STEM::game_fill_rect(SDL_Renderer* renderer, SDL_Rect* box, float hue, float saturation, float value, uint8_t alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    SDL_RenderFillRect(renderer, box);
}

void WarGrey::STEM::game_draw_rect(SDL_Renderer* renderer, int x, int y, int width, int height, uint32_t rgb, uint8_t alpha) {
    SDL_Rect box;

    box.x = x;
    box.y = y;
    box.w = width;
    box.h = height;

    game_draw_rect(renderer, &box, rgb, alpha);
}

void WarGrey::STEM::game_draw_rect(SDL_Renderer* renderer, int x, int y, int width, int height, float hue, float saturation, float value, uint8_t alpha) {
    SDL_Rect box;

    box.x = x;
    box.y = y;
    box.w = width;
    box.h = height;

    game_draw_rect(renderer, &box, hue, saturation, value, alpha);
}

void WarGrey::STEM::game_fill_rect(SDL_Renderer* renderer, int x, int y, int width, int height, uint32_t rgb, uint8_t alpha) {
    SDL_Rect box;

    box.x = x;
    box.y = y;
    box.w = width;
    box.h = height;

    game_fill_rect(renderer, &box, rgb, alpha);
}

void WarGrey::STEM::game_fill_rect(SDL_Renderer* renderer, int x, int y, int width, int height, float hue, float saturation, float value, uint8_t alpha) {
    SDL_Rect box;

    box.x = x;
    box.y = y;
    box.w = width;
    box.h = height;

    game_fill_rect(renderer, &box, hue, saturation, value, alpha);
}

void WarGrey::STEM::game_draw_square(SDL_Renderer* renderer, int cx, int cy, int apothem, uint32_t rgb, uint8_t alpha) {
    game_draw_rect(renderer, cx - apothem, cy - apothem, apothem * 2, apothem * 2, rgb, alpha);
}

void WarGrey::STEM::game_draw_square(SDL_Renderer* renderer, int cx, int cy, int apothem, float hue, float saturation, float value, uint8_t alpha) {
    game_draw_rect(renderer, cx - apothem, cy - apothem, apothem * 2, apothem * 2, hue, saturation, value, alpha);
}

void WarGrey::STEM::game_fill_square(SDL_Renderer* renderer, int cx, int cy, int apothem, uint32_t rgb, uint8_t alpha) {
    game_fill_rect(renderer, cx - apothem, cy - apothem, apothem * 2, apothem * 2, rgb, alpha);
}

void WarGrey::STEM::game_fill_square(SDL_Renderer* renderer, int cx, int cy, int apothem, float hue, float saturation, float value, uint8_t alpha) {
    game_fill_rect(renderer, cx - apothem, cy - apothem, apothem * 2, apothem * 2, hue, saturation, value, alpha);
}

void WarGrey::STEM::game_draw_circle(SDL_Renderer* renderer, int cx, int cy, int radius, uint32_t rgb, uint8_t alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    draw_circle(renderer, cx, cy, radius);
}

void WarGrey::STEM::game_draw_circle(SDL_Renderer* renderer, int cx, int cy, int radius, float hue, float saturation, float value, uint8_t alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    draw_circle(renderer, cx, cy, radius);
}

void WarGrey::STEM::game_fill_circle(SDL_Renderer* renderer, int cx, int cy, int radius, uint32_t rgb, uint8_t alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    draw_filled_circle(renderer, cx, cy, radius);
}

void WarGrey::STEM::game_fill_circle(SDL_Renderer* renderer, int cx, int cy, int radius, float hue, float saturation, float value, uint8_t alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    draw_filled_circle(renderer, cx, cy, radius);
}

void WarGrey::STEM::game_draw_ellipse(SDL_Renderer* renderer, int cx, int cy, int ar, int br, uint32_t rgb, uint8_t alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    draw_ellipse(renderer, cx, cy, ar, br);
}

void WarGrey::STEM::game_draw_ellipse(SDL_Renderer* renderer, int cx, int cy, int ar, int br, float hue, float saturation, float value, uint8_t alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    draw_ellipse(renderer, cx, cy, ar, br);
}

void WarGrey::STEM::game_fill_ellipse(SDL_Renderer* renderer, int cx, int cy, int ar, int br, uint32_t rgb, uint8_t alpha) {
    RGB_SetRenderDrawColor(renderer, rgb, alpha);
    draw_filled_ellipse(renderer, cx, cy, ar, br);
}

void WarGrey::STEM::game_fill_ellipse(SDL_Renderer* renderer, int cx, int cy, int ar, int br, float hue, float saturation, float value, uint8_t alpha) {
    HSV_SetRenderDrawColor(renderer, hue, saturation, value, alpha);
    draw_filled_ellipse(renderer, cx, cy, ar, br);
}

