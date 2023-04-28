#pragma once

#include <SDL2/SDL.h>

#include <cstdint>

#include "../graphlet.hpp"

#include "../../graphics/font.hpp"
#include "../../graphics/texture.hpp"
#include "../../graphics/named_colors.hpp"

namespace WarGrey::STEM {
    class ITextlet : public virtual WarGrey::STEM::IGraphlet {
    public:
        ITextlet();
        virtual ~ITextlet() noexcept {}

        void construct(SDL_Renderer* render) override;

    public:
        void set_text(const char* fmt, ...);
        void set_text(uint32_t color, const char* fmt, ...);
        void set_text(WarGrey::STEM::MatterAnchor anchor, const char* fmt, ...);
        void set_text(const std::string& content, WarGrey::STEM::MatterAnchor anchor = MatterAnchor::LT);
        
    public:
        void set_font(shared_font_t font, WarGrey::STEM::MatterAnchor anchor = MatterAnchor::LT);
        void set_text_color(uint32_t color_hex = SILVER, double alpha = 1.0);
        uint32_t get_text_color(double* alpha = nullptr);
        void set_background_color(uint32_t bg_hex, double alpha = 1.0);
        uint32_t get_background_color() { return this->bg_color; }
        double get_background_alpha() { return this->bg_alpha; }
        void set_border_color(uint32_t border_hex, double alpha = 1.0);
        uint32_t get_border_color() { return this->bg_color; }
        double get_border_alpha() { return this->bg_alpha; }

    public:
        void feed_extent(float x, float y, float* w = nullptr, float* h = nullptr) override;
        void draw(SDL_Renderer* ds, float x, float y, float Width, float Height) override;

    public:
        size_t content_size() { return this->raw.size(); }
        const char* c_str() { return this->raw.c_str(); }

    protected:
        virtual void on_font_changed() {}

    private:
        void update_texture();

    protected:
        SDL_Color text_color;
        shared_font_t text_font = nullptr;
        shared_texture_t texture = nullptr;
        uint32_t bg_color;
        double bg_alpha = 0.0;
        uint32_t border_color;
        double border_alpha = 0.0;

    private:
        std::string raw;
    };

    class Labellet : public virtual WarGrey::STEM::ITextlet {
    public:
        Labellet(const char* fmt, ...);
        Labellet(shared_font_t font, const char* fmt, ...);
        Labellet(uint32_t color_hex, const char* fmt, ...);
        Labellet(uint32_t color_hex, double alpha, const char* fmt, ...);
        Labellet(shared_font_t font, uint32_t color_hex, const char* fmt, ...);
        Labellet(shared_font_t font, uint32_t color_hex, double alpha, const char* fmt, ...);
    };

    /*********************************************************************************************/
    Labellet* make_label_for_tooltip(shared_font_t font,
        uint32_t fg_color = BLACK,
        uint32_t bg_color = SNOW,
        uint32_t border_color = GOLD);
}
