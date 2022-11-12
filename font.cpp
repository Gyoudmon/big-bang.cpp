#include "font.hpp"

#include <unordered_map>
#include <filesystem>

using namespace WarGrey::STEM;
using namespace std::filesystem;

/*************************************************************************************************/
#define Game_Close_Font(id) if (id != nullptr) TTF_CloseFont(id); id = nullptr;

TTF_Font* WarGrey::STEM::game_font::DEFAULT = nullptr;
TTF_Font* WarGrey::STEM::game_font::sans_serif = nullptr;
TTF_Font* WarGrey::STEM::game_font::serif = nullptr;
TTF_Font* WarGrey::STEM::game_font::monospace = nullptr;
TTF_Font* WarGrey::STEM::game_font::math = nullptr;
TTF_Font* WarGrey::STEM::game_font::unicode = nullptr;

/*************************************************************************************************/
static std::unordered_map<std::string, std::string> system_fonts;
static std::string system_fontdirs[] = {
    "/System/Library/Fonts",
    "/Library/Fonts",
    "C:\\Windows\\Fonts",
    "/usr/share/fonts"
};

static void game_push_fonts_of_directory(path& root) {
    for (auto entry : directory_iterator(root)) {
        path self = entry.path();

        if (entry.is_directory()) {
            game_push_fonts_of_directory(self);
        } else if (entry.is_regular_file()) {
            system_fonts[self.filename().string()] = self.string();
        }
    }
}

/*************************************************************************************************/
void WarGrey::STEM::game_fonts_initialize(int fontsize) {
    for (unsigned int idx = 0; idx < sizeof(system_fontdirs) / sizeof(std::string); idx++) {
        path root(system_fontdirs[idx]);

        if (exists(root) && is_directory(root)) {
            game_push_fonts_of_directory(root);
        }
    }

#if defined(__macosx__)
    game_font::sans_serif = game_create_font("LucidaGrande.ttc", fontsize);
    game_font::serif = game_create_font("Times.ttc", fontsize);
    game_font::monospace = game_create_font("Courier.ttc", fontsize);
    game_font::math = game_create_font("Bodoni 72.ttc", fontsize);
    game_font::unicode = game_create_font("PingFang.ttc", fontsize);
#elif defined(__windows__) /* HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\Fonts */
    game_font::sans_serif = game_create_font("msyh.ttc", fontsize); // Microsoft YaHei
    game_font::serif = game_create_font("times.ttf", fontsize); // Times New Roman
    game_font::monospace = game_create_font("cour.ttf", fontsize); // Courier New
    game_font::math = game_create_font("BOD_R.TTF", fontsize); // Bodoni MT
    game_font::unicode = game_create_font("msyh.ttc", fontsize);
#else /* the following fonts have not been tested */
    game_font::sans_serif = game_create_font("Nimbus Sans.ttc", fontsize);
    game_font::serif = game_create_font("DejaVu Serif.ttc", fontsize);
    game_font::monospace = game_create_font("Monospace.ttf", fontsize);
    game_font::math = game_create_font("URW Bookman.ttf", fontsize);
    game_font::unicode = game_create_font("Arial Unicode.ttf", fontsize);
#endif

    game_font::DEFAULT = game_font::sans_serif;
}

void WarGrey::STEM::game_fonts_destroy() {
    Game_Close_Font(game_font::sans_serif);
    Game_Close_Font(game_font::serif);
    Game_Close_Font(game_font::monospace);
    Game_Close_Font(game_font::math);
    Game_Close_Font(game_font::unicode);
}

/*************************************************************************************************/
TTF_Font* WarGrey::STEM::game_create_font(const char* face, int fontsize) {
    std::string face_key(face);
    TTF_Font* font = nullptr;

#if defined(__macosx__)
    if (system_fonts.find(face_key) == system_fonts.end()) {
        font = TTF_OpenFontDPI(face, fontsize / 2, 198, 198);
    } else {
        font = TTF_OpenFontDPI(system_fonts[face_key].c_str(), fontsize / 2, 198, 198);
    }
#else
    if (system_fonts.find(face_key) == system_fonts.end()) {
        font = TTF_OpenFont(face, fontsize);
    } else {
        font = TTF_OpenFont(system_fonts[face_key].c_str(), fontsize);
    }
#endif

    if (font == nullptr) {
        fprintf(stderr, "无法加载字体 '%s': %s\n", face, TTF_GetError());
    }

    return font;
}

void WarGrey::STEM::game_font_destroy(TTF_Font* font, bool usr_only) {
    if (font != nullptr) {
        if (!usr_only) {
            TTF_CloseFont(font);
        } else if (font == game_font::DEFAULT) {
        } else if (font == game_font::sans_serif) {
        } else if (font == game_font::serif) {
        } else if (font == game_font::monospace) {
        } else if (font == game_font::math) {
        } else if (font == game_font::unicode) {
        } else {
            TTF_CloseFont(font);
        }
    }
}

const std::string* WarGrey::STEM::game_font_list(int* n, int fontsize) {
    static std::string* font_list = new std::string[system_fonts.size()];
    static int i = 0;

    if (i == 0) {
        for (std::pair<std::string, std::string> k_v : system_fonts) {
            TTF_Font* f = TTF_OpenFont(k_v.second.c_str(), fontsize);

            if (f != nullptr) {
                font_list[i ++] = k_v.first;
                
                // because of insufficient resources to open all fonts
                TTF_CloseFont(f);
            }
        }
    }
    
    if (n != nullptr) {
        (*n) = i;
    }

    return (const std::string*)font_list;
}

