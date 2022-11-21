#include "textlet.hpp"

#include "../../font.hpp"
#include "../../text.hpp"
#include "../../colorspace.hpp"
#include "../../geometry.hpp"

#include "../../datum/string.hpp"
#include "../../datum/box.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::ITextlet::ITextlet() {
    this->set_text_color();
}

WarGrey::STEM::ITextlet::~ITextlet() {
    game_font_destroy(this->text_font);

    if (this->text_surface != nullptr) {
        SDL_FreeSurface(this->text_surface);
    }
}

void WarGrey::STEM::ITextlet::set_text_color(uint32_t color_hex, float alpha) {
    RGB_FillColor(&this->text_color, color_hex, alpha);
    this->update_text_surface();
    this->notify_updated();
}

void WarGrey::STEM::ITextlet::set_font(TTF_Font* font, MatterAnchor anchor) {
    this->moor(anchor);

    this->text_font = ((font == nullptr) ? game_font::DEFAULT : font);
    this->set_text(this->raw, anchor);
    this->on_font_changed();

    this->notify_updated();
}

void WarGrey::STEM::ITextlet::set_text(std::string& content, MatterAnchor anchor) {
    this->raw = content;

    this->moor(anchor);

    if (this->text_font == nullptr) {
        this->set_font(nullptr, anchor);
    } else {
        this->update_text_surface();
    }

    this->notify_updated();
}

void WarGrey::STEM::ITextlet::set_text(const char* fmt, ...) {
    VSNPRINT(content, fmt);
    this->set_text(content);
}

void WarGrey::STEM::ITextlet::fill_extent(float x, float y, float* w, float* h) {
    if (this->text_surface != nullptr) {
        SET_BOX(w, float(this->text_surface->w));
        SET_BOX(h, float(this->text_surface->h));
    } else {
        IGraphlet::fill_extent(x, y, w, h);
    }
}

void WarGrey::STEM::ITextlet::draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) {
    if (this->text_surface != nullptr) {
        game_render_surface(renderer, this->text_surface, x, y);
    }
}

void WarGrey::STEM::ITextlet::update_text_surface() {
    if (this->text_surface != nullptr) {
        SDL_FreeSurface(this->text_surface);
    }

    if (this->raw.empty()) {
        this->text_surface = nullptr;
    } else {
        this->text_surface = game_text_surface(this->raw, this->text_font,
                TextRenderMode::Blender, this->text_color, this->text_color, 0);
    }
}

/*************************************************************************************************/
WarGrey::STEM::Labellet::Labellet(const char *fmt, ...) {
    VSNPRINT(caption, fmt);
    this->set_text(caption);
}

WarGrey::STEM::Labellet::Labellet(TTF_Font* font, const char* fmt, ...) {
    VSNPRINT(caption, fmt);
    this->set_font(font);
    this->set_text(caption);
}

WarGrey::STEM::Labellet::Labellet(TTF_Font* font, unsigned int color_hex, const char* fmt, ...) {
    VSNPRINT(caption, fmt);

    this->set_font(font);
    this->set_text_color(color_hex, 1.0F);
    this->set_text(caption);
}

WarGrey::STEM::Labellet::Labellet(TTF_Font* font, unsigned int color_hex, float alpha, const char* fmt, ...) {
    VSNPRINT(caption, fmt);

    this->set_font(font);
    this->set_text_color(color_hex, alpha);
    this->set_text(caption);
}

WarGrey::STEM::Labellet::Labellet(unsigned int color_hex, const char* fmt, ...) {
    VSNPRINT(caption, fmt);

    this->set_text_color(color_hex, 1.0F);
    this->set_text(caption);
}

WarGrey::STEM::Labellet::Labellet(unsigned int color_hex, float alpha, const char* fmt, ...) {
    VSNPRINT(caption, fmt);

    this->set_text_color(color_hex, alpha);
    this->set_text(caption);
}

