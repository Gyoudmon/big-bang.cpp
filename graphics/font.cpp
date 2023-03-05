#include "font.hpp"

#include <map>
#include <unordered_map>
#include <filesystem>

#include "../datum/flonum.hpp"
#include "../datum/string.hpp"

using namespace WarGrey::STEM;
using namespace std::filesystem;

/*************************************************************************************************/
typedef std::tuple<std::string, int> font_key_t;

static std::map<font_key_t, shared_font_t> fontdb;

static int medium_fontsize = 16;

// GameFont::Title "Hiragino Sans GB.ttc";
// GameFont::DEFAULT = GameFont::sans_serif;

/*************************************************************************************************/
static std::unordered_map<std::string, std::string> system_fonts;
static std::unordered_map<std::string, std::string> basenames;

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
void WarGrey::STEM::game_fonts_initialize() {
    for (unsigned int idx = 0; idx < sizeof(system_fontdirs) / sizeof(std::string); idx++) {
        path root(system_fontdirs[idx]);

        if (exists(root) && is_directory(root)) {
            game_push_fonts_of_directory(root);
        }
    }
}

void WarGrey::STEM::game_fonts_destroy() { /* nothing has to be done */}

int WarGrey::STEM::generic_font_size(FontSize size) {
    switch (size) {
        case FontSize::xx_large: return medium_fontsize * 2;
        case FontSize::x_large:  return medium_fontsize * 3 / 2;
        case FontSize::large:    return medium_fontsize * 6 / 5;
        case FontSize::small:    return medium_fontsize * 8 / 9;
        case FontSize::x_small:  return medium_fontsize * 3 / 4;
        case FontSize::xx_small: return medium_fontsize * 3 / 5;
        default: return medium_fontsize;
    }
}

const char* WarGrey::STEM::generic_font_family_name(FontFamily family) {
    switch (family) {
#if defined(__macosx__)
    case FontFamily::sans_serif: return "LucidaGrande.ttc"; break;
    case FontFamily::serif: return "Times.ttc"; break;
    case FontFamily::monospace: return "Courier.ttc"; break;
    case FontFamily::math: return "Bodoni 72.ttc"; break;
    case FontFamily::cursive: return "Courier.ttc"; break;
    case FontFamily::fantasy: return "Bodoni 72.ttc"; break;

    /* This is the only font that is elegant, although some characters are missing */
    case FontFamily::fangsong: return "PingFang.ttc"; break;
#elif defined(__windows__) /* HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\Fonts */
    case FontFamily::sans_serif: return "msyh.ttc"; break; // Microsoft YaHei
    case FontFamily::serif: return "times.ttf"; break; // Times New Roman
    case FontFamily::monospace: return "cour.ttf"; break; // Courier New
    case FontFamily::math: return "BOD_R.TTF"; break; // Bodoni MT
    case FontFamily::cursive: return "Courier.ttc"; break;
    case FontFamily::fantasy: return "Bodoni 72.ttc"; break;
    case FontFamily::fangsong: return "msyh.ttc"; break;
#else /* the following fonts have not been tested */
    case FontFamily::sans_serif: return "Nimbus Sans.ttc"; break;
    case FontFamily::serif: return "DejaVu Serif.ttc"; break;
    case FontFamily::monospace: return "Monospace.ttf"; break;
    case FontFamily::math: return "URW Bookman.ttf"; break;
    case FontFamily::fangsong: return "Arial Unicode.ttf"; break;
    case FontFamily::cursive: return "Chancery.ttf"; break;
    case FontFamily::fantasy: return "Helvetica.ttf"; break;
#endif
    default: return "";
    }
}

/*************************************************************************************************/
shared_font_t WarGrey::STEM::game_create_font(const char* face, int fontsize) {
    std::string face_key(face);
    font_key_t font_key;
    
    if (system_fonts.find(face_key) == system_fonts.end()) {
        font_key = std::tuple<std::string, int>(face_key, fontsize);
    } else {
        font_key = std::tuple<std::string, int>(system_fonts[face_key], fontsize);
    }

    if (fontdb.find(font_key) == fontdb.end()) {
        TTF_Font* font = TTF_OpenFont(std::get<0>(font_key).c_str(), fontsize);

        if (font == nullptr) {
            fprintf(stderr, "无法加载字体 '%s': %s\n", face, TTF_GetError());
            fontdb[font_key] = null_font;
        } else {
            basenames[std::string(TTF_FontFaceFamilyName(font))] = std::string(face);
            fontdb[font_key] = std::make_shared<GameFont>(font);
        }
    }

    return fontdb.find(font_key)->second;
}

shared_font_t WarGrey::STEM::game_create_font(const char* face, float fontsize) {
    return game_create_font(face, fl2fxi(fontsize));
}

const std::string* WarGrey::STEM::game_fontname_list(int* n, int fontsize) {
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

    return static_cast<const std::string*>(font_list);
}

/*************************************************************************************************/
void WarGrey::STEM::GameFont::fontsize(int ftsize) {
    if (ftsize > 0) {
        medium_fontsize = ftsize;
    }
}

std::shared_ptr<GameFont> WarGrey::STEM::GameFont::sans_serif(FontSize absize) {
    return GameFont::sans_serif(generic_font_size(absize));
}

std::shared_ptr<GameFont> WarGrey::STEM::GameFont::serif(FontSize absize) {
    return GameFont::serif(generic_font_size(absize));
}

std::shared_ptr<GameFont> WarGrey::STEM::GameFont::cursive(FontSize absize) {
    return GameFont::cursive(generic_font_size(absize));
}

std::shared_ptr<GameFont> WarGrey::STEM::GameFont::fantasy(FontSize absize) {
    return GameFont::fantasy(generic_font_size(absize));
}

std::shared_ptr<GameFont> WarGrey::STEM::GameFont::monospace(FontSize absize) {
    return GameFont::monospace(generic_font_size(absize));
}

std::shared_ptr<GameFont> WarGrey::STEM::GameFont::math(FontSize absize) {
    return GameFont::math(generic_font_size(absize));
}

std::shared_ptr<GameFont> WarGrey::STEM::GameFont::fangsong(FontSize absize) {
    return GameFont::fangsong(generic_font_size(absize));
}

std::shared_ptr<GameFont> WarGrey::STEM::GameFont::Default(FontSize absize) {
    return GameFont::Default(generic_font_size(absize));
}

std::shared_ptr<GameFont> WarGrey::STEM::GameFont::sans_serif(int ftsize) {
    return game_create_font(generic_font_family_name(FontFamily::sans_serif), ftsize);
}

std::shared_ptr<GameFont> WarGrey::STEM::GameFont::serif(int ftsize) {
    return game_create_font(generic_font_family_name(FontFamily::serif), ftsize);
}

std::shared_ptr<GameFont> WarGrey::STEM::GameFont::cursive(int ftsize) {
    return game_create_font(generic_font_family_name(FontFamily::cursive), ftsize);
}

std::shared_ptr<GameFont> WarGrey::STEM::GameFont::fantasy(int ftsize) {
    return game_create_font(generic_font_family_name(FontFamily::fantasy), ftsize);
}

std::shared_ptr<GameFont> WarGrey::STEM::GameFont::monospace(int ftsize) {
    return game_create_font(generic_font_family_name(FontFamily::monospace), ftsize);
}

std::shared_ptr<GameFont> WarGrey::STEM::GameFont::math(int ftsize) {
    return game_create_font(generic_font_family_name(FontFamily::math), ftsize);
}

std::shared_ptr<GameFont> WarGrey::STEM::GameFont::fangsong(int ftsize) {
    return game_create_font(generic_font_family_name(FontFamily::fangsong), ftsize);
}

std::shared_ptr<GameFont> WarGrey::STEM::GameFont::Default(int ftsize) {
    return GameFont::serif(ftsize);
}

std::shared_ptr<GameFont> WarGrey::STEM::GameFont::Title() {
    return GameFont::sans_serif(FontSize::xx_large);
}

std::shared_ptr<GameFont> WarGrey::STEM::GameFont::Tooltip() {
    return GameFont::serif(FontSize::x_small);
}

const char* WarGrey::STEM::GameFont::basename() {
    if (this->okay()) {
        std::string family_name(TTF_FontFaceFamilyName(font));
        
        if (basenames.find(family_name) == basenames.end()) {
            return nullptr;
        } else {
            return basenames[family_name].c_str();
        }
    } else {
        return nullptr;
    }
}

void WarGrey::STEM::GameFont::feed_text_extent(const char* unicode, int* width, int* height) {
    if (this->okay()) {
        TTF_SizeUTF8(this->font, unicode, width, height);
    }
}

bool WarGrey::STEM::GameFont::is_suitable(const std::string& text) {
    bool okay = true;
    
    if (this->okay()) {
        size_t utf8_size = string_utf8_length(text.c_str(), text.size());

        for (size_t idx = 0; idx < utf8_size; idx ++) {
            if (!TTF_GlyphIsProvided32(this->font, string_utf8_ref(text, idx))) {
                okay = false;
                break;
            }
        }
    } else {
        okay = false;
    }

    return okay;
}

int WarGrey::STEM::GameFont::width(const char* unicode) {
    int width = 0;

    if (this->okay()) {
        this->feed_text_extent(unicode, &width);
    }

    return width;
}

int WarGrey::STEM::GameFont::height() {
    return this->okay() ? TTF_FontHeight(this->font) : 0;
}

int WarGrey::STEM::GameFont::ascent() {
    return this->okay() ? TTF_FontAscent(this->font) : 0;
}

int WarGrey::STEM::GameFont::descent() {
    return this->okay() ? TTF_FontDescent(this->font) : 0;
}
