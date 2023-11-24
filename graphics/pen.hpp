#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <cstdint>
#include <string>

namespace WarGrey::STEM {
    class __lambda__ Pen {
    public:
        static void clear(SDL_Renderer* renderer, uint32_t rgb = 0U, double alpha = 0.0);
        static void clear(SDL_Renderer* renderer, double hue, double saturation = 1.0 , double value = 1.0, double alpha = 1.0);

        static void draw_frame(SDL_Renderer* renderer, int x, int y, int width, int height);
        static void draw_grid(SDL_Renderer* renderer, int row, int col, int cell_width, int cell_height, int xoff = 0, int yoff = 0);
        static void fill_grid(SDL_Renderer* renderer, int* grids[], int row, int col, int cell_width, int cell_height, int xoff = 0, int yoff = 0);

        static void draw_frame(SDL_Renderer* renderer, float x, float y, float width, float height);
        static void draw_grid(SDL_Renderer* renderer, int row, int col, float cell_width, float cell_height, float xoff = 0.0F, float yoff = 0.0F);
        static void fill_grid(SDL_Renderer* renderer, int* grids[], int row, int col, float cell_width, float cell_height, float xoff = 0.0F, float yoff = 0.0F);

        static void stamp(SDL_Renderer* target, SDL_Surface* surface, int x, int y, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        static void stamp(SDL_Renderer* target, SDL_Surface* surface, int x, int y, int widht, int height, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        static void stamp(SDL_Renderer* target, SDL_Surface* surface, SDL_Rect* region, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        static void stamp(SDL_Renderer* target, SDL_Surface* surface, SDL_Rect* src, SDL_Rect* dst, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);

        static void stamp(SDL_Renderer* target, SDL_Surface* surface, float x, float y, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        static void stamp(SDL_Renderer* target, SDL_Surface* surface, float x, float y, float width, float height, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        static void stamp(SDL_Renderer* target, SDL_Surface* surface, SDL_FRect* region, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        static void stamp(SDL_Renderer* target, SDL_Surface* surface, SDL_Rect* src, SDL_FRect* dst, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);

        static int stamp(SDL_Renderer* target, SDL_Texture* texture, int x, int y, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        static int stamp(SDL_Renderer* target, SDL_Texture* texture, int x, int y, int width, int height, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        static int stamp(SDL_Renderer* target, SDL_Texture* texture, SDL_Rect* region, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        static int stamp(SDL_Renderer* target, SDL_Texture* texture, SDL_Rect* src, SDL_Rect* dst, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        
        static int stamp(SDL_Renderer* target, SDL_Texture* texture, float x, float y, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        static int stamp(SDL_Renderer* target, SDL_Texture* texture, float x, float y, float width, float height, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        static int stamp(SDL_Renderer* target, SDL_Texture* texture, SDL_FRect* region, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        static int stamp(SDL_Renderer* target, SDL_Texture* texture, SDL_Rect* src, SDL_FRect* dst, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        
    public:
        static void draw_point(SDL_Renderer* renderer, int x, int y, uint32_t rgb, double alpha = 1.0);
        static void draw_point(SDL_Renderer* renderer, int x, int y, double hue, double saturation = 1.0 , double value = 1.0, double alpha = 1.0);
        static void draw_line(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, uint32_t rgb, double alpha = 1.0);
        static void draw_line(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void draw_hline(SDL_Renderer* renderer, int x, int y, int length, uint32_t rgb, double alpha = 1.0);
        static void draw_hline(SDL_Renderer* renderer, int x, int y, int length, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void draw_vline(SDL_Renderer* renderer, int x, int y, int length, uint32_t rgb, double alpha = 1.0);
        static void draw_vline(SDL_Renderer* renderer, int x, int y, int length, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
    
        static void draw_rect(SDL_Renderer* renderer, SDL_Rect* box, uint32_t rgb, double alpha = 1.0);
        static void draw_rect(SDL_Renderer* renderer, SDL_Rect* box, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void fill_rect(SDL_Renderer* renderer, SDL_Rect* box, uint32_t rgb, double alpha = 1.0);
        static void fill_rect(SDL_Renderer* renderer, SDL_Rect* box, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void draw_rect(SDL_Renderer* renderer, int x, int y, int width, int height, uint32_t rgb, double alpha = 1.0);
        static void draw_rect(SDL_Renderer* renderer, int x, int y, int width, int height, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void fill_rect(SDL_Renderer* renderer, int x, int y, int width, int height, uint32_t rgb, double alpha = 1.0);
        static void fill_rect(SDL_Renderer* renderer, int x, int y, int width, int height, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);

        static void draw_rounded_rect(SDL_Renderer* renderer, SDL_Rect* box, float rad, uint32_t rgb, double alpha = 1.0);
        static void draw_rounded_rect(SDL_Renderer* renderer, SDL_Rect* box, float rad, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void fill_rounded_rect(SDL_Renderer* renderer, SDL_Rect* box, float rad, uint32_t rgb, double alpha = 1.0);
        static void fill_rounded_rect(SDL_Renderer* renderer, SDL_Rect* box, float rad, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void draw_rounded_rect(SDL_Renderer* renderer, int x, int y, int width, int height, float rad, uint32_t rgb, double alpha = 1.0);
        static void draw_rounded_rect(SDL_Renderer* renderer, int x, int y, int width, int height, float rad, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void fill_rounded_rect(SDL_Renderer* renderer, int x, int y, int width, int height, float rad, uint32_t rgb, double alpha = 1.0);
        static void fill_rounded_rect(SDL_Renderer* renderer, int x, int y, int width, int height, float rad, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);

        static void draw_square(SDL_Renderer* renderer, int cx, int cy, int apothem, uint32_t rgb, double alpha = 1.0);
        static void draw_square(SDL_Renderer* renderer, int cx, int cy, int apothem, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void fill_square(SDL_Renderer* renderer, int cx, int cy, int apothem, uint32_t rgb, double alpha = 1.0);
        static void fill_square(SDL_Renderer* renderer, int cx, int cy, int apothem, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);

        static void draw_rounded_square(SDL_Renderer* renderer, int cx, int cy, int apothem, float rad, uint32_t rgb, double alpha = 1.0);
        static void draw_rounded_square(SDL_Renderer* renderer, int cx, int cy, int apothem, float rad, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void fill_rounded_square(SDL_Renderer* renderer, int cx, int cy, int apothem, float rad, uint32_t rgb, double alpha = 1.0);
        static void fill_rounded_square(SDL_Renderer* renderer, int cx, int cy, int apothem, float rad, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);

        static void draw_circle(SDL_Renderer* renderer, int cx, int cy, int radius, uint32_t rgb, double alpha = 1.0);
        static void draw_circle(SDL_Renderer* renderer, int cx, int cy, int radius, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void fill_circle(SDL_Renderer* renderer, int cx, int cy, int radius, uint32_t rgb, double alpha = 1.0);
        static void fill_circle(SDL_Renderer* renderer, int cx, int cy, int radius, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);

        static void draw_ellipse(SDL_Renderer* renderer, int cx, int cy, int aradius, int bradius, uint32_t rgb, double alpha = 1.0);
        static void draw_ellipse(SDL_Renderer* renderer, int cx, int cy, int aradius, int bradius, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void fill_ellipse(SDL_Renderer* renderer, int cx, int cy, int aradius, int bradius, uint32_t rgb, double alpha = 1.0);
        static void fill_ellipse(SDL_Renderer* renderer, int cx, int cy, int aradius, int bradius, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);

        static void draw_regular_polygon(SDL_Renderer* renderer, int n, int cx, int cy, int radius, float rotation, uint32_t rgb, double alpha = 1.0);
        static void draw_regular_polygon(SDL_Renderer* renderer, int n, int cx, int cy, int radius, float rotation, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void fill_regular_polygon(SDL_Renderer* renderer, int n, int cx, int cy, int radius, float rotation, uint32_t rgb, double alpha = 1.0);
        static void fill_regular_polygon(SDL_Renderer* renderer, int n, int cx, int cy, int radius, float rotation, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);

    public:
        static void draw_point(SDL_Renderer* renderer, float x, float y, uint32_t rgb, double alpha = 1.0);
        static void draw_point(SDL_Renderer* renderer, float x, float y, double hue, double saturation = 1.0 , double value = 1.0, double alpha = 1.0);
        static void draw_line(SDL_Renderer* renderer, float x1, float y1, float x2, float y2, uint32_t rgb, double alpha = 1.0);
        static void draw_line(SDL_Renderer* renderer, float x1, float y1, float x2, float y2, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void draw_hline(SDL_Renderer* renderer, float x, float y, float length, uint32_t rgb, double alpha = 1.0);
        static void draw_hline(SDL_Renderer* renderer, float x, float y, float length, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void draw_vline(SDL_Renderer* renderer, float x, float y, float length, uint32_t rgb, double alpha = 1.0);
        static void draw_vline(SDL_Renderer* renderer, float x, float y, float length, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
    
        static void draw_points(SDL_Renderer* renderer, const SDL_FPoint* pts, int size, uint32_t rgb, double alpha = 1.0);
        static void draw_points(SDL_Renderer* renderer, const SDL_FPoint* pts, int size, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void draw_lines(SDL_Renderer* renderer, const SDL_FPoint* pts, int size, uint32_t rgb, double alpha = 1.0);
        static void draw_lines(SDL_Renderer* renderer, const SDL_FPoint* pts, int size, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);

        static void draw_rect(SDL_Renderer* renderer, SDL_FRect* box, uint32_t rgb, double alpha = 1.0);
        static void draw_rect(SDL_Renderer* renderer, SDL_FRect* box, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void fill_rect(SDL_Renderer* renderer, SDL_FRect* box, uint32_t rgb, double alpha = 1.0);
        static void fill_rect(SDL_Renderer* renderer, SDL_FRect* box, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void draw_rect(SDL_Renderer* renderer, float x, float y, float width, float height, uint32_t rgb, double alpha = 1.0);
        static void draw_rect(SDL_Renderer* renderer, float x, float y, float width, float height, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void fill_rect(SDL_Renderer* renderer, float x, float y, float width, float height, uint32_t rgb, double alpha = 1.0);
        static void fill_rect(SDL_Renderer* renderer, float x, float y, float width, float height, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);

        static void draw_rounded_rect(SDL_Renderer* renderer, SDL_FRect* box, float rad, uint32_t rgb, double alpha = 1.0);
        static void draw_rounded_rect(SDL_Renderer* renderer, SDL_FRect* box, float rad, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void fill_rounded_rect(SDL_Renderer* renderer, SDL_FRect* box, float rad, uint32_t rgb, double alpha = 1.0);
        static void fill_rounded_rect(SDL_Renderer* renderer, SDL_FRect* box, float rad, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void draw_rounded_rect(SDL_Renderer* renderer, float x, float y, float width, float height, float rad, uint32_t rgb, double alpha = 1.0);
        static void draw_rounded_rect(SDL_Renderer* renderer, float x, float y, float width, float height, float rad, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void fill_rounded_rect(SDL_Renderer* renderer, float x, float y, float width, float height, float rad, uint32_t rgb, double alpha = 1.0);
        static void fill_rounded_rect(SDL_Renderer* renderer, float x, float y, float width, float height, float rad, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);

        static void draw_square(SDL_Renderer* renderer, float cx, float cy, float apothem, uint32_t rgb, double alpha = 1.0);
        static void draw_square(SDL_Renderer* renderer, float cx, float cy, float apothem, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void fill_square(SDL_Renderer* renderer, float cx, float cy, float apothem, uint32_t rgb, double alpha = 1.0);
        static void fill_square(SDL_Renderer* renderer, float cx, float cy, float apothem, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);

        static void draw_circle(SDL_Renderer* renderer, float cx, float cy, float radius, uint32_t rgb, double alpha = 1.0);
        static void draw_circle(SDL_Renderer* renderer, float cx, float cy, float radius, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void fill_circle(SDL_Renderer* renderer, float cx, float cy, float radius, uint32_t rgb, double alpha = 1.0);
        static void fill_circle(SDL_Renderer* renderer, float cx, float cy, float radius, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);

        static void draw_rounded_square(SDL_Renderer* renderer, float cx, float cy, float apothem, float rad, uint32_t rgb, double alpha = 1.0);
        static void draw_rounded_square(SDL_Renderer* renderer, float cx, float cy, float apothem, float rad, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void fill_rounded_square(SDL_Renderer* renderer, float cx, float cy, float apothem, float rad, uint32_t rgb, double alpha = 1.0);
        static void fill_rounded_square(SDL_Renderer* renderer, float cx, float cy, float apothem, float rad, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);

        static void draw_ellipse(SDL_Renderer* renderer, float cx, float cy, float aradius, float bradius, uint32_t rgb, double alpha = 1.0);
        static void draw_ellipse(SDL_Renderer* renderer, float cx, float cy, float aradius, float bradius, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void fill_ellipse(SDL_Renderer* renderer, float cx, float cy, float aradius, float bradius, uint32_t rgb, double alpha = 1.0);
        static void fill_ellipse(SDL_Renderer* renderer, float cx, float cy, float aradius, float bradius, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);

        static void draw_regular_polygon(SDL_Renderer* renderer, int n, float cx, float cy, float radius, float rotation, uint32_t rgb, double alpha = 1.0);
        static void draw_regular_polygon(SDL_Renderer* renderer, int n, float cx, float cy, float radius, float rotation, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
        static void fill_regular_polygon(SDL_Renderer* renderer, int n, float cx, float cy, float radius, float rotation, uint32_t rgb, double alpha = 1.0);
        static void fill_regular_polygon(SDL_Renderer* renderer, int n, float cx, float cy, float radius, float rotation, double hue, double saturation = 1.0, double value = 1.0, double alpha = 1.0);
    };
}
