#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <cstdint>
#include <string>

#include "font.hpp"

#include "../physics/color/rgba.hpp"

namespace GYDM {
    enum TextRenderMode { Solid, Shaded, Blender, LCD };

    class __lambda__ DrawingContext {
    public:
        DrawingContext(SDL_Renderer* device);
        ~DrawingContext() noexcept;

    public:
        SDL_Renderer* self() const { return this->device; }
        const char* name() const { return this->info.name; }

    public:
        int feed_output_size(int* width, int* height) { return SDL_GetRendererOutputSize(this->device, width, height); }
        SDL_Surface* snapshot(int width, int height);

    public:
        void clear(const GYDM::RGBA& color);
        void reset(const GYDM::RGBA& fgc, const GYDM::RGBA& bgc);
        void reset(SDL_Texture* texture, const GYDM::RGBA& fgc, const GYDM::RGBA& bgc);
        void refresh(SDL_Texture* texture);

    public:
        SDL_Texture* create_blank_image(int width, int height);
        SDL_Texture* create_blank_image(float width, float height);
        SDL_Texture* get_target() { return SDL_GetRenderTarget(this->device); }
        int set_draw_color(const GYDM::RGBA& color);
        int set_target(SDL_Texture* target) { return SDL_SetRenderTarget(this->device, target); }
        bool set_clipping_region(SDL_Rect* rect) { return SDL_RenderSetClipRect(this->device, rect); }
        bool clear_clipping_region() { return this->set_clipping_region(nullptr); }

    public:
        void draw_frame(int x, int y, int width, int height, const GYDM::RGBA& color);
        void draw_grid(int row, int col, int cell_width, int cell_height, const GYDM::RGBA& color, int xoff = 0, int yoff = 0);
        void fill_grid(int* grids[], int row, int col, int cell_width, int cell_height, const GYDM::RGBA& color, int xoff = 0, int yoff = 0);

        void draw_frame(float x, float y, float width, float height, const GYDM::RGBA& color);
        void draw_grid(int row, int col, float cell_width, float cell_height, const GYDM::RGBA& color, float xoff = 0.0F, float yoff = 0.0F);
        void fill_grid(int* grids[], int row, int col, float cell_width, float cell_height, const GYDM::RGBA& color, float xoff = 0.0F, float yoff = 0.0F);

        void stamp(SDL_Surface* surface, int x, int y, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        void stamp(SDL_Surface* surface, int x, int y, int widht, int height, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        void stamp(SDL_Surface* surface, SDL_Rect* region, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        void stamp(SDL_Surface* surface, SDL_Rect* src, SDL_Rect* dst, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);

        void stamp(SDL_Surface* surface, float x, float y, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        void stamp(SDL_Surface* surface, float x, float y, float width, float height, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        void stamp(SDL_Surface* surface, SDL_FRect* region, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        void stamp(SDL_Surface* surface, SDL_Rect* src, SDL_FRect* dst, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);

        int stamp(SDL_Texture* texture, int x, int y, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        int stamp(SDL_Texture* texture, int x, int y, int width, int height, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        int stamp(SDL_Texture* texture, SDL_Rect* region, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        int stamp(SDL_Texture* texture, SDL_Rect* src, SDL_Rect* dst, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        
        int stamp(SDL_Texture* texture, float x, float y, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        int stamp(SDL_Texture* texture, float x, float y, float width, float height, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        int stamp(SDL_Texture* texture, SDL_FRect* region, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        int stamp(SDL_Texture* texture, SDL_Rect* src, SDL_FRect* dst, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0);
        
    public:
        void draw_point(int x, int y, const GYDM::RGBA& color);
        void draw_line(int x1, int y1, int x2, int y2, const GYDM::RGBA& color);
        void draw_hline(int x, int y, int length, const GYDM::RGBA& color);
        void draw_vline(int x, int y, int length, const GYDM::RGBA& color);

        void draw_points(const SDL_Point* pts, int size, const GYDM::RGBA& color);
        void draw_lines(const SDL_Point* pts, int size, const GYDM::RGBA& color);
        
        void draw_rect(SDL_Rect* box, const GYDM::RGBA& color);
        void fill_rect(SDL_Rect* box, const GYDM::RGBA& color);
        void draw_rect(int x, int y, int width, int height, const GYDM::RGBA& color);
        void fill_rect(int x, int y, int width, int height, const GYDM::RGBA& color);
        
        void draw_rounded_rect(SDL_Rect* box, float rad, const GYDM::RGBA& color);
        void fill_rounded_rect(SDL_Rect* box, float rad, const GYDM::RGBA& color);
        void draw_rounded_rect(int x, int y, int width, int height, float rad, const GYDM::RGBA& color);
        void fill_rounded_rect(int x, int y, int width, int height, float rad, const GYDM::RGBA& color);
        
        void draw_square(int cx, int cy, int apothem, const GYDM::RGBA& color);
        void fill_square(int cx, int cy, int apothem, const GYDM::RGBA& color);
        
        void draw_rounded_square(int cx, int cy, int apothem, float rad, const GYDM::RGBA& color);
        void fill_rounded_square(int cx, int cy, int apothem, float rad, const GYDM::RGBA& color);
        
        void draw_circle(int cx, int cy, int radius, const GYDM::RGBA& color);
        void fill_circle(int cx, int cy, int radius, const GYDM::RGBA& color);
        
        void draw_ellipse(int cx, int cy, int aradius, int bradius, const GYDM::RGBA& color);
        void fill_ellipse(int cx, int cy, int aradius, int bradius, const GYDM::RGBA& color);
        
        void draw_regular_polygon(int n, int cx, int cy, int radius, float rotation, const GYDM::RGBA& color);
        void fill_regular_polygon(int n, int cx, int cy, int radius, float rotation, const GYDM::RGBA& color);
        
    public:
        void draw_point(float x, float y, const GYDM::RGBA& color);
        void draw_line(float x1, float y1, float x2, float y2, const GYDM::RGBA& color);
        void draw_hline(float x, float y, float length, const GYDM::RGBA& color);
        void draw_vline(float x, float y, float length, const GYDM::RGBA& color);
        
        void draw_points(const SDL_FPoint* pts, int size, const GYDM::RGBA& color);
        void draw_lines(const SDL_FPoint* pts, int size, const GYDM::RGBA& color);
        
        void draw_rect(SDL_FRect* box, const GYDM::RGBA& color);
        void fill_rect(SDL_FRect* box, const GYDM::RGBA& color);
        void draw_rect(float x, float y, float width, float height, const GYDM::RGBA& color);
        void fill_rect(float x, float y, float width, float height, const GYDM::RGBA& color);
        
        void draw_rounded_rect(SDL_FRect* box, float rad, const GYDM::RGBA& color);
        void fill_rounded_rect(SDL_FRect* box, float rad, const GYDM::RGBA& color);
        void draw_rounded_rect(float x, float y, float width, float height, float rad, const GYDM::RGBA& color);
        void fill_rounded_rect(float x, float y, float width, float height, float rad, const GYDM::RGBA& color);
        
        void draw_square(float cx, float cy, float apothem, const GYDM::RGBA& color);
        void fill_square(float cx, float cy, float apothem, const GYDM::RGBA& color);
        
        void draw_circle(float cx, float cy, float radius, const GYDM::RGBA& color);
        void fill_circle(float cx, float cy, float radius, const GYDM::RGBA& color);
        
        void draw_rounded_square(float cx, float cy, float apothem, float rad, const GYDM::RGBA& color);
        void fill_rounded_square(float cx, float cy, float apothem, float rad, const GYDM::RGBA& color);
        
        void draw_ellipse(float cx, float cy, float aradius, float bradius, const GYDM::RGBA& color);
        void fill_ellipse(float cx, float cy, float aradius, float bradius, const GYDM::RGBA& color);
        
        void draw_regular_polygon(int n, float cx, float cy, float radius, float rotation, const GYDM::RGBA& color);
        void fill_regular_polygon(int n, float cx, float cy, float radius, float rotation, const GYDM::RGBA& color);

    public:
        void disable_font_selection(bool yes) { this->_disable_font_selection = yes; }

        SDL_Texture* create_solid_text(const std::string& text, const shared_font_t& font, const GYDM::RGBA& fgc, int wrap = 0);
        SDL_Texture* create_shaded_text(const std::string& text, const shared_font_t& font, const GYDM::RGBA& fgc, const GYDM::RGBA& bgc, int wrap = 0);
        SDL_Texture* create_lcd_text(const std::string& text, const shared_font_t& font, const GYDM::RGBA& fgc, const GYDM::RGBA& bgc, int wrap = 0);
        SDL_Texture* create_blended_text(const std::string& text, const shared_font_t& font, const GYDM::RGBA& fgc, int wrap = 0);

        void draw_solid_text(const std::string& text, const shared_font_t& font, float x, float y, const GYDM::RGBA& rgb, int wrap = 0);
        void draw_shaded_text(const std::string& text, const shared_font_t& font, float x, float y, const GYDM::RGBA& fgc, const GYDM::RGBA& bgc, int wrap = 0);
        void draw_lcd_text(const std::string& text, const shared_font_t& font, float x, float y, const GYDM::RGBA& fgc, const GYDM::RGBA& bgc, int wrap = 0);
        void draw_blended_text(const std::string& text, const shared_font_t& font, float x, float y, const GYDM::RGBA& rgb, int wrap = 0);

        void draw_solid_text(const std::string& text, const shared_font_t& font, int x, int y, const GYDM::RGBA& rgb, int wrap = 0);
        void draw_shaded_text(const std::string& text, const shared_font_t& font, int x, int y, const GYDM::RGBA& fgc, const GYDM::RGBA& bgc, int wrap = 0);
        void draw_lcd_text(const std::string& text, const shared_font_t& font, int x, int y, const GYDM::RGBA& fgc, const GYDM::RGBA& bgc, int wrap = 0);
        void draw_blended_text(const std::string& text, const shared_font_t& font, int x, int y, const GYDM::RGBA& rgb, int wrap = 0);

    private:
        SDL_Texture* create_text_texture(const std::string& text, const shared_font_t& font, GYDM::TextRenderMode mode, const GYDM::RGBA& fgc, const GYDM::RGBA& bgc, int wrap = 0);

    private:
        bool _disable_font_selection = false;
        SDL_RendererInfo info;
        SDL_Renderer* device = nullptr;
    };

    typedef DrawingContext dc_t;
}
