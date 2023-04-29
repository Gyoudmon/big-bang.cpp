#include <SDL2/SDL.h>                 // 放最前面以兼容 macOS

#include "text.hpp"
#include "font.hpp"
#include "geometry.hpp"
#include "colorspace.hpp"

#include "../datum/string.hpp"

#include <cstdarg>
#include <iostream>

using namespace WarGrey::STEM;

/*************************************************************************************************/
static bool disable_font_selection = false;

/*************************************************************************************************/
static inline void unsafe_utf8_size(const shared_font_t& font, int* width, int* height, const std::string& text) {
    if (TTF_SizeUTF8(font->self(), text.c_str(), width, height)) {
        fprintf(stderr, "无法计算文本尺寸: %s\n", TTF_GetError());
    }
}

static inline void setup_for_text(const std::string& text, unsigned int rgb, SDL_Color& c) {
    RGB_From_Hexadecimal(rgb, &c.r, &c.g, &c.b);
    c.a = 0xFFU;
}

template<typename T>
static inline void safe_render_text_surface(SDL_Renderer* target, SDL_Surface* message, T x, T y) {
    /** TODO: Cache the textures of text **/

    if (message != nullptr) {
        game_render_surface(target, message, x, y);
        SDL_FreeSurface(message);
    }
}

static SDL_Surface* solid_text_surface(const shared_font_t& font, uint32_t rgb, const std::string& text, int wrap) {
    SDL_Color text_color;

    setup_for_text(text, rgb, text_color);
    
    return game_text_surface(text, font, ::TextRenderMode::Solid, text_color, text_color, wrap);
}

static SDL_Surface* shaded_text_surface(const shared_font_t& font, uint32_t fgc, uint32_t bgc, const std::string& text, int wrap) {
    SDL_Color text_color, background_color;

    setup_for_text(text, fgc, text_color);
    setup_for_text(text, bgc, background_color);

    return game_text_surface(text, font, ::TextRenderMode::Shaded, text_color, background_color, wrap);
}

static SDL_Surface* lcd_text_surface(const shared_font_t& font, uint32_t fgc, uint32_t bgc, const std::string& text, int wrap) {
    SDL_Color text_color, background_color;

    setup_for_text(text, fgc, text_color);
    setup_for_text(text, bgc, background_color);

    return game_text_surface(text, font, ::TextRenderMode::LCD, text_color, background_color, wrap);
}

static SDL_Surface* blended_text_surface(const shared_font_t& font, uint32_t rgb, const std::string& text, int wrap) {
    SDL_Color text_color;

    setup_for_text(text, rgb, text_color);
    
    return game_text_surface(text, font, ::TextRenderMode::Blender, text_color, text_color, wrap);
}

static TTF_Font* select_font(const shared_font_t& sfont, const std::string& text) {
    shared_font_t f = sfont;

    if (!f->is_suitable(text)) {
        f = f->try_fallback_for_unicode();
    }

    if (!f->okay()) {
        f = GameFont::Default();
    }

    return f->self();
}

/*************************************************************************************************/
std::string WarGrey::STEM::game_create_string(const char* fmt, ...) {
    VSNPRINT(s, fmt);
    return s;
}

void WarGrey::STEM::game_disable_font_selection(bool yes) {
    disable_font_selection = yes;
}

/*************************************************************************************************/
void WarGrey::STEM::game_text_size(const shared_font_t& font, int* width, int* height, const char* fmt, ...) {
    VSNPRINT(text, fmt);
    game_text_size(font, width, height, text);
}

void WarGrey::STEM::game_text_size(const shared_font_t& font, int* width, int* height, const std::string& text) { 
    unsafe_utf8_size(font, width, height, text);
}

void WarGrey::STEM::game_text_size(const shared_font_t& font, float* width, float* height, const char* fmt, ...) {
    VSNPRINT(text, fmt);
    game_text_size(font, width, height, text);
}

void WarGrey::STEM::game_text_size(const shared_font_t& font, float* width, float* height, const std::string& text) { 
    int fxwidth, fxheight;

    game_text_size(font, &fxwidth, &fxheight, text);
    
    if (width != nullptr) {
        (*width) = float(fxwidth);
    }

    if (height != nullptr) {
        (*height) = float(fxheight);
    }
}

/*************************************************************************************************/
SDL_Surface* WarGrey::STEM::game_text_surface(const std::string& text, const shared_font_t& sfont, TextRenderMode mode, SDL_Color& fgc, SDL_Color& bgc, int wrap) {
    SDL_Surface* surface = nullptr;
    TTF_Font* font = (disable_font_selection) ? sfont->self() : select_font(sfont, text);

#ifndef __windows__
    if (wrap >= 0) { // will wrap by newline for 0
        switch (mode) {
            case ::TextRenderMode::Solid: surface = TTF_RenderUTF8_Solid_Wrapped(font, text.c_str(), fgc, wrap); break;
            case ::TextRenderMode::Shaded: surface = TTF_RenderUTF8_Shaded_Wrapped(font, text.c_str(), fgc, bgc, wrap); break;
            case ::TextRenderMode::Blender: surface = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), fgc, wrap); break;
            case ::TextRenderMode::LCD: surface = TTF_RenderUTF8_LCD_Wrapped(font, text.c_str(), fgc, bgc, wrap); break;
        }
    } else {
#endif
        switch (mode) {
            case ::TextRenderMode::Solid: surface = TTF_RenderUTF8_Solid(font, text.c_str(), fgc); break;
            case ::TextRenderMode::Shaded: surface = TTF_RenderUTF8_Shaded(font, text.c_str(), fgc, bgc); break;
            case ::TextRenderMode::Blender: surface = TTF_RenderUTF8_Blended(font, text.c_str(), fgc); break;
#ifndef __windows__
            case ::TextRenderMode::LCD: surface = TTF_RenderUTF8_LCD(font, text.c_str(), fgc, bgc); break;
#endif
        }

#ifndef __windows__
    }
#endif

    if (surface == nullptr) {
        fprintf(stderr, "无法渲染文本: %s\n", TTF_GetError());
    }

    return surface;
}

SDL_Texture* WarGrey::STEM::game_text_texture(SDL_Renderer* renderer, const std::string& text, const shared_font_t& font, TextRenderMode mode, SDL_Color& fgc, SDL_Color& bgc, int wrap) {
    SDL_Surface* surface = game_text_surface(text, font, mode, fgc, bgc, wrap);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);

    return texture;
}

SDL_Texture* WarGrey::STEM::game_solid_text_texture(SDL_Renderer* renderer, const std::string& text, const shared_font_t& font, uint32_t fgc, int wrap) {
    SDL_Color c;

    RGB_FillColor(&c, fgc);

    return game_text_texture(renderer, text, font, TextRenderMode::Solid, c, c, wrap);
}

SDL_Texture* WarGrey::STEM::game_shaded_text_texture(SDL_Renderer* renderer, const std::string& text, const shared_font_t& font, uint32_t fgc, uint32_t bgc, int wrap) {
    SDL_Color fc, bc;

    RGB_FillColor(&fc, fgc);
    RGB_FillColor(&bc, bgc);

    return game_text_texture(renderer, text, font, TextRenderMode::Shaded, fc, bc, wrap);
}

SDL_Texture* WarGrey::STEM::game_lcd_text_texture(SDL_Renderer* renderer, const std::string& text, const shared_font_t& font, uint32_t fgc, uint32_t bgc, int wrap) {
    SDL_Color fc, bc;

    RGB_FillColor(&fc, fgc);
    RGB_FillColor(&bc, bgc);

    return game_text_texture(renderer, text, font, TextRenderMode::LCD, fc, bc, wrap);
}

SDL_Texture* WarGrey::STEM::game_blended_text_texture(SDL_Renderer* renderer, const std::string& text, const shared_font_t& font, uint32_t fgc, int wrap) {
    SDL_Color c;

    RGB_FillColor(&c, fgc);

    return game_text_texture(renderer, text, font, TextRenderMode::Blender, c, c, wrap);
}

/*************************************************************************************************/
void WarGrey::STEM::game_draw_solid_text(const shared_font_t& font, SDL_Renderer* renderer, uint32_t rgb, int x, int y, const std::string& text, int wrap) {
    SDL_Surface* message = solid_text_surface(font, rgb, text, wrap);
    safe_render_text_surface(renderer, message, x, y);
}

void WarGrey::STEM::game_draw_shaded_text(const shared_font_t& font, SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc, int x, int y, const std::string& text, int wrap) {
    SDL_Surface* message = shaded_text_surface(font, fgc, bgc, text, wrap);
    safe_render_text_surface(renderer, message, x, y);
}

void WarGrey::STEM::game_draw_lcd_text(const shared_font_t& font, SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc, int x, int y, const std::string& text, int wrap) {
    SDL_Surface* message = lcd_text_surface(font, fgc, bgc, text, wrap);
    safe_render_text_surface(renderer, message, x, y);
}

void WarGrey::STEM::game_draw_blended_text(const shared_font_t& font, SDL_Renderer* renderer, uint32_t rgb, int x, int y, const std::string& text, int wrap) {
    SDL_Surface* message = blended_text_surface(font, rgb, text, wrap);
    safe_render_text_surface(renderer, message, x, y);
}

void WarGrey::STEM::game_draw_solid_text(const shared_font_t& font, SDL_Renderer* renderer, uint32_t rgb, int x, int y, const char* fmt, ...) {
    VSNPRINT(text, fmt);
    game_draw_solid_text(font, renderer, rgb, x, y, text);
}

void WarGrey::STEM::game_draw_shaded_text(const shared_font_t& font, SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc, int x, int y, const char* fmt, ...) {
    VSNPRINT(text, fmt);
    game_draw_shaded_text(font, renderer, fgc, bgc, x, y, text);
}

void WarGrey::STEM::game_draw_lcd_text(const shared_font_t& font, SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc, int x, int y, const char* fmt, ...) {
    VSNPRINT(text, fmt);
    game_draw_lcd_text(font, renderer, fgc, bgc, x, y, text);
}

void WarGrey::STEM::game_draw_blended_text(const shared_font_t& font, SDL_Renderer* renderer, uint32_t rgb, int x, int y, const char* fmt, ...) {
    VSNPRINT(text, fmt);
    game_draw_blended_text(font, renderer, rgb, x, y, text);
}

/*************************************************************************************************/
void WarGrey::STEM::game_draw_solid_text(const shared_font_t& font, SDL_Renderer* renderer, uint32_t rgb, float x, float y, const std::string& text, int wrap) {
    SDL_Surface* message = solid_text_surface(font, rgb, text, wrap);
    safe_render_text_surface(renderer, message, x, y);
}

void WarGrey::STEM::game_draw_shaded_text(const shared_font_t& font, SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc, float x, float y, const std::string& text, int wrap) {
    SDL_Surface* message = shaded_text_surface(font, fgc, bgc, text, wrap);
    safe_render_text_surface(renderer, message, x, y);
}

void WarGrey::STEM::game_draw_lcd_text(const shared_font_t& font, SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc, float x, float y, const std::string& text, int wrap) {
    SDL_Surface* message = lcd_text_surface(font, fgc, bgc, text, wrap);
    safe_render_text_surface(renderer, message, x, y);
}

void WarGrey::STEM::game_draw_blended_text(const shared_font_t& font, SDL_Renderer* renderer, uint32_t rgb, float x, float y, const std::string& text, int wrap) {
    SDL_Surface* message = blended_text_surface(font, rgb, text, wrap);
    safe_render_text_surface(renderer, message, x, y);
}

void WarGrey::STEM::game_draw_solid_text(const shared_font_t& font, SDL_Renderer* renderer, uint32_t rgb, float x, float y, const char* fmt, ...) {
    VSNPRINT(text, fmt);
    game_draw_solid_text(font, renderer, rgb, x, y, text);
}

void WarGrey::STEM::game_draw_shaded_text(const shared_font_t& font, SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc, float x, float y, const char* fmt, ...) {
    VSNPRINT(text, fmt);
    game_draw_shaded_text(font, renderer, fgc, bgc, x, y, text);
}

void WarGrey::STEM::game_draw_lcd_text(const shared_font_t& font, SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc, float x, float y, const char* fmt, ...) {
    VSNPRINT(text, fmt);
    game_draw_lcd_text(font, renderer, fgc, bgc, x, y, text);
}

void WarGrey::STEM::game_draw_blended_text(const shared_font_t& font, SDL_Renderer* renderer, uint32_t rgb, float x, float y, const char* fmt, ...) {
    VSNPRINT(text, fmt);
    game_draw_blended_text(font, renderer, rgb, x, y, text);
}
