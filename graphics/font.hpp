#pragma once

#include <SDL2/SDL_ttf.h>

#include <string>

namespace WarGrey::STEM {
    class game_font {
        public:
            static TTF_Font* DEFAULT;
            static TTF_Font* sans_serif;
            static TTF_Font* serif;
            static TTF_Font* monospace;
            static TTF_Font* math;
            static TTF_Font* unicode;
    };

    void game_fonts_initialize(int fontsize = 16);
    void game_fonts_destroy();

    TTF_Font* game_create_font(const char* face, int fontsize);
    void game_font_destroy(TTF_Font* font, bool usr_only = true);
    const std::string* game_font_list(int* n, int fontsize = 16);
}

