#pragma once

#include <SDL2/SDL.h>

#include "font.hpp"

namespace WarGrey::STEM {
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
	
	class __lambda__ Ruler {
	public:
		static WarGrey::STEM::HHatchMarkMetrics hhatchmark_metrics(
					double vmin, double vmax, uint8_t precision = 0U);

		static WarGrey::STEM::HHatchMarkMetrics hhatchmark_metrics(WarGrey::STEM::shared_font_t font,
					double vmin, double vmax, uint8_t precision = 0U);

	public:
		static void draw_ht_hatchmark(SDL_Renderer* renderer, float x, float y,
					float width, double vmin, double vmax, uint32_t step, uint32_t color,
					WarGrey::STEM::HHatchMarkMetrics* metrics = nullptr,
					uint8_t precision = 0U, bool no_short = false);

		static void draw_ht_hatchmark(WarGrey::STEM::shared_font_t font,
					SDL_Renderer* renderer, float x, float y,
					float width, double vmin, double vmax, uint32_t step, uint32_t color,
					WarGrey::STEM::HHatchMarkMetrics* metrics = nullptr,
					uint8_t precision = 0U, bool no_short = false);

		static void draw_hb_hatchmark(SDL_Renderer* renderer, float x, float y,
					float width, double vmin, double vmax, uint32_t step, uint32_t color,
					WarGrey::STEM::HHatchMarkMetrics* metrics = nullptr,
					uint8_t precision = 0U, bool no_short = false);
		
		static void draw_hb_hatchmark(WarGrey::STEM::shared_font_t font,
					SDL_Renderer* renderer, float x, float y,
					float width, double vmin, double vmax, uint32_t step, uint32_t color,
					WarGrey::STEM::HHatchMarkMetrics* metrics = nullptr,
					uint8_t precision = 0U, bool no_short = false);
	};
}
