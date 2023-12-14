#pragma once

#if defined(_MSC_VER)
#pragma warning (disable: 4250)
#endif

#include <SDL2/SDL.h>

#include <cstdint>
#include <optional>

#include "../graphlet.hpp"
#include "../../graphics/font.hpp"
#include "../../graphics/color.hpp"
#include "../../graphics/texture.hpp"
#include "../../graphics/named_colors.hpp"

namespace WarGrey::STEM {
    /*********************************************************************************************/
    enum class DimensionState { Default, Highlight, _ };

    /** By design, negative numerical parameters are considered not set. **/
    
    /** TODO: how to safely release the fonts, meanwhile leave them with memory leaks **/

    struct __lambda__ DimensionStyle {
		shared_font_t label_font = nullptr;
		std::optional<WarGrey::STEM::RGBA> label_color = std::nullopt;
		std::optional<WarGrey::STEM::RGBA> label_border_color = std::nullopt;
		std::optional<WarGrey::STEM::RGBA> label_background_color = std::nullopt;

		shared_font_t number_font = nullptr;
		std::optional<WarGrey::STEM::RGBA> number_color = std::nullopt;
		std::optional<WarGrey::STEM::RGBA> number_border_color = std::nullopt;
		std::optional<WarGrey::STEM::RGBA> number_background_color = std::nullopt;

		shared_font_t unit_font = nullptr;
		std::optional<WarGrey::STEM::RGBA> unit_color = std::nullopt;
		std::optional<WarGrey::STEM::RGBA> unit_border_color = std::nullopt;
		std::optional<WarGrey::STEM::RGBA> unit_background_color = std::nullopt;

		float minimize_label_width = -1.0F;
		float label_xfraction = -1.0F;

		float minimize_number_width = -1.0F;
		float number_xfraction = -1.0F;

		float number_leading_space = -1.0F;
		float unit_leading_space = -1.0F;

		int precision = -1;
	};

    __lambda__ WarGrey::STEM::DimensionStyle make_plain_dimension_style(int lfontsize, int nfontsize, int ufontsize, int precision = -1);
	__lambda__ WarGrey::STEM::DimensionStyle make_plain_dimension_style(int nfontsize, unsigned int min_n, int precision = -1);
	__lambda__ WarGrey::STEM::DimensionStyle make_setting_dimension_style(int nfontsize, unsigned int min_n, int precision = -1,
            const WarGrey::STEM::RGBA& color = SILVER);

	__lambda__ WarGrey::STEM::DimensionStyle make_highlight_dimension_style(int nfontsize, unsigned int min_n, int precision = -1,
            const WarGrey::STEM::RGBA& number_bgcolor = GOLDENROD, const WarGrey::STEM::RGBA& label_bgcolor = FORESTGREEN,
            const WarGrey::STEM::RGBA& color = GHOSTWHITE);
    
	__lambda__ WarGrey::STEM::DimensionStyle make_highlight_dimension_style(int nfontsize, unsigned int min_label, unsigned int min_n, int precision,
            const WarGrey::STEM::RGBA& number_bgcolor = GOLDENROD, const WarGrey::STEM::RGBA& label_bgcolor = FORESTGREEN,
            const WarGrey::STEM::RGBA& color = GHOSTWHITE);

    /*********************************************************************************************/
    class __lambda__ Dimensionlet
        : public virtual WarGrey::STEM::IGraphlet
        , public virtual WarGrey::STEM::IValuelet<double>
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
        void on_value_changed(SDL_Renderer* ds, double value) override;

    private:
        void feed_subextent(size_t n, float* w = nullptr, float* h = nullptr);
        void update_drawing_box(size_t idx, float minimize_width, shared_font_t font, float leading_space);
        void update_number_texture(SDL_Renderer* ds, double value, WarGrey::STEM::DimensionStyle& style);
        void draw_box(SDL_Renderer* ds, int idx, float xfraction, float x, float y, float Height,
                const std::optional<WarGrey::STEM::RGBA>& bgcolor,
                const std::optional<WarGrey::STEM::RGBA>& bcolor);

    private:
        shared_texture_t textures[3] = {};
        SDL_FRect boxes[3] = {}; // `FRect.y` is useless

    private:
        std::string label;
        std::string unit;
    };
}
