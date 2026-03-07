#pragma once

#include <SDL2/SDL.h>

#include <cstdint>
#include <optional>

#include "../graphlet.hpp"
#include "../../graphics/font.hpp"
#include "../../graphics/texture.hpp"
#include "../../physics/color/rgba.hpp"
#include "../../physics/color/names.hpp"
#include "../../physics/geometry/aabox.hpp"

namespace Plteen {
    /*********************************************************************************************/
    enum class DimensionState { Default, Highlight, _ };

    /** By design, negative numerical parameters are considered not set. **/
    
    /** TODO: how to safely release the fonts, meanwhile leave them with memory leaks **/

    struct __lambda__ DimensionStyle {
		shared_font_t label_font = nullptr;
		std::optional<Plteen::RGBA> label_color = std::nullopt;
		std::optional<Plteen::RGBA> label_border_color = std::nullopt;
		std::optional<Plteen::RGBA> label_background_color = std::nullopt;

		shared_font_t number_font = nullptr;
		std::optional<Plteen::RGBA> number_color = std::nullopt;
		std::optional<Plteen::RGBA> number_border_color = std::nullopt;
		std::optional<Plteen::RGBA> number_background_color = std::nullopt;

		shared_font_t unit_font = nullptr;
		std::optional<Plteen::RGBA> unit_color = std::nullopt;
		std::optional<Plteen::RGBA> unit_border_color = std::nullopt;
		std::optional<Plteen::RGBA> unit_background_color = std::nullopt;

		float minimize_label_width = -1.0F;
		float label_xfraction = -1.0F;

		float minimize_number_width = -1.0F;
		float number_xfraction = -1.0F;

		float number_leading_space = -1.0F;
		float unit_leading_space = -1.0F;

		int precision = -1;
	};

    __lambda__ Plteen::DimensionStyle make_plain_dimension_style(int lfontsize, int nfontsize, int ufontsize, int precision = -1);
	__lambda__ Plteen::DimensionStyle make_plain_dimension_style(int nfontsize, unsigned int min_n, int precision = -1);
	__lambda__ Plteen::DimensionStyle make_setting_dimension_style(int nfontsize, unsigned int min_n, int precision = -1,
            const Plteen::RGBA& color = SILVER);

	__lambda__ Plteen::DimensionStyle make_highlight_dimension_style(int nfontsize, unsigned int min_n, int precision = -1,
            const Plteen::RGBA& number_bgcolor = GOLDENROD, const Plteen::RGBA& label_bgcolor = FORESTGREEN,
            const Plteen::RGBA& color = GHOSTWHITE);
    
	__lambda__ Plteen::DimensionStyle make_highlight_dimension_style(int nfontsize, unsigned int min_label, unsigned int min_n, int precision,
            const Plteen::RGBA& number_bgcolor = GOLDENROD, const Plteen::RGBA& label_bgcolor = FORESTGREEN,
            const Plteen::RGBA& color = GHOSTWHITE);

    /*********************************************************************************************/
    class __lambda__ Dimensionlet
        : public virtual Plteen::IGraphlet
        , public virtual Plteen::IValuelet<double>
        , public virtual Plteen::IStatelet<Plteen::DimensionState, Plteen::DimensionStyle> {
    public:
        Dimensionlet(const char* unit, const std::string& label = "");
        Dimensionlet(const char* unit, const char* label_fmt, ...);
        Dimensionlet(Plteen::DimensionState& state, const char* unit, const std::string& label = "");
        Dimensionlet(Plteen::DimensionState& state, const char* unit, const char* label_fmt, ...);
        Dimensionlet(const Plteen::DimensionStyle& style, const char* unit, const std::string& label = "");
        Dimensionlet(const Plteen::DimensionStyle& style, const char* unit, const char* label_fmt, ...);
        Dimensionlet(Plteen::DimensionState& state, const Plteen::DimensionStyle& style, const char* unit, const std::string& label = "");
        Dimensionlet(Plteen::DimensionState& state, const Plteen::DimensionStyle& style, const char* unit, const char* label_fmt, ...);
        virtual ~Dimensionlet() noexcept {}

    public:
        Plteen::Box get_bounding_box() override;
        void draw(Plteen::dc_t* dc, float x, float y, float Width, float Height) override;

    protected:
        void prepare_style(Plteen::DimensionState status, Plteen::DimensionStyle& style) override;
		void apply_style(Plteen::DimensionStyle& style, Plteen::dc_t* dc) override;

    protected:
        void on_value_changed(Plteen::dc_t* dc, double value) override;

    private:
        void feed_subextent(size_t n, float* w = nullptr, float* h = nullptr);
        void update_drawing_box(size_t idx, float minimize_width, shared_font_t font, float leading_space);
        void update_number_texture(Plteen::dc_t* dc, double value, Plteen::DimensionStyle& style);
        void draw_box(Plteen::dc_t* dc, int idx, float xfraction, float x, float y, float Height,
                const std::optional<Plteen::RGBA>& bgcolor,
                const std::optional<Plteen::RGBA>& bcolor);

    private:
        shared_texture_t textures[3] = {};
        SDL_FRect boxes[3] = {}; // `FRect.y` is useless

    private:
        std::string label;
        std::string unit;
    };
}
