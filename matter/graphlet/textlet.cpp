#include "textlet.hpp"

#include "../../graphics/font.hpp"
#include "../../graphics/text.hpp"
#include "../../graphics/colorspace.hpp"
#include "../../graphics/geometry.hpp"

#include "../../datum/string.hpp"
#include "../../datum/box.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
Labellet* WarGrey::STEM::make_label_for_tooltip(shared_font_t font, uint32_t fg_color, uint32_t bg_color, uint32_t border_color) {
    Labellet* tooltip = new Labellet(font, fg_color, "");

    tooltip->set_background_color(bg_color);
    tooltip->set_border_color(border_color);

    return tooltip;
}

/*************************************************************************************************/
WarGrey::STEM::ITextlet::ITextlet() {
    this->set_text_color();
}

void WarGrey::STEM::ITextlet::construct(SDL_Renderer* renderer) {
    this->update_texture();
}

uint32_t WarGrey::STEM::ITextlet::get_text_color(double* alpha) {
    return Hexadecimal_From_Color(&this->text_color, alpha);
}

void WarGrey::STEM::ITextlet::set_text_color(uint32_t color_hex, double alpha) {
    double self_alpha = 0.0;
    uint32_t hex = Hexadecimal_From_Color(&this->text_color, &self_alpha);

    if ((hex != color_hex) || (self_alpha != alpha)) {
        RGB_FillColor(&this->text_color, color_hex, alpha);
        this->update_texture();
        this->notify_updated();
    }
}

void WarGrey::STEM::ITextlet::set_background_color(uint32_t bg_hex, double alpha) {
    if ((this->bg_color != bg_hex) || (this->bg_alpha != alpha)) {
        this->bg_color = bg_hex;
        this->bg_alpha = alpha;
        this->notify_updated();
    }
}

void WarGrey::STEM::ITextlet::set_border_color(uint32_t border_hex, double alpha) {
    if ((this->border_color != border_hex) || (this->border_alpha != alpha)) {
        this->border_color = border_hex;
        this->border_alpha = alpha;
        this->notify_updated();
    }
}

void WarGrey::STEM::ITextlet::set_font(shared_font_t font, MatterAnchor anchor) {
    this->moor(anchor);

    this->text_font = (((font.use_count() > 0) && (font->okay())) ? font : GameFont::Default());
    this->set_text(this->raw, anchor);
    this->on_font_changed();

    this->notify_updated();
}

void WarGrey::STEM::ITextlet::set_text(const std::string& content, MatterAnchor anchor) {
    this->raw = content;

    this->moor(anchor);

    if ((this->text_font.use_count() == 0) || (!this->text_font->okay())) {
        this->set_font(nullptr, anchor);
    } else {
        this->update_texture();
    }

    this->notify_updated();
}

void WarGrey::STEM::ITextlet::set_text(const char* fmt, ...) {
    VSNPRINT(content, fmt);
    this->set_text(content);
}

void WarGrey::STEM::ITextlet::set_text(uint32_t color, const char* fmt, ...) {
    VSNPRINT(content, fmt);
    this->set_text(content);
    this->set_text_color(color);
}

void WarGrey::STEM::ITextlet::set_text(MatterAnchor anchor, const char* fmt, ...) {
    VSNPRINT(content, fmt);
    this->set_text(content, anchor);
}

void WarGrey::STEM::ITextlet::feed_extent(float x, float y, float* w, float* h) {
    if ((this->texture.use_count() > 0) && (this->texture->okay())) {
        this->texture->feed_extent(w, h);
    } else {
        IGraphlet::feed_extent(x, y, w, h);
    }
}

void WarGrey::STEM::ITextlet::draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) {
    if ((this->texture.use_count() > 0) && this->texture->okay()) {
        if (this->bg_alpha > 0.0F) {
            game_fill_rect(renderer, x, y, Width, Height, this->bg_color, this->bg_alpha);
        }

        if (this->border_alpha > 0.0F) {
            game_draw_rect(renderer, x + 0.5F, y + 0.5F, Width - 1.0F, Height - 1.0F, this->border_color, this->border_alpha);
        }

        game_render_texture(renderer, this->texture->self(), x, y);
    }
}

void WarGrey::STEM::ITextlet::update_texture() {
    SDL_Renderer* renderer = this->master_renderer();

    if ((this->raw.empty()) || (renderer == nullptr)) {
        this->texture.reset();
    } else {
        this->texture.reset(new Texture(game_text_texture(renderer, this->raw, this->text_font,
            TextRenderMode::Blender, this->text_color, this->text_color, 0)));
    }
}

/*************************************************************************************************/
WarGrey::STEM::Labellet::Labellet(const char *fmt, ...) {
    VSNPRINT(caption, fmt);
    this->set_text(caption);
}

WarGrey::STEM::Labellet::Labellet(shared_font_t font, const char* fmt, ...) {
    VSNPRINT(caption, fmt);
    this->set_font(font);
    this->set_text(caption);
}

WarGrey::STEM::Labellet::Labellet(shared_font_t font, uint32_t color_hex, const char* fmt, ...) {
    VSNPRINT(caption, fmt);

    this->set_font(font);
    this->set_text_color(color_hex, 1.0F);
    this->set_text(caption);
}

WarGrey::STEM::Labellet::Labellet(shared_font_t font, uint32_t color_hex, double alpha, const char* fmt, ...) {
    VSNPRINT(caption, fmt);

    this->set_font(font);
    this->set_text_color(color_hex, alpha);
    this->set_text(caption);
}

WarGrey::STEM::Labellet::Labellet(uint32_t color_hex, const char* fmt, ...) {
    VSNPRINT(caption, fmt);

    this->set_text_color(color_hex, 1.0F);
    this->set_text(caption);
}

WarGrey::STEM::Labellet::Labellet(uint32_t color_hex, double alpha, const char* fmt, ...) {
    VSNPRINT(caption, fmt);

    this->set_text_color(color_hex, alpha);
    this->set_text(caption);
}
