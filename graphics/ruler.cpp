#include "ruler.hpp"

#include "../datum/box.hpp"
#include "../datum/string.hpp"
#include "../datum/flonum.hpp"
#include "../datum/fixnum.hpp"

using namespace GYDM;

/*************************************************************************************************/
static const float hatch_long_ratio = 0.618F;
static const float mark_space_ratio = 0.500F;

inline static shared_font_t hatchmark_default_font() {
	static shared_font_t default_mark_font = GameFont::monospace(12);

	return default_mark_font;
}

inline static std::string make_mark_string(double mark, uint8_t precision) {
	return flstring(mark, int(precision));
}

inline static std::string make_lmark_string(double mark, uint32_t precision, uint32_t span, float* span_off) {
	std::string s = make_mark_string(mark, precision);
	(*span_off) = float(span - s.size());

	return s;
}

inline static size_t mark_span(const std::string& mark) {
	// TODO: resolve the mark language
	return mark.size();
}

static inline void fill_consistent_hatch_metrics(shared_font_t font, float thickness, float* hatch_height, float* gapsize) {
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

static uint32_t resolve_step(double vmin, double vmax, float height, float em, uint8_t precision) {
	double range = (vmax - vmin) * flexpt(10.0, double(precision + 2));
	double available_height = double(height - em);
	uint32_t max_fxstep = fl2fx<uint32_t>(flfloor(available_height / (double(em) * 1.618)));
	uint32_t fxstep = 2;

	for (uint32_t step = max_fxstep; step > 2; step--) {
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

static float resolve_interval(uint32_t* step, double vmin, double vmax, float length, float margin, uint32_t* skip, double* diff) {
	uint32_t fxstep = (*step);
	double delta = (vmax - vmin) / double(fxstep);
	float interval = (length - margin)  / float(fxstep);

	(*step) = fxstep;
	SET_BOX(skip, (fxstep % 2 == 0) ? 2 : 1);
	SET_BOX(diff, delta);

	return interval;
}

static void draw_hthatch(GYDM::dc_t* dc, float x0, float y0
		, HHatchMarkMetrics* metrics, float interval, uint32_t step, uint32_t color, bool no_short) {
	float x = x0 + metrics->hatch_x;
	float y = y0 + metrics->height - 1.0F /* thickness */;
	float height = metrics->hatch_height;
	float short_length = height * (((step % 2 == 1) || no_short) ? 0.0F : 0.382F);
	
	metrics->hatch_width = interval * step;
	dc->draw_hline(x, y, metrics->hatch_width, color);
	
	for (unsigned int i = 0; i <= step; i++) {
		dc->draw_vline(interval * float(i) + x, y,
			- ((i % 2 == 0) ? height : short_length), color);
	}
}

static void draw_hbhatch(GYDM::dc_t* dc, float x0, float y0
		, HHatchMarkMetrics* metrics, float interval, uint32_t step, uint32_t color, bool no_short) {
	float x = x0 + metrics->hatch_x;
	float y = y0 + metrics->hatch_y;
	float height = metrics->hatch_height;
	float short_length = height * (((step % 2 == 1) || no_short) ? 0.0F : 0.382F);
	
	metrics->hatch_width = interval * step;
	dc->draw_hline(x, y, metrics->hatch_width, color);
	
	for (unsigned int i = 0; i <= step; i++) {
		dc->draw_vline(interval * float(i) + x, y,
			((i % 2 == 0) ? height : short_length), color);
	}
}

static void draw_vlhatch(GYDM::dc_t* dc, float x0, float y0
		, VHatchMarkMetrics* metrics, float interval, uint32_t step, uint32_t color, bool no_short) {
	float x = x0 + metrics->hatch_x;
	float y = y0 + metrics->hatch_y;
	float width = metrics->hatch_width;
	float short_length = width * (((step % 2 == 1) || no_short) ? 0.0F : 0.382F);
	
	metrics->hatch_height = interval * step;
	x += width - 1.0F /* thickness */;
	dc->draw_vline(x, y, metrics->hatch_height, color);

	for (uint32_t i = 0; i <= step; i++) {
		dc->draw_hline(x, y + interval * float(i),
			-((i % 2 == 0) ? width : short_length), color);
	}
}

static void draw_vrhatch(GYDM::dc_t* dc, float x0, float y0
		, VHatchMarkMetrics* metrics, float interval, uint32_t step, uint32_t color, bool no_short) {
	float x = x0 + metrics->hatch_x;
	float y = y0 + metrics->hatch_y;
	float width = metrics->hatch_width;
	float short_length = width * (((step % 2 == 1) || no_short) ? 0.0F : 0.382F);
	
	metrics->hatch_height = interval * step;
	dc->draw_vline(x, y, metrics->hatch_height, color);

	for (uint32_t i = 0; i <= step; i++) {
		dc->draw_hline(x, y + interval * float(i),
			(i % 2 == 0) ? width : short_length, color);
	}
}

/*************************************************************************************************/
HHatchMarkMetrics GYDM::Ruler::hhatchmark_metrics(double vmin, double vmax, uint8_t precision) {
	return hhatchmark_metrics(hatchmark_default_font(), vmin, vmax, precision);
}

void GYDM::Ruler::draw_ht_hatchmark(GYDM::dc_t* dc, float x, float y, float width, double vmin, double vmax
		, uint32_t step, uint32_t color, HHatchMarkMetrics* metrics, uint8_t precision, bool no_short) {
	Ruler::draw_ht_hatchmark(hatchmark_default_font(), dc, x, y, width, vmin, vmax, step, color, metrics, precision, no_short);
}

void GYDM::Ruler::draw_hb_hatchmark(GYDM::dc_t* dc, float x, float y, float width, double vmin, double vmax
		, uint32_t step, uint32_t color, HHatchMarkMetrics* metrics, uint8_t precision, bool no_short) {
	Ruler::draw_hb_hatchmark(hatchmark_default_font(), dc, x, y, width, vmin, vmax, step, color, metrics, precision, no_short);
}

HHatchMarkMetrics GYDM::Ruler::hhatchmark_metrics(shared_font_t font, double vmin, double vmax, uint8_t precision) {
	HHatchMarkMetrics metrics;
	std::string min_mark = make_mark_string(vmin, precision);
	std::string max_mark = make_mark_string(vmax, precision);
	size_t min_span = mark_span(min_mark);
	size_t max_span = mark_span(max_mark);
	std::string longer_mark = ((max_span > min_span) ? max_mark : min_mark);
	size_t longer_span = mark_span(longer_mark);
	TextMetrics tm = font->get_text_metrics(longer_mark);

	fill_consistent_hatch_metrics(font, 1.0F, &metrics.hatch_height, &metrics.gap_space);

	metrics.ch = tm.width / float(longer_span);
	metrics.em = tm.height - tm.tspace - tm.bspace;
	metrics.top_space = tm.tspace;

	metrics.hatch_x = metrics.ch * min_span * 0.5F;
	metrics.hatch_rx = metrics.ch * max_span * 0.5F;
	
	metrics.height = metrics.em + metrics.gap_space + metrics.hatch_height;

	return metrics;
}

void GYDM::Ruler::draw_ht_hatchmark(shared_font_t font
		, GYDM::dc_t* dc, float x, float y, float width, double vmin, double vmax, uint32_t step0
		, uint32_t color, HHatchMarkMetrics* maybe_metrics, uint8_t precision, bool no_short) {
	uint32_t skip;
	double diff;
	HHatchMarkMetrics metrics = Ruler::hhatchmark_metrics(font, vmin, vmax, precision);
	uint32_t step = (step0 == 0) ? resolve_step(vmin, vmax, width, metrics.hatch_x, metrics.hatch_rx, precision) : step0;
	float interval = resolve_interval(&step, vmin, vmax, width, metrics.hatch_x + metrics.hatch_rx, &skip, &diff);
	float mark_ty = y - metrics.top_space + 1.0F /* thickness */;
	
	metrics.hatch_y = metrics.height - metrics.hatch_height - metrics.top_space;
	draw_hthatch(dc, x, y, &metrics, interval, step, color, no_short);
	
	for (uint32_t i = 0; i <= step; i += (no_short ? 1 : skip)) {
		std::string mark = make_mark_string(vmin + diff * double(i), precision);
		float tx = x + metrics.hatch_x + interval * float(i) - float(font->width(mark)) * 0.5F;

		dc->draw_blended_text(mark, font, tx, mark_ty, color);
	}

	SET_BOX(maybe_metrics, metrics);
}

void GYDM::Ruler::draw_hb_hatchmark(shared_font_t font
		, GYDM::dc_t* dc, float x, float y, float width, double vmin, double vmax, uint32_t step0
		, uint32_t color, HHatchMarkMetrics* maybe_metrics, uint8_t precision, bool no_short) {
	uint32_t skip;
	double diff;
	HHatchMarkMetrics metrics = Ruler::hhatchmark_metrics(font, vmin, vmax, precision);
	uint32_t step = (step0 == 0) ? resolve_step(vmin, vmax, width, metrics.hatch_x, metrics.hatch_rx, precision) : step0;
	float interval = resolve_interval(&step, vmin, vmax, width, metrics.hatch_x + metrics.hatch_rx, &skip, &diff);
	float mark_ty = y + metrics.height - metrics.em;
	
	metrics.hatch_y = 0.0F;
	draw_hbhatch(dc, x, y, &metrics, interval, step, color, no_short);

	for (uint32_t i = 0; i <= step; i += (no_short ? 1 : skip)) {
		std::string mark = make_mark_string(vmin + diff * double(i), precision);
		float tx = x + metrics.hatch_x + interval * float(i) - float(font->width(mark)) * 0.5F;
		
		dc->draw_blended_text(mark, font, tx, mark_ty, color);
	}

	SET_BOX(maybe_metrics, metrics);
}

/*************************************************************************************************/
VHatchMarkMetrics GYDM::Ruler::vhatchmark_metrics(double vmin, double vmax, uint8_t precision) {
	return vhatchmark_metrics(hatchmark_default_font(), vmin, vmax, precision);
}

void GYDM::Ruler::draw_vl_hatchmark(GYDM::dc_t* dc, float x, float y, float width, double vmin, double vmax
		, uint32_t step, uint32_t color, VHatchMarkMetrics* metrics, uint8_t precision, bool no_short) {
	Ruler::draw_vl_hatchmark(hatchmark_default_font(), dc, x, y, width, vmin, vmax, step, color, metrics, precision, no_short);
}

void GYDM::Ruler::draw_vr_hatchmark(GYDM::dc_t* dc, float x, float y, float width, double vmin, double vmax
		, uint32_t step, uint32_t color, VHatchMarkMetrics* metrics, uint8_t precision, bool no_short) {
	Ruler::draw_vr_hatchmark(hatchmark_default_font(), dc, x, y, width, vmin, vmax, step, color, metrics, precision, no_short);
}

VHatchMarkMetrics GYDM::Ruler::vhatchmark_metrics(shared_font_t font, double vmin, double vmax, uint8_t precision) {
	VHatchMarkMetrics metrics;
	std::string min_mark = make_mark_string(vmin, precision);
	std::string max_mark = make_mark_string(vmax, precision);
	size_t min_span = mark_span(min_mark);
	size_t max_span = mark_span(max_mark);
	std::string longer_mark = ((max_span > min_span) ? max_mark : min_mark);
	size_t longer_span = mark_span(longer_mark);
	TextMetrics tm = font->get_text_metrics(longer_mark);

	fill_consistent_hatch_metrics(font, 1.0F, &metrics.hatch_width, &metrics.gap_space);

	metrics.ch = tm.width / float(longer_span);
	metrics.em = tm.height - tm.tspace - tm.bspace;

	metrics.mark_width = tm.width;
	metrics.span = longer_span;
	metrics.top_space = tm.tspace;

	metrics.hatch_y = metrics.em * 0.5F;
	metrics.width = metrics.mark_width + metrics.gap_space + metrics.hatch_width;

	return metrics;
}

void GYDM::Ruler::draw_vl_hatchmark(shared_font_t font
		, GYDM::dc_t* dc, float x, float y, float height, double vmin, double vmax, uint32_t step0
		, uint32_t color, VHatchMarkMetrics* maybe_metrics, uint8_t precision, bool no_short) {
	uint32_t skip;
	float mark_span_off;
	double diff;
	VHatchMarkMetrics metrics = Ruler::vhatchmark_metrics(font, vmin, vmax, precision);
	uint32_t step = (step0 == 0) ? resolve_step(vmin, vmax, height, metrics.em, precision) : step0;
	float interval = resolve_interval(&step, vmin, vmax, height, metrics.em, &skip, &diff);
	
	metrics.hatch_x = metrics.width - metrics.hatch_width;
	draw_vlhatch(dc, x, y, &metrics, interval, step, color, no_short);
	
	for (uint32_t i = 0; i <= step; i += (no_short ? 1 : skip)) {
		std::string mark = make_lmark_string(vmax - diff * double(i), precision, metrics.span, &mark_span_off);
		float tx = x + mark_span_off * metrics.ch;
		float ty = y + interval * float(i);

		dc->draw_blended_text(mark, font, tx, ty, color);
	}

	SET_BOX(maybe_metrics, metrics);
}

void GYDM::Ruler::draw_vr_hatchmark(shared_font_t font
		, GYDM::dc_t* dc, float x, float y, float height, double vmin, double vmax, uint32_t step0
		, uint32_t color, VHatchMarkMetrics* maybe_metrics, uint8_t precision, bool no_short) {
	unsigned int skip;
	double diff;
	VHatchMarkMetrics metrics = Ruler::vhatchmark_metrics(font, vmin, vmax, precision);
	uint32_t step = (step0 == 0) ? resolve_step(vmin, vmax, height, metrics.em, precision) : step0;
	float interval = resolve_interval(&step, vmin, vmax, height, metrics.em, &skip, &diff);
	float mark_tx = x + metrics.hatch_width + metrics.gap_space;
	
	metrics.hatch_x = 0.0F;
	draw_vrhatch(dc, x, y, &metrics, interval, step, color, no_short);
	
	for (uint32_t i = 0; i <= step; i += (no_short ? 1 : skip)) {
		std::string mark = make_mark_string(vmax - diff * double(i), precision);
		float ty = y + interval * float(i);

		dc->draw_blended_text(mark, font, mark_tx, ty, color);
	}

	SET_BOX(maybe_metrics, metrics);
}
