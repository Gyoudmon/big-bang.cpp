#pragma once

#include <SDL2/SDL_ttf.h>

#include <string>
#include <cstdint>

#include "font.hpp"
#include "color.hpp"

namespace WarGrey::STEM {
    enum TextRenderMode { Solid, Shaded, Blender, LCD };

    /*********************************************************************************************/
    __lambda__ void game_disable_font_selection(bool yes);

    /*********************************************************************************************/
    __lambda__ SDL_Surface* game_text_surface(const std::string& text, const shared_font_t& font, WarGrey::STEM::TextRenderMode mode, const WarGrey::STEM::RGBA& fgc, const WarGrey::STEM::RGBA& bgc, int wrap = 0);
    __lambda__ SDL_Texture* game_text_texture(SDL_Renderer* renderer, const std::string& text, const shared_font_t& font, WarGrey::STEM::TextRenderMode mode, const WarGrey::STEM::RGBA& fgc, const WarGrey::STEM::RGBA& bgc, int wrap = 0);

    __lambda__ SDL_Texture* game_solid_text_texture(SDL_Renderer* renderer, const std::string& text, const shared_font_t& font, const WarGrey::STEM::RGBA& fgc, int wrap = 0);
    __lambda__ SDL_Texture* game_shaded_text_texture(SDL_Renderer* renderer, const std::string& text, const shared_font_t& font, const WarGrey::STEM::RGBA& fgc, const WarGrey::STEM::RGBA& bgc, int wrap = 0);
    __lambda__ SDL_Texture* game_lcd_text_texture(SDL_Renderer* renderer, const std::string& text, const shared_font_t& font, const WarGrey::STEM::RGBA& fgc, const WarGrey::STEM::RGBA& bgc, int wrap = 0);
    __lambda__ SDL_Texture* game_blended_text_texture(SDL_Renderer* renderer, const std::string& text, const shared_font_t& font, const WarGrey::STEM::RGBA& fgc, int wrap = 0);

    class __lambda__ Pen {
    public:
        static void draw_solid_text(const shared_font_t& font, SDL_Renderer* renderer, const WarGrey::STEM::RGBA& rgb, int x, int y, const std::string& text, int wrap = 0);
        static void draw_shaded_text(const shared_font_t& font, SDL_Renderer* renderer, const WarGrey::STEM::RGBA& fgc, const WarGrey::STEM::RGBA& bgc, int x, int y, const std::string& text, int wrap = 0);
        static void draw_lcd_text(const shared_font_t& font, SDL_Renderer* renderer, const WarGrey::STEM::RGBA& fgc, const WarGrey::STEM::RGBA& bgc, int x, int y, const std::string& text, int wrap = 0);
        static void draw_blended_text(const shared_font_t& font, SDL_Renderer* renderer, const WarGrey::STEM::RGBA& rgb, int x, int y, const std::string& text, int wrap = 0);

        static void draw_solid_text(const shared_font_t& font, SDL_Renderer* renderer, const WarGrey::STEM::RGBA& rgb, float x, float y, const std::string& text, int wrap = 0);
        static void draw_shaded_text(const shared_font_t& font, SDL_Renderer* renderer, const WarGrey::STEM::RGBA& fgc, const WarGrey::STEM::RGBA& bgc, float x, float y, const std::string& text, int wrap = 0);
        static void draw_lcd_text(const shared_font_t& font, SDL_Renderer* renderer, const WarGrey::STEM::RGBA& fgc, const WarGrey::STEM::RGBA& bgc, float x, float y, const std::string& text, int wrap = 0);
        static void draw_blended_text(const shared_font_t& font, SDL_Renderer* renderer, const WarGrey::STEM::RGBA& rgb, float x, float y, const std::string& text, int wrap = 0);
    };
}
