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

namespace GYDM {
    /*********************************************************************************************/
    enum class DimensionState { Default, Highlight, _ };

    /** By design, negative numerical parameters are considered not set. **/
    
    /** TODO: how to safely release the fonts, meanwhile leave them with memory leaks **/

    struct __lambda__ DimensionStyle {
		shared_font_t label_font = nullptr;
		std::optional<GYDM::RGBA> label_color = std::nullopt;
		std::optional<GYDM::RGBA> label_border_color = std::nullopt;
		std::optional<GYDM::RGBA> label_background_color = std::nullopt;

		shared_font_t number_font = nullptr;
		std::optional<GYDM::RGBA> number_color = std::nullopt;
		std::optional<GYDM::RGBA> number_border_color = std::nullopt;
		std::optional<GYDM::RGBA> number_background_color = std::nullopt;

		shared_font_t unit_font = nullptr;
		std::optional<GYDM::RGBA> unit_color = std::nullopt;
		std::optional<GYDM::RGBA> unit_border_color = std::nullopt;
		std::optional<GYDM::RGBA> unit_background_color = std::nullopt;

		float minimize_label_width = -1.0F;
		float label_xfraction = -1.0F;

		float minimize_number_width = -1.0F;
		float number_xfraction = -1.0F;

		float number_leading_space = -1.0F;
		float unit_leading_space = -1.0F;

		int precision = -1;
	};

    __lambda__ GYDM::DimensionStyle make_plain_dimension_style(int lfontsize, int nfontsize, int ufontsize, int precision = -1);
	__lambda__ GYDM::DimensionStyle make_plain_dimension_style(int nfontsize, unsigned int min_n, int precision = -1);
	__lambda__ GYDM::DimensionStyle make_setting_dimension_style(int nfontsize, unsigned int min_n, int precision = -1,
            const GYDM::RGBA& color = SILVER);

	__lambda__ GYDM::DimensionStyle make_highlight_dimension_style(int nfontsize, unsigned int min_n, int precision = -1,
            const GYDM::RGBA& number_bgcolor = GOLDENROD, const GYDM::RGBA& label_bgcolor = FORESTGREEN,
            const GYDM::RGBA& color = GHOSTWHITE);
    
	__lambda__ GYDM::DimensionStyle make_highlight_dimension_style(int nfontsize, unsigned int min_label, unsigned int min_n, int precision,
            const GYDM::RGBA& number_bgcolor = GOLDENROD, const GYDM::RGBA& label_bgcolor = FORESTGREEN,
            const GYDM::RGBA& color = GHOSTWHITE);

    /*********************************************************************************************/
    class __lambda__ Dimensionlet
        : public virtual GYDM::IGraphlet
        , public virtual GYDM::IValuelet<double>
        , public virtual GYDM::IStatelet<GYDM::DimensionState, GYDM::DimensionStyle> {
    public:
        Dimensionlet(const char* unit, const std::string& label = "");
        Dimensionlet(const char* unit, const char* label_fmt, ...);
        Dimensionlet(GYDM::DimensionState& state, const char* unit, const std::string& label = "");
        Dimensionlet(GYDM::DimensionState& state, const char* unit, const char* label_fmt, ...);
        Dimensionlet(GYDM::DimensionStyle& style, const char* unit, const std::string& label = "");
        Dimensionlet(GYDM::DimensionStyle& style, const char* unit, const char* label_fmt, ...);
        Dimensionlet(GYDM::DimensionState& state, GYDM::DimensionStyle& style, const char* unit, const std::string& label = "");
        Dimensionlet(GYDM::DimensionState& state, GYDM::DimensionStyle& style, const char* unit, const char* label_fmt, ...);
        virtual ~Dimensionlet() noexcept {}

    public:
        GYDM::Box get_bounding_box() override;
        void draw(GYDM::dc_t* dc, float x, float y, float Width, float Height) override;

    protected:
        void prepare_style(GYDM::DimensionState status, GYDM::DimensionStyle& style) override;
		void apply_style(GYDM::DimensionStyle& style, GYDM::dc_t* dc) override;

    protected:
        void on_value_changed(GYDM::dc_t* dc, double value) override;

    private:
        void feed_subextent(size_t n, float* w = nullptr, float* h = nullptr);
        void update_drawing_box(size_t idx, float minimize_width, shared_font_t font, float leading_space);
        void update_number_texture(GYDM::dc_t* dc, double value, GYDM::DimensionStyle& style);
        void draw_box(GYDM::dc_t* dc, int idx, float xfraction, float x, float y, float Height,
                const std::optional<GYDM::RGBA>& bgcolor,
                const std::optional<GYDM::RGBA>& bcolor);

    private:
        shared_texture_t textures[3] = {};
        SDL_FRect boxes[3] = {}; // `FRect.y` is useless

    private:
        std::string label;
        std::string unit;
    };
}
