#include "bang.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
static std::string the_name = "青少计算机科学";

TTF_Font* WarGrey::STEM::bang_font::title = nullptr;
TTF_Font* WarGrey::STEM::bang_font::tooltip = nullptr;
TTF_Font* WarGrey::STEM::bang_font::huge = nullptr;
TTF_Font* WarGrey::STEM::bang_font::large = nullptr;
TTF_Font* WarGrey::STEM::bang_font::normal = nullptr;
TTF_Font* WarGrey::STEM::bang_font::small = nullptr;
TTF_Font* WarGrey::STEM::bang_font::tiny = nullptr;
TTF_Font* WarGrey::STEM::bang_font::mono = nullptr;

/*************************************************************************************************/
void WarGrey::STEM::the_big_bang_name(const char* name) {
    the_name = std::string(name);
}
    
const char* WarGrey::STEM::the_big_bang_name() {
    return the_name.c_str();
}

void WarGrey::STEM::bang_fonts_initialize() {
    if (bang_font::title == nullptr) {
        bang_font::title = game_create_font(font_basename(game_font::title), bang_fontsize::xx_large);
        bang_font::tooltip = game_create_font(font_basename(game_font::unicode), bang_fontsize::x_small);
        bang_font::huge = game_create_font(font_basename(game_font::unicode), bang_fontsize::x_large);
        bang_font::large = game_create_font(font_basename(game_font::unicode), bang_fontsize::large);
        bang_font::normal = game_create_font(font_basename(game_font::unicode), bang_fontsize::medium);
        bang_font::small = game_create_font(font_basename(game_font::unicode), bang_fontsize::small);
        bang_font::tiny = game_create_font(font_basename(game_font::unicode), bang_fontsize::xx_small);
        bang_font::mono = game_create_font(font_basename(game_font::monospace), bang_fontsize::medium);
    }
}

void WarGrey::STEM::bang_fonts_destroy() {
    game_font_destroy(bang_font::title);
    game_font_destroy(bang_font::tooltip);
    game_font_destroy(bang_font::huge);
    game_font_destroy(bang_font::large);
    game_font_destroy(bang_font::normal);
    game_font_destroy(bang_font::small);
    game_font_destroy(bang_font::tiny);
    game_font_destroy(bang_font::mono);
}

/*************************************************************************************************/
void WarGrey::STEM::TheBigBang::load(float width, float height) {
    this->title = this->insert(new Labellet(bang_font::title, this->title_color, "%s: %s", the_name.c_str(), this->name()));

    this->agent = this->insert(new Linkmon());
    this->agent->scale(-1.0F, 1.0F);

    this->set_sentry_sprite(this->agent);
    this->move_to(this->title, this->agent, MatterAnchor::RB, MatterAnchor::LB);
}
