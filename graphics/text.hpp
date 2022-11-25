#pragma once

#include <SDL2/SDL_ttf.h>

#include <string>
#include <cstdint>

namespace WarGrey::STEM {
    enum TextRenderMode { Solid, Shaded, Blender, LCD };

    std::string game_create_string(const char* fmt, ...);
    SDL_Surface* game_text_surface(const std::string& text, TTF_Font* font, WarGrey::STEM::TextRenderMode, SDL_Color& fgc, SDL_Color& bgc, int wrap);

    /*********************************************************************************************/
    void game_text_size(TTF_Font* font, int* width, int* height, const char* fmt, ...);
    void game_text_size(TTF_Font* font, int* width, int* height, const std::string& t);
    
    void game_draw_solid_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t rgb, int x, int y, const std::string& text, int wrap = 0);
    void game_draw_shaded_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc, int x, int y, const std::string& text, int wrap = 0);
    void game_draw_lcd_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc, int x, int y, const std::string& text, int wrap = 0);
    void game_draw_blended_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t rgb, int x, int y, const std::string& text, int wrap = 0);

    void game_draw_solid_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t rgb, int x, int y, const char* fmt, ...);
    void game_draw_shaded_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc, int x, int y, const char* fmt, ...);
    void game_draw_lcd_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc, int x, int y, const char* fmt, ...);
    void game_draw_blended_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t rgb, int x, int y, const char* fmt, ...);
    
    /*********************************************************************************************/
    void game_text_size(TTF_Font* font, float* width, float* height, const char* fmt, ...);
    void game_text_size(TTF_Font* font, float* width, float* height, const std::string& t);
    
    void game_draw_solid_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t rgb, float x, float y, const std::string& text, int wrap = 0);
    void game_draw_shaded_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc, float x, float y, const std::string& text, int wrap = 0);
    void game_draw_lcd_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc, float x, float y, const std::string& text, int wrap = 0);
    void game_draw_blended_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t rgb, float x, float y, const std::string& text, int wrap = 0);

    void game_draw_solid_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t rgb, float x, float y, const char* fmt, ...);
    void game_draw_shaded_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc, float x, float y, const char* fmt, ...);
    void game_draw_lcd_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc, float x, float y, const char* fmt, ...);
    void game_draw_blended_text(TTF_Font* font, SDL_Renderer* renderer, uint32_t rgb, float x, float y, const char* fmt, ...);
}

