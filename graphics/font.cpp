#include "font.hpp"

#include <unordered_map>
#include <filesystem>

#include "../datum/flonum.hpp"
#include "../datum/string.hpp"
#include "../datum/hash.hpp"
#include "../datum/box.hpp"

using namespace GYDM;
using namespace std::filesystem;

/*************************************************************************************************/
typedef std::tuple<std::string, int> font_key_t;

namespace {
    struct FontKeyHash { // It might not be neccessary
    public:
        std::size_t operator() (const font_key_t& fk) const {
            size_t hash = 0;
            
            hash_combine(hash, std::get<0>(fk));
            hash_combine(hash, std::get<1>(fk));

            return hash;
        }
    };
}

static std::unordered_map<font_key_t, shared_font_t, FontKeyHash> fontdb;
static int medium_fontsize = 16;

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
void GYDM::game_fonts_initialize() {
    for (unsigned int idx = 0; idx < sizeof(system_fontdirs) / sizeof(std::string); idx++) {
        path root(system_fontdirs[idx]);

        if (exists(root) && is_directory(root)) {
            game_push_fonts_of_directory(root);
        }
    }
}

void GYDM::game_fonts_destroy() {
    /**
     * Please remeber to clear the fonts
     *   Or it will fail at exit due to segfault
     * TODO: find out why.
     */
    fontdb.clear();
}

int GYDM::generic_font_size(FontSize size) {
    // It's okay to work with integer-division
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

const char* GYDM::generic_font_family_name_for_ascii(FontFamily family) {
    switch (family) {
#if defined(__macosx__)
    case FontFamily::sans_serif: return "LucidaGrande.ttc"; break;
    case FontFamily::serif: return "Times New Roman.ttf"; break;
    case FontFamily::monospace: return "Courier New Bold.ttf"; break;
    case FontFamily::math: return "STIXTwoText-Italic.ttf"; break;
    case FontFamily::cursive: return "Apple Chancery.ttf"; break;
    case FontFamily::fantasy: return "Comic Sans MS.ttf"; break;

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

const char* GYDM::generic_font_family_name_for_chinese(FontFamily family) {
    switch (family) {
#if defined(__macosx__)
    case FontFamily::sans_serif: return "Hiragino Sans GB.ttc"; break;
    case FontFamily::serif: return "Songti.ttc"; break;
    case FontFamily::monospace: return "STHeiti Medium.ttc"; break;
    case FontFamily::fangsong: return "Arial Unicode.ttf"; break;
#elif defined(__windows__) /* HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\Fonts */
    // Some fonts might fail due to referenced by BIG-CAPTION names.
    // Say, The 'SIMFANG.TTF', for instance.
    case FontFamily::sans_serif: return "msyh.ttc"; break; // 雅黑
    case FontFamily::serif: return "simkai.ttf"; break; // 楷体
    case FontFamily::monospace: return "simhei.ttf"; break;
    case FontFamily::cursive: return "FZSTK.TTF"; break;
    case FontFamily::fantasy: return "STHUPO.TTF"; break;
    case FontFamily::fangsong: return "simfang.ttf"; break;
#else /* the following fonts have not been tested */
#endif
    default: return generic_font_family_name_for_ascii(family);
    }
}

/*************************************************************************************************/
shared_font_t GYDM::game_create_shared_font(const char* face, int fontsize) {
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
            fontdb[font_key] = nullptr;
        } else {
            basenames[std::string(TTF_FontFaceFamilyName(font))] = std::string(face);
            fontdb[font_key] = std::make_shared<GameFont>(font, fontsize);
        }
    }

    return fontdb[font_key];
}

shared_font_t GYDM::game_create_shared_font(const char* face, float fontsize) {
    return game_create_shared_font(face, fl2fxi(fontsize));
}

TTF_Font* GYDM::game_create_font(const char* face, int fontsize) {
    std::string face_key(face);
    TTF_Font* font = nullptr;
    
    if (system_fonts.find(face_key) == system_fonts.end()) {
        font = TTF_OpenFont(face, fontsize);
    } else {
        font = TTF_OpenFont(system_fonts[face_key].c_str(), fontsize);
    }

    if (font == nullptr) {
        fprintf(stderr, "无法加载字体 '%s': %s\n", face, TTF_GetError());
    } else {
        basenames[std::string(TTF_FontFaceFamilyName(font))] = std::string(face);
    }

    return font;
}

TTF_Font* GYDM::game_create_font(const char* face, float fontsize) {
    return game_create_font(face, fl2fxi(fontsize));
}

void GYDM::game_destory_font(TTF_Font* font) {
    if (font != nullptr) {
        TTF_CloseFont(font);
    }
}

const std::string* GYDM::game_fontname_list(int* n, int fontsize) {
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
void GYDM::GameFont::fontsize(int ftsize) {
    if (ftsize > 0) {
        medium_fontsize = ftsize;
    }
}

std::shared_ptr<GameFont> GYDM::GameFont::sans_serif(FontSize absize) {
    return GameFont::sans_serif(generic_font_size(absize));
}

std::shared_ptr<GameFont> GYDM::GameFont::serif(FontSize absize) {
    return GameFont::serif(generic_font_size(absize));
}

std::shared_ptr<GameFont> GYDM::GameFont::cursive(FontSize absize) {
    return GameFont::cursive(generic_font_size(absize));
}

std::shared_ptr<GameFont> GYDM::GameFont::fantasy(FontSize absize) {
    return GameFont::fantasy(generic_font_size(absize));
}

std::shared_ptr<GameFont> GYDM::GameFont::monospace(FontSize absize) {
    return GameFont::monospace(generic_font_size(absize));
}

std::shared_ptr<GameFont> GYDM::GameFont::math(FontSize absize) {
    return GameFont::math(generic_font_size(absize));
}

std::shared_ptr<GameFont> GYDM::GameFont::fangsong(FontSize absize) {
    return GameFont::fangsong(generic_font_size(absize));
}

std::shared_ptr<GameFont> GYDM::GameFont::Default(FontSize absize) {
    return GameFont::Default(generic_font_size(absize));
}

std::shared_ptr<GameFont> GYDM::GameFont::sans_serif(int ftsize) {
    return GameFont::create_font(FontFamily::sans_serif, ftsize);
}

std::shared_ptr<GameFont> GYDM::GameFont::serif(int ftsize) {
    return GameFont::create_font(FontFamily::serif, ftsize);
}

std::shared_ptr<GameFont> GYDM::GameFont::cursive(int ftsize) {
    return GameFont::create_font(FontFamily::cursive, ftsize);
}

std::shared_ptr<GameFont> GYDM::GameFont::fantasy(int ftsize) {
    return GameFont::create_font(FontFamily::fantasy, ftsize);
}

std::shared_ptr<GameFont> GYDM::GameFont::monospace(int ftsize) {
    return GameFont::create_font(FontFamily::monospace, ftsize);
}

std::shared_ptr<GameFont> GYDM::GameFont::math(int ftsize) {
    return GameFont::create_font(FontFamily::math, ftsize);
}

std::shared_ptr<GameFont> GYDM::GameFont::fangsong(int ftsize) {
    return GameFont::create_font(FontFamily::fangsong, ftsize);
}

std::shared_ptr<GameFont> GYDM::GameFont::Default(int ftsize) {
    return GameFont::serif(ftsize);
}

std::shared_ptr<GameFont> GYDM::GameFont::Title() {
    return GameFont::sans_serif(FontSize::xx_large);
}

std::shared_ptr<GameFont> GYDM::GameFont::Tooltip(FontSize absize) {
    return GameFont::serif(absize);
}

std::shared_ptr<GameFont> GYDM::GameFont::create_font(FontFamily family, int ftsize) {
    std::shared_ptr<GameFont> font = game_create_shared_font(generic_font_family_name_for_ascii(family), ftsize);

    font->family = family;

    return font;
}


std::shared_ptr<GameFont> GYDM::GameFont::try_fallback_for_unicode() {
    if (this->family != FontFamily::_) {
        return game_create_shared_font(generic_font_family_name_for_chinese(this->family), this->size);
    } else {
        return game_create_shared_font(generic_font_family_name_for_chinese(FontFamily::fangsong), this->size);
    }
}

const char* GYDM::GameFont::basename() {
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

void GYDM::GameFont::feed_text_extent(const char* unicode, int* width, int* height) {
    if (this->okay()) {
        TTF_SizeUTF8(this->font, unicode, width, height);
    }
}

TextMetrics GYDM::GameFont::get_text_metrics(const char* unicode) {
    TextMetrics metrics;

    if (this->okay()) {
        size_t char_size = strlen(unicode);
        size_t utf8_size = string_utf8_length(unicode);
        int width, height, xmin, xmax, advance;
        int yminf, ymaxf, yminl, ymaxl;

        TTF_SizeUTF8(this->font, unicode, &width, &height);
        metrics.width = float(width);
        metrics.height = float(height);
        metrics.ascent = float(TTF_FontAscent(this->font));
        metrics.descent = -float(TTF_FontDescent(this->font));

        if (char_size > 0) {
            if (char_size == utf8_size) {
                TTF_GlyphMetrics(this->font, unicode[0], &xmin, nullptr, &yminf, &ymaxf, nullptr);
                TTF_GlyphMetrics(this->font, unicode[char_size - 1], nullptr, &xmax, &yminl, &ymaxl, &advance);
                metrics.lspace = float(xmin);
                metrics.rspace = float(advance - xmax);
                metrics.tspace = metrics.ascent - flmax(float(ymaxf), float(ymaxl));
                metrics.tspace = metrics.descent + flmin(float(yminf), float(yminl));
            }
            // TODO: deal with Unicode text
        } else {
            metrics.lspace = metrics.rspace = 0.0F;
            metrics.tspace = metrics.ascent;
            metrics.bspace = metrics.descent;
        }
    }

    return metrics;
}

bool GYDM::GameFont::is_suitable(const std::string& text) {
    bool okay = true;
    
    if (this->okay()) {
        size_t utf8_size = string_utf8_length(text.c_str(), int(text.size()));

        for (size_t idx = 0; idx < utf8_size; idx ++) {
#ifndef __windows__
            if (!TTF_GlyphIsProvided32(this->font, string_utf8_ref(text, idx))) {
                okay = false;
                break;
            }
#else
            uint32_t utf8_ch = string_utf8_ref(text, int(idx));

            // TODO: Upgrade the windows version of TTF_Font
            if (utf8_ch > 0xFFU) {
                okay = false;
                break;
            } else if (!TTF_GlyphIsProvided(this->font, utf8_ch)) {
                okay = false;
                break;
            }
#endif
        }
    } else {
        okay = false;
    }

    return okay;
}

int GYDM::GameFont::width(const char* unicode) {
    int width = 0;

    if (this->okay()) {
        this->feed_text_extent(unicode, &width);
    }

    return width;
}

int GYDM::GameFont::width(uint16_t ch, int* offset) {
    int xmin, advance = 0;

    if (this->okay()) {
        TTF_GlyphMetrics(this->font, ch, &xmin, nullptr, nullptr, nullptr, &advance);
        SET_BOX(offset, xmin);
    }

    return advance;
}

int GYDM::GameFont::height() {
    return this->okay() ? TTF_FontHeight(this->font) : 0;
}

int GYDM::GameFont::ascent() {
    return this->okay() ? TTF_FontAscent(this->font) : 0;
}

int GYDM::GameFont::descent() {
    return this->okay() ? -TTF_FontDescent(this->font) : 0;
}
