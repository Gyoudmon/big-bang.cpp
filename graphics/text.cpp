#include <SDL2/SDL.h>                 // 放最前面以兼容 macOS

#include "text.hpp"
#include "font.hpp"
#include "brush.hpp"

#include <cstdarg>
#include <iostream>

using namespace GYDM;

/*************************************************************************************************/
static bool disable_font_selection = false;

/*************************************************************************************************/
static inline void setup_for_text(const std::string& text, const RGBA& rgb, SDL_Color& c) {
    rgb.unbox(&c.r, &c.g, &c.b, &c.a);
}

template<typename T>
static inline void safe_render_text_surface(SDL_Renderer* target, SDL_Surface* message, T x, T y) {
    /** TODO: Cache the textures of text **/

    if (message != nullptr) {
        Brush::stamp(target, message, x, y);
        SDL_FreeSurface(message);
    }
}

static SDL_Surface* solid_text_surface(const shared_font_t& font, const RGBA& rgb, const std::string& text, int wrap) {
    return game_text_surface(text, font, ::TextRenderMode::Solid, rgb, rgb, wrap);
}

static SDL_Surface* shaded_text_surface(const shared_font_t& font, const RGBA& fgc, const RGBA& bgc, const std::string& text, int wrap) {
    return game_text_surface(text, font, ::TextRenderMode::Shaded, fgc, bgc, wrap);
}

static SDL_Surface* lcd_text_surface(const shared_font_t& font, const RGBA& fgc, const RGBA& bgc, const std::string& text, int wrap) {
    return game_text_surface(text, font, ::TextRenderMode::LCD, fgc, bgc, wrap);
}

static SDL_Surface* blended_text_surface(const shared_font_t& font, const RGBA& rgb, const std::string& text, int wrap) {
    return game_text_surface(text, font, ::TextRenderMode::Blender, rgb, rgb, wrap);
}

/*************************************************************************************************/
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
void GYDM::game_disable_font_selection(bool yes) {
    disable_font_selection = yes;
}

/*************************************************************************************************/
SDL_Surface* GYDM::game_text_surface(const std::string& text, const shared_font_t& sfont, TextRenderMode mode, const RGBA& fg, const RGBA& bg, int wrap) {
    SDL_Surface* surface = nullptr;
    TTF_Font* font = (disable_font_selection) ? sfont->self() : select_font(sfont, text);
    SDL_Color fgc, bgc;

    setup_for_text(text, fg, fgc);
    setup_for_text(text, bg, bgc);

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

SDL_Texture* GYDM::game_text_texture(SDL_Renderer* renderer, const std::string& text, const shared_font_t& font, TextRenderMode mode, const RGBA& fgc, const RGBA& bgc, int wrap) {
    SDL_Surface* surface = game_text_surface(text, font, mode, fgc, bgc, wrap);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);

    return texture;
}

SDL_Texture* GYDM::game_solid_text_texture(SDL_Renderer* renderer, const std::string& text, const shared_font_t& font, const RGBA& fgc, int wrap) {
    return game_text_texture(renderer, text, font, TextRenderMode::Solid, fgc, fgc, wrap);
}

SDL_Texture* GYDM::game_shaded_text_texture(SDL_Renderer* renderer, const std::string& text, const shared_font_t& font, const RGBA& fgc, const RGBA& bgc, int wrap) {
    return game_text_texture(renderer, text, font, TextRenderMode::Shaded, fgc, bgc, wrap);
}

SDL_Texture* GYDM::game_lcd_text_texture(SDL_Renderer* renderer, const std::string& text, const shared_font_t& font, const RGBA& fgc, const RGBA& bgc, int wrap) {
    return game_text_texture(renderer, text, font, TextRenderMode::LCD, fgc, bgc, wrap);
}

SDL_Texture* GYDM::game_blended_text_texture(SDL_Renderer* renderer, const std::string& text, const shared_font_t& font, const RGBA& fgc, int wrap) {
    return game_text_texture(renderer, text, font, TextRenderMode::Blender, fgc, fgc, wrap);
}

/**************************************************************************************************/
void GYDM::Pen::draw_solid_text(const shared_font_t& font, SDL_Renderer* renderer, const RGBA& rgb, int x, int y, const std::string& text, int wrap) {
    SDL_Surface* message = solid_text_surface(font, rgb, text, wrap);
    safe_render_text_surface(renderer, message, x, y);
}

void GYDM::Pen::draw_shaded_text(const shared_font_t& font, SDL_Renderer* renderer, const RGBA& fgc, const RGBA& bgc, int x, int y, const std::string& text, int wrap) {
    SDL_Surface* message = shaded_text_surface(font, fgc, bgc, text, wrap);
    safe_render_text_surface(renderer, message, x, y);
}

void GYDM::Pen::draw_lcd_text(const shared_font_t& font, SDL_Renderer* renderer, const RGBA& fgc, const RGBA& bgc, int x, int y, const std::string& text, int wrap) {
    SDL_Surface* message = lcd_text_surface(font, fgc, bgc, text, wrap);
    safe_render_text_surface(renderer, message, x, y);
}

void GYDM::Pen::draw_blended_text(const shared_font_t& font, SDL_Renderer* renderer, const RGBA& rgb, int x, int y, const std::string& text, int wrap) {
    SDL_Surface* message = blended_text_surface(font, rgb, text, wrap);
    safe_render_text_surface(renderer, message, x, y);
}

void GYDM::Pen::draw_solid_text(const shared_font_t& font, SDL_Renderer* renderer, const RGBA& rgb, float x, float y, const std::string& text, int wrap) {
    SDL_Surface* message = solid_text_surface(font, rgb, text, wrap);
    safe_render_text_surface(renderer, message, x, y);
}

void GYDM::Pen::draw_shaded_text(const shared_font_t& font, SDL_Renderer* renderer, const RGBA& fgc, const RGBA& bgc, float x, float y, const std::string& text, int wrap) {
    SDL_Surface* message = shaded_text_surface(font, fgc, bgc, text, wrap);
    safe_render_text_surface(renderer, message, x, y);
}

void GYDM::Pen::draw_lcd_text(const shared_font_t& font, SDL_Renderer* renderer, const RGBA& fgc, const RGBA& bgc, float x, float y, const std::string& text, int wrap) {
    SDL_Surface* message = lcd_text_surface(font, fgc, bgc, text, wrap);
    safe_render_text_surface(renderer, message, x, y);
}

void GYDM::Pen::draw_blended_text(const shared_font_t& font, SDL_Renderer* renderer, const RGBA& rgb, float x, float y, const std::string& text, int wrap) {
    SDL_Surface* message = blended_text_surface(font, rgb, text, wrap);
    safe_render_text_surface(renderer, message, x, y);
}
