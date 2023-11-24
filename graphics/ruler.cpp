#include "ruler.hpp"

#include "../datum/box.hpp"
#include "../datum/string.hpp"
#include "../datum/flonum.hpp"
#include "../datum/fixnum.hpp"

#include "text.hpp"
#include "pen.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
static const float hatch_long_ratio = 0.618F;
static const float mark_space_ratio = 0.500F;

inline static shared_font_t hatchmark_default_font() {
	static shared_font_t default_mark_font = GameFont::monospace(12.0F);

	return default_mark_font;
}

inline static std::string make_mark_string(double mark, uint8_t precision) {
	return flstring(mark, int(precision));
}

inline static size_t mark_span(const std::string& mark) {
	// TODO: resolve the mark language
	return mark.size();
}

static std::string resolve_longest_mark(std::string marks[], size_t count) {
	std::string longest_mark = marks[0];
	size_t longest_span = mark_span(marks[0]);

	for (size_t idx = 1; idx < count; idx++) {
		unsigned int this_span = mark_span(marks[idx]);

		if (this_span > longest_span) {
			longest_span = this_span;
			longest_mark = marks[idx];
		}
	}

	return longest_mark;
}

static inline void fill_consistent_hhatch_metrics(shared_font_t font, float thickness, float* hatch_height, float* gapsize) {
	float chwidth = float(font->width('0'));
	
	SET_BOX(hatch_height, chwidth * hatch_long_ratio + thickness);
	SET_BOX(gapsize, chwidth * mark_space_ratio + thickness);
}

static uint32_t resolve_step(double vmin, double vmax, float width, float lspace, float rspace, uint8_t precision) {
	double range = (vmax - vmin) * flexpt(10.0, double(precision + 2));
	uint32_t max_fxstep = fl2fx<uint32_t>(flfloor(double(width) / double(lspace + rspace)));
	uint32_t fxstep = 2;

	for (uint32_t step = max_fxstep; step > 2; step --) {
		double interval = range / double(step);
		uint32_t fxinterval = fl2fx<uint32_t>(flfloor(interval));

		if (interval == double(fxinterval)) {
			if (fxinterval % 10 == 0) {
				if ((step < 10) || (step % 2 == 0)) {
					fxstep = step;
					break;
				}
			}
		}
	}
	
	return fxstep;
}

static float resolve_interval(uint32_t* step, double vmin, double vmax, float width, float lspace, float rspace
		, uint8_t precision, uint32_t* skip, double* diff) {
	uint32_t fxstep = (((*step) == 0U) ? resolve_step(vmin, vmax, width, lspace, rspace, precision) : (*step));
	double delta = (vmax - vmin) / double(fxstep);
	float interval = (width - lspace - rspace)  / float(fxstep);

	(*step) = fxstep;
	SET_BOX(skip, (fxstep % 2 == 0) ? 2 : 1);
	SET_BOX(diff, delta);

	return interval;
}

static void draw_hthatch(SDL_Renderer* renderer, float x0, float y0
		, HHatchMarkMetrics* metrics, float interval, uint32_t step, uint32_t color, bool no_short) {
	float x = x0 + metrics->hatch_x;
	float y = y0 + metrics->height - 1.0F /* thickness */;
	float height = metrics->hatch_height;
	float short_length = height * (((step % 2 == 1) || no_short) ? 0.0F : 0.382F);
	
	metrics->hatch_width = interval * step;
	Pen::draw_hline(renderer, x, y, metrics->hatch_width, color);
	
	for (unsigned int i = 0; i <= step; i++) {
		float xthis = interval * float(i) + x;

		Pen::draw_vline(renderer, xthis, y, - ((i % 2 == 0) ? height : short_length), color);
	}
}

static void draw_hbhatch(SDL_Renderer* renderer, float x0, float y0
		, HHatchMarkMetrics* metrics, float interval, unsigned int step, uint32_t color, bool no_short) {
	float x = x0 + metrics->hatch_x;
	float y = y0 + metrics->hatch_y;
	float height = metrics->hatch_height;
	float short_length = height * (((step % 2 == 1) || no_short) ? 0.0F : 0.382F);
	
	metrics->hatch_width = interval * step;
	Pen::draw_hline(renderer, x, y, metrics->hatch_width, color);
	
	for (unsigned int i = 0; i <= step; i++) {
		float xthis = interval * float(i) + x;

		Pen::draw_vline(renderer, xthis, y, ((i % 2 == 0) ? height : short_length), color);
	}
}

/*************************************************************************************************/
HHatchMarkMetrics WarGrey::STEM::Ruler::hhatchmark_metrics(double vmin, double vmax, uint8_t precision) {
	return hhatchmark_metrics(hatchmark_default_font(), vmin, vmax, precision);
}

void WarGrey::STEM::Ruler::draw_ht_hatchmark(SDL_Renderer* renderer, float x, float y, float width, double vmin, double vmax
		, uint32_t step, uint32_t color, HHatchMarkMetrics* metrics, uint8_t precision, bool no_short) {
	Ruler::draw_ht_hatchmark(hatchmark_default_font(), renderer, x, y, width, vmin, vmax, step, color, metrics, precision, no_short);
}

void WarGrey::STEM::Ruler::draw_hb_hatchmark(SDL_Renderer* renderer, float x, float y, float width, double vmin, double vmax
		, uint32_t step, uint32_t color, HHatchMarkMetrics* metrics, uint8_t precision, bool no_short) {
	Ruler::draw_hb_hatchmark(hatchmark_default_font(), renderer, x, y, width, vmin, vmax, step, color, metrics, precision, no_short);
}

HHatchMarkMetrics WarGrey::STEM::Ruler::hhatchmark_metrics(shared_font_t font, double vmin, double vmax, uint8_t precision) {
	HHatchMarkMetrics metrics;
	std::string min_mark = make_mark_string(vmin, precision);
	std::string max_mark = make_mark_string(vmax, precision);
	size_t min_span = mark_span(min_mark);
	size_t max_span = mark_span(max_mark);
	std::string longer_mark = ((max_span > min_span) ? max_mark : min_mark);
	size_t longer_span = mark_span(longer_mark);
	TextMetrics tm = font->get_text_metrics(longer_mark);

	fill_consistent_hhatch_metrics(font, 1.0F, &metrics.hatch_height, &metrics.gap_space);

	metrics.ch = tm.width / float(longer_span);
	metrics.em = tm.height - tm.tspace - tm.bspace;
	metrics.top_space = tm.tspace;

	metrics.hatch_x = metrics.ch * min_span * 0.5F;
	metrics.hatch_rx = metrics.ch * max_span * 0.5F;
	
	metrics.height = metrics.em + metrics.gap_space + metrics.hatch_height;

	return metrics;
}

void WarGrey::STEM::Ruler::draw_ht_hatchmark(shared_font_t font
		, SDL_Renderer* renderer, float x, float y, float width, double vmin, double vmax, uint32_t step
		, uint32_t color, HHatchMarkMetrics* maybe_metrics, uint8_t precision, bool no_short) {
	uint32_t skip;
	double diff;
	HHatchMarkMetrics metrics = Ruler::hhatchmark_metrics(font, vmin, vmax, precision);
	float interval = resolve_interval(&step, vmin, vmax, width, metrics.hatch_x, metrics.hatch_rx, precision, &skip, &diff);
	float mark_ty = y - metrics.top_space + 1.0F /* thickness */;
	
	metrics.hatch_y = metrics.height - metrics.hatch_height - metrics.top_space;
	draw_hthatch(renderer, x, y, &metrics, interval, step, color, no_short);
	
	for (uint32_t i = 0; i <= step; i += (no_short ? 1 : skip)) {
		std::string mark = make_mark_string(vmin + diff * double(i), precision);
		auto pmark = game_blended_text_texture(renderer, mark, font, color);
		float tx = x + metrics.hatch_x + interval * float(i) - float(font->width(mark)) * 0.5F;

		Pen::stamp(renderer, pmark, tx, mark_ty);
	}

	SET_BOX(maybe_metrics, metrics);
}

void WarGrey::STEM::Ruler::draw_hb_hatchmark(shared_font_t font
		, SDL_Renderer* renderer, float x, float y, float width, double vmin, double vmax, uint32_t step
		, uint32_t color, HHatchMarkMetrics* maybe_metrics, uint8_t precision, bool no_short) {
	unsigned int skip;
	double diff;
	HHatchMarkMetrics metrics = Ruler::hhatchmark_metrics(font, vmin, vmax, precision);
	float interval = resolve_interval(&step, vmin, vmax, width, metrics.hatch_x, metrics.hatch_rx, precision, &skip, &diff);
	float mark_ty = y + metrics.height - metrics.em;
	
	metrics.hatch_y = 0.0F;
	draw_hbhatch(renderer, x, y, &metrics, interval, step, color, no_short);

	for (unsigned int i = 0; i <= step; i += (no_short ? 1 : skip)) {
		std::string mark = make_mark_string(vmin + diff * double(i), precision);
		auto pmark = game_blended_text_texture(renderer, mark, font, color);
		float tx = x + metrics.hatch_x + interval * float(i) - float(font->width(mark)) * 0.5F;
		
		Pen::stamp(renderer, pmark, tx, mark_ty);
	}

	SET_BOX(maybe_metrics, metrics);
}
