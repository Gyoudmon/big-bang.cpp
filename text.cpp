#include "game.hpp"                 // 放最前面以兼容 macOS
#include "text.hpp"
#include "vsntext.hpp"
#include "colorspace.hpp"

#include <cstdarg>
#include <iostream>

using namespace WarGrey::STEM;

/*************************************************************************************************/
enum TextRenderMode { Solid, Shaded, Blender, LCD };

/*************************************************************************************************/
static inline void unsafe_utf8_size(TTF_Font* font, int* width, int* height, const std::string& text) {
    if (TTF_SizeUTF8(font, text.c_str(), width, height)) {
        fprintf(stderr, "无法计算文本尺寸: %s\n", TTF_GetError());
    }
}

static inline void setup_for_text(TTF_Font* font, const std::string& text, unsigned int rgb, SDL_Color& c) {
    RGB_FromHexadecimal(rgb, &c.r, &c.g, &c.b);
    c.a = 0xFFU;
}

static SDL_Surface* game_text_surface(const std::string& text, ::TextRenderMode mode, TTF_Font* font, SDL_Color& fgc, SDL_Color& bgc, int wrap) {
    SDL_Surface* surface = NULL;

    if (wrap >= 0) {
        switch (mode) {
            case ::TextRenderMode::Solid: surface = TTF_RenderUTF8_Solid_Wrapped(font, text.c_str(), fgc, wrap); break;
            case ::TextRenderMode::Shaded: surface = TTF_RenderUTF8_Shaded_Wrapped(font, text.c_str(), fgc, bgc, wrap); break;
            case ::TextRenderMode::LCD: surface = TTF_RenderUTF8_LCD_Wrapped(font, text.c_str(), fgc, bgc, wrap); break;
            case ::TextRenderMode::Blender: surface = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), fgc, wrap); break;
        }
    } else {
        switch (mode) {
            case ::TextRenderMode::Solid: surface = TTF_RenderUTF8_Solid(font, text.c_str(), fgc); break;
            case ::TextRenderMode::Shaded: surface = TTF_RenderUTF8_Shaded(font, text.c_str(), fgc, bgc); break;
            case ::TextRenderMode::LCD: surface = TTF_RenderUTF8_LCD(font, text.c_str(), fgc, bgc); break;
            case ::TextRenderMode::Blender: surface = TTF_RenderUTF8_Blended(font, text.c_str(), fgc); break;
        }
    }

    if (surface == NULL) {
        fprintf(stderr, "无法渲染文本: %s\n", TTF_GetError());
    }

    return surface;
}

static inline void safe_render_text_surface(SDL_Renderer* target, SDL_Surface* message, int x, int y) {
    /** TODO: Cache the textures of text **/

    if (message != NULL) {
        game_render_surface(target, message, x, y);
        SDL_FreeSurface(message);
    }
}

/*************************************************************************************************/
std::string WarGrey::STEM::game_create_string(const char* fmt, ...) {
    VSNPRINT(s, fmt);

    return s;
}

/*************************************************************************************************/
void WarGrey::STEM::game_text_size(TTF_Font* font, int* width, int* height, const char* fmt, ...) {
    VSNPRINT(text, fmt);
    game_text_size(font, width, height, text);
}

void WarGrey::STEM::game_text_size(TTF_Font* font, int* width, int* height, const std::string& text) { 
    if (font == NULL) {
        font = GAME_DEFAULT_FONT;
    }

    unsafe_utf8_size(font, width, height, text);
}

    
void WarGrey::STEM::game_draw_solid_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t rgb, int x, int y, const std::string& text, int wrap) {
    SDL_Color text_color;

    if (font == NULL) {
        font = GAME_DEFAULT_FONT;
    }

    setup_for_text(font, text, rgb, text_color);
    SDL_Surface* message = game_text_surface(text, ::TextRenderMode::Solid, font, text_color, text_color, wrap);
    safe_render_text_surface(renderer, message, x, y);
}

void WarGrey::STEM::game_draw_shaded_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc, int x, int y, const std::string& text, int wrap) {
    SDL_Color text_color, background_color;

    if (font == NULL) {
        font = GAME_DEFAULT_FONT;
    }

    setup_for_text(font, text, fgc, text_color);
    setup_for_text(font, text, bgc, background_color);
    SDL_Surface* message = game_text_surface(text, ::TextRenderMode::Shaded, font, text_color, background_color, wrap);
    safe_render_text_surface(renderer, message, x, y);
}

void WarGrey::STEM::game_draw_lcd_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc, int x, int y, const std::string& text, int wrap) {
    SDL_Color text_color, background_color;

    if (font == NULL) {
        font = GAME_DEFAULT_FONT;
    }

    setup_for_text(font, text, fgc, text_color);
    setup_for_text(font, text, bgc, background_color);
    SDL_Surface* message = game_text_surface(text, ::TextRenderMode::LCD, font, text_color, background_color, wrap);
    safe_render_text_surface(renderer, message, x, y);
}

void WarGrey::STEM::game_draw_blended_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t rgb, int x, int y, const std::string& text, int wrap) {
    SDL_Color text_color;

    if (font == NULL) {
        font = GAME_DEFAULT_FONT;
    }

    setup_for_text(font, text, rgb, text_color);
    SDL_Surface* message = game_text_surface(text, ::TextRenderMode::Blender, font, text_color, text_color, wrap);
    safe_render_text_surface(renderer, message, x, y);
}

void WarGrey::STEM::game_draw_solid_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t rgb, int x, int y, const char* fmt, ...) {
    VSNPRINT(text, fmt);
    game_draw_solid_text(font, renderer, rgb, x, y, text);
}

void WarGrey::STEM::game_draw_shaded_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc, int x, int y, const char* fmt, ...) {
    VSNPRINT(text, fmt);
    game_draw_shaded_text(font, renderer, fgc, bgc, x, y, text);
}

void WarGrey::STEM::game_draw_lcd_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc, int x, int y, const char* fmt, ...) {
    VSNPRINT(text, fmt);
    game_draw_lcd_text(font, renderer, fgc, bgc, x, y, text);
}

void WarGrey::STEM::game_draw_blended_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t rgb, int x, int y, const char* fmt, ...) {
    VSNPRINT(text, fmt);
    game_draw_blended_text(font, renderer, rgb, x, y, text);
}

