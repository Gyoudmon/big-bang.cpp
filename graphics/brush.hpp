#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <cstdint>
#include <string>

#include "../physics/color/rgba.hpp"
#include "../physics/geometry/aabox.hpp"

namespace GYDM {
    class __lambda__ Brush {
    public:
        static void feed_rect(SDL_Rect* rect, const GYDM::AABox<int>& box);
        static void feed_rect(SDL_FRect* rect, const GYDM::Box& box);

    public:
        static void clear(SDL_Renderer* renderer, const GYDM::RGBA& color);

        static void draw_frame(SDL_Renderer* renderer, int x, int y, int width, int height, const GYDM::RGBA& color);
        static void draw_grid(SDL_Renderer* renderer, int row, int col, int cell_width, int cell_height, const GYDM::RGBA& color, int xoff = 0, int yoff = 0);
        static void fill_grid(SDL_Renderer* renderer, int* grids[], int row, int col, int cell_width, int cell_height, const GYDM::RGBA& color, int xoff = 0, int yoff = 0);

        static void draw_frame(SDL_Renderer* renderer, float x, float y, float width, float height, const GYDM::RGBA& color);
        static void draw_grid(SDL_Renderer* renderer, int row, int col, float cell_width, float cell_height, const GYDM::RGBA& color, float xoff = 0.0F, float yoff = 0.0F);
        static void fill_grid(SDL_Renderer* renderer, int* grids[], int row, int col, float cell_width, float cell_height, const GYDM::RGBA& color, float xoff = 0.0F, float yoff = 0.0F);

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
        static void draw_point(SDL_Renderer* renderer, int x, int y, const GYDM::RGBA& color);
        static void draw_line(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, const GYDM::RGBA& color);
        static void draw_hline(SDL_Renderer* renderer, int x, int y, int length, const GYDM::RGBA& color);
        static void draw_vline(SDL_Renderer* renderer, int x, int y, int length, const GYDM::RGBA& color);
        
        static void draw_rect(SDL_Renderer* renderer, SDL_Rect* box, const GYDM::RGBA& color);
        static void fill_rect(SDL_Renderer* renderer, SDL_Rect* box, const GYDM::RGBA& color);
        static void draw_rect(SDL_Renderer* renderer, int x, int y, int width, int height, const GYDM::RGBA& color);
        static void fill_rect(SDL_Renderer* renderer, int x, int y, int width, int height, const GYDM::RGBA& color);
        
        static void draw_rounded_rect(SDL_Renderer* renderer, SDL_Rect* box, float rad, const GYDM::RGBA& color);
        static void fill_rounded_rect(SDL_Renderer* renderer, SDL_Rect* box, float rad, const GYDM::RGBA& color);
        static void draw_rounded_rect(SDL_Renderer* renderer, int x, int y, int width, int height, float rad, const GYDM::RGBA& color);
        static void fill_rounded_rect(SDL_Renderer* renderer, int x, int y, int width, int height, float rad, const GYDM::RGBA& color);
        
        static void draw_square(SDL_Renderer* renderer, int cx, int cy, int apothem, const GYDM::RGBA& color);
        static void fill_square(SDL_Renderer* renderer, int cx, int cy, int apothem, const GYDM::RGBA& color);
        
        static void draw_rounded_square(SDL_Renderer* renderer, int cx, int cy, int apothem, float rad, const GYDM::RGBA& color);
        static void fill_rounded_square(SDL_Renderer* renderer, int cx, int cy, int apothem, float rad, const GYDM::RGBA& color);
        
        static void draw_circle(SDL_Renderer* renderer, int cx, int cy, int radius, const GYDM::RGBA& color);
        static void fill_circle(SDL_Renderer* renderer, int cx, int cy, int radius, const GYDM::RGBA& color);
        
        static void draw_ellipse(SDL_Renderer* renderer, int cx, int cy, int aradius, int bradius, const GYDM::RGBA& color);
        static void fill_ellipse(SDL_Renderer* renderer, int cx, int cy, int aradius, int bradius, const GYDM::RGBA& color);
        
        static void draw_regular_polygon(SDL_Renderer* renderer, int n, int cx, int cy, int radius, float rotation, const GYDM::RGBA& color);
        static void fill_regular_polygon(SDL_Renderer* renderer, int n, int cx, int cy, int radius, float rotation, const GYDM::RGBA& color);
        
    public:
        static void draw_point(SDL_Renderer* renderer, float x, float y, const GYDM::RGBA& color);
        static void draw_line(SDL_Renderer* renderer, float x1, float y1, float x2, float y2, const GYDM::RGBA& color);
        static void draw_hline(SDL_Renderer* renderer, float x, float y, float length, const GYDM::RGBA& color);
        static void draw_vline(SDL_Renderer* renderer, float x, float y, float length, const GYDM::RGBA& color);
        
        static void draw_points(SDL_Renderer* renderer, const SDL_FPoint* pts, int size, const GYDM::RGBA& color);
        static void draw_lines(SDL_Renderer* renderer, const SDL_FPoint* pts, int size, const GYDM::RGBA& color);
        
        static void draw_rect(SDL_Renderer* renderer, SDL_FRect* box, const GYDM::RGBA& color);
        static void fill_rect(SDL_Renderer* renderer, SDL_FRect* box, const GYDM::RGBA& color);
        static void draw_rect(SDL_Renderer* renderer, float x, float y, float width, float height, const GYDM::RGBA& color);
        static void fill_rect(SDL_Renderer* renderer, float x, float y, float width, float height, const GYDM::RGBA& color);
        
        static void draw_rounded_rect(SDL_Renderer* renderer, SDL_FRect* box, float rad, const GYDM::RGBA& color);
        static void fill_rounded_rect(SDL_Renderer* renderer, SDL_FRect* box, float rad, const GYDM::RGBA& color);
        static void draw_rounded_rect(SDL_Renderer* renderer, float x, float y, float width, float height, float rad, const GYDM::RGBA& color);
        static void fill_rounded_rect(SDL_Renderer* renderer, float x, float y, float width, float height, float rad, const GYDM::RGBA& color);
        
        static void draw_square(SDL_Renderer* renderer, float cx, float cy, float apothem, const GYDM::RGBA& color);
        static void fill_square(SDL_Renderer* renderer, float cx, float cy, float apothem, const GYDM::RGBA& color);
        
        static void draw_circle(SDL_Renderer* renderer, float cx, float cy, float radius, const GYDM::RGBA& color);
        static void fill_circle(SDL_Renderer* renderer, float cx, float cy, float radius, const GYDM::RGBA& color);
        
        static void draw_rounded_square(SDL_Renderer* renderer, float cx, float cy, float apothem, float rad, const GYDM::RGBA& color);
        static void fill_rounded_square(SDL_Renderer* renderer, float cx, float cy, float apothem, float rad, const GYDM::RGBA& color);
        
        static void draw_ellipse(SDL_Renderer* renderer, float cx, float cy, float aradius, float bradius, const GYDM::RGBA& color);
        static void fill_ellipse(SDL_Renderer* renderer, float cx, float cy, float aradius, float bradius, const GYDM::RGBA& color);
        
        static void draw_regular_polygon(SDL_Renderer* renderer, int n, float cx, float cy, float radius, float rotation, const GYDM::RGBA& color);
        static void fill_regular_polygon(SDL_Renderer* renderer, int n, float cx, float cy, float radius, float rotation, const GYDM::RGBA& color);
    };
}
