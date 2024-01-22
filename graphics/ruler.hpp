#pragma once

#include "dc.hpp"
#include "font.hpp"

#include <vector>

namespace GYDM {
	struct __lambda__ HHatchMarkMetrics {
		float gap_space;
		float top_space;
		float ch;
		float em;

		float height;
		float hatch_x;
		float hatch_y;
		float hatch_width;
		float hatch_height;
		float hatch_rx;
	};

	struct __lambda__ VHatchMarkMetrics {
		size_t span;
		float mark_width;
		float gap_space;
		float top_space;
		float ch;
		float em;

		float width;
		float hatch_x;
		float hatch_y;
		float hatch_width;
		float hatch_height;
	};
	
	class __lambda__ Ruler {
	public:
		static GYDM::HHatchMarkMetrics hhatchmark_metrics(
					double vmin, double vmax, uint8_t precision = 0U);

		static GYDM::HHatchMarkMetrics hhatchmark_metrics(
					GYDM::shared_font_t font,
					double vmin, double vmax, uint8_t precision = 0U);

		static GYDM::VHatchMarkMetrics vhatchmark_metrics(
					double vmin, double vmax, uint8_t precision = 0U);

		static GYDM::VHatchMarkMetrics vhatchmark_metrics(
					GYDM::shared_font_t font,
					double vmin, double vmax, uint8_t precision = 0U);

	public:
		static void draw_ht_hatchmark(GYDM::dc_t* dc, float x, float y,
					float width, double vmin, double vmax, uint32_t step, uint32_t color,
					GYDM::HHatchMarkMetrics* metrics = nullptr,
					uint8_t precision = 0U, bool no_short = false);

		static void draw_ht_hatchmark(GYDM::shared_font_t font,
					GYDM::dc_t* dc, float x, float y,
					float width, double vmin, double vmax, uint32_t step, uint32_t color,
					GYDM::HHatchMarkMetrics* metrics = nullptr,
					uint8_t precision = 0U, bool no_short = false);

		static void draw_hb_hatchmark(GYDM::dc_t* dc, float x, float y,
					float width, double vmin, double vmax, uint32_t step, uint32_t color,
					GYDM::HHatchMarkMetrics* metrics = nullptr,
					uint8_t precision = 0U, bool no_short = false);
		
		static void draw_hb_hatchmark(GYDM::shared_font_t font,
					GYDM::dc_t* dc, float x, float y,
					float width, double vmin, double vmax, uint32_t step, uint32_t color,
					GYDM::HHatchMarkMetrics* metrics = nullptr,
					uint8_t precision = 0U, bool no_short = false);

		static void draw_vl_hatchmark(GYDM::dc_t* dc, float x, float y,
					float height, double vmin, double vmax, uint32_t step, uint32_t color,
					GYDM::VHatchMarkMetrics* metrics = nullptr,
					uint8_t precision = 0U, bool no_short = false);

		static void draw_vl_hatchmark(GYDM::shared_font_t font,
					GYDM::dc_t* dc, float x, float y,
					float height, double vmin, double vmax, uint32_t step, uint32_t color,
					GYDM::VHatchMarkMetrics* metrics = nullptr,
					uint8_t precision = 0U, bool no_short = false);

		static void draw_vr_hatchmark(GYDM::dc_t* dc, float x, float y,
					float height, double vmin, double vmax, uint32_t step, uint32_t color,
					GYDM::VHatchMarkMetrics* metrics = nullptr,
					uint8_t precision = 0U, bool no_short = false);

		static void draw_vr_hatchmark(GYDM::shared_font_t font,
					GYDM::dc_t* dc, float x, float y,
					float height, double vmin, double vmax, uint32_t step, uint32_t color,
					GYDM::VHatchMarkMetrics* metrics = nullptr,
					uint8_t precision = 0U, bool no_short = false);
	};
}
