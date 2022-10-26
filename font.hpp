#pragma once

#include <SDL2/SDL_ttf.h>

#include <string>

namespace WarGrey::STEM {
    extern TTF_Font* GAME_DEFAULT_FONT;
    extern TTF_Font* game_sans_serif_font;
    extern TTF_Font* game_serif_font;
    extern TTF_Font* game_monospace_font;
    extern TTF_Font* game_math_font;
    extern TTF_Font* game_unicode_font;

    void game_fonts_initialize(int fontsize = 16);
    void game_fonts_destroy();

    TTF_Font* game_create_font(const char* face, int fontsize);
    void game_font_destroy(TTF_Font* font, bool usr_only = true);
    const std::string* game_font_list(int* n, int fontsize = 16);
}

