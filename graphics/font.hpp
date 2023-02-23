#pragma once

#include <SDL2/SDL_ttf.h>

#include <string>

namespace WarGrey::STEM {
    class game_font {
        public:
            static TTF_Font* DEFAULT;
            static TTF_Font* title;
            static TTF_Font* sans_serif;
            static TTF_Font* serif;
            static TTF_Font* monospace;
            static TTF_Font* math;
            static TTF_Font* unicode;
    };

    void game_fonts_initialize(int fontsize);
    void game_fonts_destroy();

    TTF_Font* game_create_font(const char* basename, int fontsize);
    TTF_Font* game_create_font(const char* basename, float fontsize);
    void game_font_destroy(TTF_Font* font, bool usr_only = true);
    const std::string* game_font_list(int* n, int fontsize = 16);

    const char* font_basename(const TTF_Font* font);

    void feed_text_extent(TTF_Font* font, const char* unicode, int* width = nullptr, int* height = nullptr);

    int font_width(TTF_Font* font, const char* unicode);
    int font_height(TTF_Font* font);
    int font_ascent(TTF_Font* font);
    int font_descent(TTF_Font* font);
}
