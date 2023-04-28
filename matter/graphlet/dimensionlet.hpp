#pragma once

#if defined(_MSC_VER)
#pragma warning (disable: 4250)
#endif

#include <SDL2/SDL.h>

#include <cstdint>

#include "../graphlet.hpp"
#include "../../graphics/font.hpp"
#include "../../graphics/texture.hpp"
#include "../../graphics/named_colors.hpp"

namespace WarGrey::STEM {
    /*********************************************************************************************/
    enum class DimensionState { Default, Highlight, _ };

    /** By design, negative numerical parameters are considered not set. **/
    
    /** TODO: how to safely release the fonts, meanwhile leave them with memory leaks **/

    struct DimensionStyle {
		shared_font_t label_font = nullptr;
		long label_color = -1;
		long label_border_color = -1;
		long label_background_color = -1;

		shared_font_t number_font = nullptr;
		long number_color = -1;
		long number_border_color = -1;
		long number_background_color = -1;

		shared_font_t unit_font = nullptr;
		long unit_color = -1;
		long unit_border_color = -1;
		long unit_background_color = -1;

		float minimize_label_width = -1.0F;
		float label_xfraction = -1.0F;

		float minimize_number_width = -1.0F;
		float number_xfraction = -1.0F;

		float number_leading_space = -1.0F;
		float unit_leading_space = -1.0F;

		int precision = -1;
	};

    WarGrey::STEM::DimensionStyle make_plain_dimension_style(int lfontsize, int nfontsize, int ufontsize, int precision = -1);
	WarGrey::STEM::DimensionStyle make_plain_dimension_style(int nfontsize, unsigned int min_n, int precision = -1);
	WarGrey::STEM::DimensionStyle make_setting_dimension_style(int nfontsize, unsigned int min_n, int precision = -1, uint32_t color = SILVER);

	WarGrey::STEM::DimensionStyle make_highlight_dimension_style(int nfontsize, unsigned int min_n, int precision = -1,
            uint32_t number_bgcolor = GOLDENROD, uint32_t label_bgcolor = FORESTGREEN, uint32_t color = GHOSTWHITE);
    
	WarGrey::STEM::DimensionStyle make_highlight_dimension_style(int nfontsize, unsigned int min_label, unsigned int min_n, int precision,
            uint32_t number_bgcolor = GOLDENROD, uint32_t label_bgcolor = FORESTGREEN, uint32_t color = GHOSTWHITE);

    /*********************************************************************************************/
    class Dimensionlet
        : public virtual WarGrey::STEM::IGraphlet
        , public virtual WarGrey::STEM::IValuelet<float>
        , public virtual WarGrey::STEM::IStatelet<WarGrey::STEM::DimensionState, WarGrey::STEM::DimensionStyle> {
    public:
        Dimensionlet(const char* unit, const std::string& label = "");
        Dimensionlet(const char* unit, const char* label_fmt, ...);
        Dimensionlet(WarGrey::STEM::DimensionState& state, const char* unit, const std::string& label = "");
        Dimensionlet(WarGrey::STEM::DimensionState& state, const char* unit, const char* label_fmt, ...);
        Dimensionlet(WarGrey::STEM::DimensionStyle& style, const char* unit, const std::string& label = "");
        Dimensionlet(WarGrey::STEM::DimensionStyle& style, const char* unit, const char* label_fmt, ...);
        Dimensionlet(WarGrey::STEM::DimensionState& state, WarGrey::STEM::DimensionStyle& style, const char* unit, const std::string& label = "");
        Dimensionlet(WarGrey::STEM::DimensionState& state, WarGrey::STEM::DimensionStyle& style, const char* unit, const char* label_fmt, ...);
        virtual ~Dimensionlet() noexcept {}

    public:
        void feed_extent(float x, float y, float* w = nullptr, float* h = nullptr) override;
        void draw(SDL_Renderer* ds, float x, float y, float Width, float Height) override;

    protected:
        void prepare_style(WarGrey::STEM::DimensionState status, WarGrey::STEM::DimensionStyle& style) override;
		void apply_style(WarGrey::STEM::DimensionStyle& style, SDL_Renderer* renderer) override;

    protected:
        void on_value_changed(SDL_Renderer* ds, float value) override;

    private:
        void feed_subextent(int n, float* w = nullptr, float* h = nullptr);
        void update_drawing_box(int idx, float minimize_width, shared_font_t font, float leading_space);
        void update_number_texture(SDL_Renderer* ds, float value, WarGrey::STEM::DimensionStyle& style);
        void draw_box(SDL_Renderer* ds, int idx, float xfraction, float x, float y, float Height, long bgcolor, long bcolor);

    private:
        shared_texture_t textures[3] = {};
        SDL_FRect boxes[3] = {}; // `FRect.y` is useless

    private:
        std::string label;
        std::string unit;
    };
}
