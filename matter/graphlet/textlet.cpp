#include "textlet.hpp"

#include "../../graphics/font.hpp"
#include "../../graphics/text.hpp"
#include "../../graphics/brush.hpp"

#include "../../datum/string.hpp"
#include "../../datum/box.hpp"

#include <SDL2/SDL2_gfxPrimitives.h>

using namespace GYDM;

/*************************************************************************************************/
static Labellet* make_styled_label(shared_font_t font, const RGBA& bg_color, const RGBA& bd_color, const RGBA& fg_color, float cr) {
    Labellet* tooltip = new Labellet(font, fg_color, "");

    tooltip->set_background_color(bg_color);
    tooltip->set_border_color(bd_color);
    
    if (cr != 0.0F) {
        tooltip->set_corner_radius(cr);
    }

    return tooltip;
}

/*************************************************************************************************/
Labellet* GYDM::make_label_for_tooltip(shared_font_t font, const RGBA& bg_color, const RGBA& bd_color, const RGBA& fg_color) {
    return make_styled_label(font, bg_color, bd_color, fg_color, 0.0F);
}

/*************************************************************************************************/
GYDM::ITextlet::ITextlet() {
    this->set_text_color();
}

void GYDM::ITextlet::construct(SDL_Renderer* renderer) {
    this->update_texture();
}

void GYDM::ITextlet::set_text_color(const RGBA& fgc) {
    if (this->foreground_color != fgc) {
        this->foreground_color = fgc;
        this->update_texture();
        this->notify_updated();
    }
}

void GYDM::ITextlet::set_text_alpha(double alpha) {
    if (this->foreground_color.alpha() != alpha) {
        this->foreground_color = RGBA(this->foreground_color, alpha);
        this->update_texture();
        this->notify_updated();
    }
}

void GYDM::ITextlet::set_background_color(const RGBA& bgc) {
    if (this->background_color != bgc) {
        this->background_color = bgc;
        this->notify_updated();
    }
}

void GYDM::ITextlet::set_border_color(const RGBA& bdc) {
    if (this->border_color != bdc) {
        this->border_color = bdc;
        this->notify_updated();
    }
}

void GYDM::ITextlet::set_corner_radius(float radius) {
    if (this->corner_radius != radius) {
        this->corner_radius = radius;
        this->notify_updated();
    }
}

void GYDM::ITextlet::set_font(shared_font_t font, MatterAnchor anchor) {
    this->moor(anchor);

    this->text_font = (((font.use_count() > 0) && (font->okay())) ? font : GameFont::Default());
    this->set_text(this->raw, anchor);
    this->on_font_changed();

    this->notify_updated();
}

void GYDM::ITextlet::set_text(const std::string& content, MatterAnchor anchor) {
    this->raw = content;

    this->moor(anchor);

    if ((this->text_font.use_count() == 0) || (!this->text_font->okay())) {
        this->set_font(nullptr, anchor);
    } else {
        this->update_texture();
    }

    this->notify_updated();
}

void GYDM::ITextlet::set_text(const char* fmt, ...) {
    VSNPRINT(content, fmt);
    this->set_text(content);
}

void GYDM::ITextlet::set_text(const RGBA& color, const char* fmt, ...) {
    VSNPRINT(content, fmt);
    this->set_text(content);
    this->set_text_color(color);
}

void GYDM::ITextlet::set_text(MatterAnchor anchor, const char* fmt, ...) {
    VSNPRINT(content, fmt);
    this->set_text(content, anchor);
}

Box GYDM::ITextlet::get_bounding_box() {
    Box box;

    if ((this->texture.use_count() > 0) && (this->texture->okay())) {
        float w, h;

        this->texture->feed_extent(&w, &h);
        box = { w, h };
    } else {
        box = IGraphlet::get_bounding_box();
    }

    return box;
}

void GYDM::ITextlet::draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) {
    if ((this->texture.use_count() > 0) && this->texture->okay()) {
        if (this->corner_radius == 0.0F) {
            float pos_off = 0.0F;
            float sizeoff = 0.5F;

            if (this->background_color.is_opacity()) {
                Brush::fill_rect(renderer, x + pos_off, y + pos_off,
                                        Width - sizeoff, Height - sizeoff,
                                        this->background_color);
            }

            if (this->border_color.is_opacity()) {
                Brush::draw_rect(renderer, x + pos_off, y + pos_off,
                                        Width - sizeoff, Height - sizeoff,
                                        this->border_color);
            }
        } else {
            float pos_off = 0.5F;
            float sizeoff = 2.0F;

            if (this->background_color.is_opacity()) {
                Brush::fill_rounded_rect(renderer, x + pos_off, y + pos_off,
                                        Width - sizeoff, Height - sizeoff,
                                        this->corner_radius, this->background_color);
            }

            if (this->border_color.is_opacity()) {
                Brush::draw_rounded_rect(renderer, x + pos_off, y + pos_off,
                                        Width - sizeoff, Height - sizeoff,
                                        this->corner_radius, this->border_color);
            }
        }

        if (this->foreground_color.is_opacity()) {
            Brush::stamp(renderer, this->texture->self(), x, y);
        }
    }
}

void GYDM::ITextlet::update_texture() {
    SDL_Renderer* renderer = this->master_renderer();

    if ((this->raw.empty()) || (renderer == nullptr)) {
        this->texture.reset();
    } else {
        this->texture.reset(new Texture(game_text_texture(renderer, this->raw, this->text_font,
            TextRenderMode::Blender, this->foreground_color, this->foreground_color, 0)));
    }
}

/*************************************************************************************************/
GYDM::Labellet::Labellet(const char *fmt, ...) {
    VSNPRINT(caption, fmt);
    this->set_text(caption);
}

GYDM::Labellet::Labellet(shared_font_t font, const char* fmt, ...) {
    VSNPRINT(caption, fmt);
    this->set_font(font);
    this->set_text(caption);
}

GYDM::Labellet::Labellet(shared_font_t font, uint32_t hex, const char* fmt, ...) {
    VSNPRINT(caption, fmt);

    this->set_font(font);
    this->set_text_color(hex);
    this->set_text(caption);
}

GYDM::Labellet::Labellet(shared_font_t font, const RGBA& rgb, const char* fmt, ...) {
    VSNPRINT(caption, fmt);

    this->set_font(font);
    this->set_text_color(rgb);
    this->set_text(caption);
}

GYDM::Labellet::Labellet(uint32_t hex, const char* fmt, ...) {
    VSNPRINT(caption, fmt);

    this->set_text_color(hex);
    this->set_text(caption);
}

GYDM::Labellet::Labellet(const RGBA& rgb, const char* fmt, ...) {
    VSNPRINT(caption, fmt);

    this->set_text_color(rgb);
    this->set_text(caption);
}
