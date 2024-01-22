#include "dimensionlet.hpp"

#include "../../graphics/font.hpp"

#include "../../datum/string.hpp"
#include "../../datum/box.hpp"
#include "../../datum/slot.hpp"

using namespace GYDM;

/*************************************************************************************************/
static uint32_t default_text_color = GHOSTWHITE;
static uint32_t default_math_color = ROYALBLUE;
static uint32_t default_unit_color = PURPLE;

static uint8_t label_idx = 0U;
static uint8_t datum_idx = 1U;
static uint8_t  unit_idx = 2U;

/*************************************************************************************************/
DimensionStyle GYDM::make_plain_dimension_style(int lfontsize, int nfsize, int ufsize, int precision) {
	DimensionStyle ds;

    ds.label_font = GameFont::Default(lfontsize);
	ds.number_font = GameFont::math(nfsize);
	ds.unit_font = GameFont::monospace(ufsize);
	ds.precision = precision;

	return ds;
}

DimensionStyle GYDM::make_plain_dimension_style(int nfsize, unsigned int min_number, int precision) {
	DimensionStyle ds = make_plain_dimension_style(nfsize, nfsize, fl2fxi(nfsize * 0.90F), precision);

    ds.minimize_number_width = ds.number_font->width('0') * float(min_number);
	ds.number_xfraction = 0.5F;
	ds.number_leading_space = 0.0F;
	ds.unit_leading_space = 0.0F;
	ds.label_color = default_text_color;
	
	return ds;
}

DimensionStyle GYDM::make_setting_dimension_style(int nfsize, unsigned int min_number, int precision, const RGBA& color) {
	DimensionStyle ds = make_plain_dimension_style(nfsize, nfsize, nfsize, precision);
	
    ds.minimize_number_width = ds.number_font->width('0') * float(min_number);
	ds.number_border_color = color;
	ds.number_color = color;
	ds.unit_color = color;
	
	return ds;
}

DimensionStyle GYDM::make_highlight_dimension_style(int nfsize, unsigned int min_number, int precision
        , const RGBA& number_bgcolor, const RGBA& label_bgcolor, const RGBA& color) {
	return make_highlight_dimension_style(nfsize, 0U, min_number, precision, number_bgcolor, label_bgcolor, color);
}

DimensionStyle GYDM::make_highlight_dimension_style(int nfsize, unsigned int min_label, unsigned int min_number, int precision
        , const RGBA& number_bgcolor, const RGBA& label_bgcolor, const RGBA& color) {
	DimensionStyle ds = make_plain_dimension_style(nfsize, fl2fxi(nfsize * 1.2F), nfsize, precision);

    ds.minimize_label_width = ((min_label == 0) ? ds.label_font->height() : (ds.label_font->width('0') * float(min_label)));
	ds.label_xfraction = 0.5F;
	ds.minimize_number_width = ds.number_font->width('0') * float(min_number);
	ds.number_leading_space = 2.0F;
	ds.precision = precision;
	ds.number_background_color = number_bgcolor;
	ds.number_color = color;
	ds.label_background_color = label_bgcolor;
	ds.label_color = color;
    
	return ds;
}

/*************************************************************************************************/
GYDM::Dimensionlet::Dimensionlet(const char* unit, const std::string& label) {
    this->unit = std::string(unit);
    this->label = std::string(label);
}

GYDM::Dimensionlet::Dimensionlet(const char* unit, const char* label_fmt, ...) {
    VSNPRINT(label, label_fmt);

    this->unit = std::string(unit);
    this->label = label;
}

GYDM::Dimensionlet::Dimensionlet(DimensionState& state, const char* unit, const std::string& label) : IStatelet(state) {
    this->unit = std::string(unit);
    this->label = label;
}

GYDM::Dimensionlet::Dimensionlet(DimensionState& state, const char* unit, const char* label_fmt, ...) : IStatelet(state) {
    VSNPRINT(label, label_fmt);
    
    this->unit = std::string(unit);
    this->label = label;
}

GYDM::Dimensionlet::Dimensionlet(DimensionStyle& style, const char* unit, const std::string& label) {
    this->unit = std::string(unit);
    this->label = label;
    this->set_style(style);
}

GYDM::Dimensionlet::Dimensionlet(DimensionStyle& style, const char* unit, const char* label_fmt, ...) {
    VSNPRINT(label, label_fmt);
    
    this->unit = std::string(unit);
    this->label = label;
    this->set_style(style);
}

GYDM::Dimensionlet::Dimensionlet(DimensionState& state, DimensionStyle& style, const char* unit, const std::string& label) : IStatelet(state) {
    this->unit = std::string(unit);
    this->label = label;
    this->set_style(style);
}

GYDM::Dimensionlet::Dimensionlet(DimensionState& state, DimensionStyle& style, const char* unit, const char* label_fmt, ...) : IStatelet(state) {
    VSNPRINT(label, label_fmt);
    
    this->unit = std::string(unit);
    this->label = label;
    this->set_style(style);
}

Box GYDM::Dimensionlet::get_bounding_box() {
    size_t n = sizeof(this->textures) / sizeof(shared_texture_t);
    float w, h;

    this->feed_subextent(n, &w, &h);

    return { w, h };
}

void GYDM::Dimensionlet::draw_box(GYDM::dc_t* dc, int idx, float xfraction, float x, float y, float Height, const std::optional<RGBA>& bgcolor, const std::optional<RGBA>& bcolor) {
    SDL_FRect* self = &this->boxes[idx];
    
    if ((self->w > 0.0F) && (self->h > 0.0F)) {
        float bottom = y + Height;
        shared_texture_t texture = this->textures[idx];
        
        self->x += x;
        self->h = Height;
        self->y = bottom - self->h;
        
        if (bgcolor.has_value()) {
            dc->fill_rect(self, bgcolor.value());
        }

        if (bcolor.has_value()) {
            dc->draw_rect(self, bcolor.value());
        }

        if (texture.use_count() > 0) {
            int width, height;

            texture->feed_extent(&width, &height);
            dc->stamp(texture->self(), self->x + (self->w - width) * xfraction, bottom - height);
        }

        self->x -= x;
    }
}

void GYDM::Dimensionlet::draw(GYDM::dc_t* dc, float x, float y, float Width, float Height) {
    DimensionStyle style = this->get_style();

    this->draw_box(dc, label_idx, style.label_xfraction, x, y, Height, style.label_background_color, style.label_border_color);
    this->draw_box(dc, datum_idx, style.number_xfraction, x, y, Height, style.number_background_color, style.number_border_color);
    this->draw_box(dc, unit_idx, 0.0F, x, y, Height, style.unit_background_color, style.unit_border_color);
}

void GYDM::Dimensionlet::prepare_style(DimensionState status, DimensionStyle& style) {
    CAS_SLOT(style.number_font, GameFont::math());
	CAS_SLOT(style.unit_font, GameFont::monospace());
	CAS_SLOT(style.label_font, GameFont::Default());

	OptCAS_SLOT(style.number_color, default_math_color);
	OptCAS_SLOT(style.unit_color, default_unit_color);
	OptCAS_SLOT(style.label_color, default_text_color);

	FLCAS_SLOT(style.minimize_label_width, 0.0F);
	FLCAS_SLOT(style.label_xfraction, 1.0F);
	FLCAS_SLOT(style.number_leading_space, float(style.number_font->width('0')));

	switch (status) {
	case DimensionState::Default: case DimensionState::Highlight: {
		FLCAS_SLOT(style.minimize_number_width, 0.0F);
		FLCAS_SLOT(style.number_xfraction, 0.5F);
		FLCAS_SLOT(style.unit_leading_space, 0.0F);
	}; break;
    default: /* ignore */;
	}

	// NOTE: the others can be `nullptr`
}

void GYDM::Dimensionlet::apply_style(DimensionStyle& style, GYDM::dc_t* dc) {
    if (!this->label.empty()) {
        this->textures[label_idx].reset(
            new Texture(dc->create_blended_text(this->label, style.label_font, style.label_color.value(), 0)));
    }

    this->update_number_texture(dc, this->get_value(), style);

    if (!this->unit.empty()) {
        this->textures[unit_idx].reset(
            new Texture(dc->create_blended_text(this->unit, style.unit_font, style.unit_color.value(), 0)));
    }
	
    this->update_drawing_box(label_idx, style.minimize_label_width, style.label_font, 0.0F);
    this->update_drawing_box(datum_idx, style.minimize_number_width, style.number_font, style.number_leading_space);
    this->update_drawing_box( unit_idx, -1.0F, style.unit_font, style.unit_leading_space);
}

void GYDM::Dimensionlet::on_value_changed(dc_t* dc, double value) {
	this->update_number_texture(dc, value, this->get_style());
}

void GYDM::Dimensionlet::update_number_texture(GYDM::dc_t* dc, double value, DimensionStyle& style) {
    this->textures[datum_idx].reset(
        new Texture(dc->create_blended_text(flstring(value, style.precision), style.number_font, style.number_color.value(), 0)));
}

void GYDM::Dimensionlet::update_drawing_box(size_t idx, float min_width, shared_font_t font, float leading_space) {
    shared_texture_t self = this->textures[idx];
    SDL_FRect* sbox = &this->boxes[idx];
    int width, height;

    this->feed_subextent(idx, &sbox->x);
    sbox->x += leading_space;

    if (self.use_count() > 0) {
        self->feed_extent(&width, &height);
        sbox->w = flmax(float(width), min_width);
        sbox->h = float(height);
    } else if (min_width > 0.0F) {
        sbox->w = flmax(min_width, 0.0F);
        sbox->h = float(font->height());
    } else {
        sbox->w = 0.0F;
        sbox->h = 0.0F;
    }
}

void GYDM::Dimensionlet::feed_subextent(size_t n, float* w, float* h) {
    float flw = 0.0F;
    float flh = 0.0F;

    for (size_t idx = 0; idx < n; idx ++) {
        flw = this->boxes[idx].x + this->boxes[idx].w;
        flh = flmax(this->boxes[idx].h, flh);
    }

    SET_BOX(w, flw);
    SET_BOX(h, flh);
}
