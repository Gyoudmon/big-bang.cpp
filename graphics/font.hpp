#pragma once

#include <SDL2/SDL_ttf.h>

#include <string>
#include <memory>

namespace WarGrey::STEM {
    // https://www.w3.org/TR/css-fonts-4
    enum class FontSize { xx_small, x_small, small, medium, large, x_large, xx_large };
    enum class FontFamily { sans_serif, serif, cursive, fantasy, math, monospace, fangsong };

    class GameFont {
    public:
        static void fontsize(int fontsize = 16);

        static std::shared_ptr<GameFont> Title();
        static std::shared_ptr<GameFont> Tooltip();

        static std::shared_ptr<GameFont> Default(FontSize absize = FontSize::medium);
        static std::shared_ptr<GameFont> sans_serif(FontSize absize = FontSize::medium);
        static std::shared_ptr<GameFont> serif(FontSize absize = FontSize::medium);
        static std::shared_ptr<GameFont> cursive(FontSize absize = FontSize::medium);
        static std::shared_ptr<GameFont> fantasy(FontSize absize = FontSize::medium);
        static std::shared_ptr<GameFont> monospace(FontSize absize = FontSize::medium);
        static std::shared_ptr<GameFont> math(FontSize absize = FontSize::medium);
        static std::shared_ptr<GameFont> fangsong(FontSize absize = FontSize::medium);

        static std::shared_ptr<GameFont> Default(int ftsize);
        static std::shared_ptr<GameFont> sans_serif(int ftsize);
        static std::shared_ptr<GameFont> serif(int ftsize);
        static std::shared_ptr<GameFont> cursive(int ftsize);
        static std::shared_ptr<GameFont> fantasy(int ftsize);
        static std::shared_ptr<GameFont> monospace(int ftsize);
        static std::shared_ptr<GameFont> math(int ftsize);
        static std::shared_ptr<GameFont> fangsong(int ftsize);

    public:
        GameFont(TTF_Font* raw) : font(raw) {}
        virtual ~GameFont() { if (this->okay()) TTF_CloseFont(this->font); }

    public:
        bool okay() { return this->font != nullptr; }
        TTF_Font* self() { return this->font; }

    public:
        const char* basename();
        void feed_text_extent(const char* unicode, int* width = nullptr, int* height = nullptr);

        bool is_suitable(const std::string& text);
        int width(const char* unicode);
        int height();
        int ascent();
        int descent();
        
    private:
        TTF_Font* font = nullptr;
    };

    typedef std::shared_ptr<GameFont> shared_font_t;
    static shared_font_t invalid_font = std::make_shared<GameFont>(nullptr);

    /*********************************************************************************************/
    void game_fonts_initialize();
    void game_fonts_destroy();

    int generic_font_size(FontSize size);
    const char* generic_font_family_name(FontFamily family);

    shared_font_t game_create_shared_font(const char* basename, int fontsize);
    shared_font_t game_create_shared_font(const char* basename, float fontsize);
    TTF_Font* game_create_font(const char* basename, int fontsize);
    TTF_Font* game_create_font(const char* basename, float fontsize);
    void game_destory_font(TTF_Font* font);
    const std::string* game_fontname_list(int* n, int fontsize = 16);
}
