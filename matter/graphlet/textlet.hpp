#pragma once

#include <SDL2/SDL.h>

#include <cstdint>

#include "../graphlet.hpp"

#include "../../graphics/font.hpp"
#include "../../graphics/texture.hpp"
#include "../../graphics/color.hpp"
#include "../../graphics/named_colors.hpp"

namespace WarGrey::STEM {
    class __lambda__ ITextlet : public virtual WarGrey::STEM::IGraphlet {
    public:
        ITextlet();
        virtual ~ITextlet() noexcept {}

        void construct(SDL_Renderer* render) override;
        const char* name() override { return this->c_str(); }

    public:
        void set_text(const char* fmt, ...);
        void set_text(const WarGrey::STEM::RGBA& color, const char* fmt, ...);
        void set_text(WarGrey::STEM::MatterAnchor anchor, const char* fmt, ...);
        void set_text(const std::string& content, WarGrey::STEM::MatterAnchor anchor = MatterAnchor::LT);
        
    public:
        void set_font(shared_font_t font, WarGrey::STEM::MatterAnchor anchor = MatterAnchor::LT);
        void set_text_color(const WarGrey::STEM::RGBA& color = SILVER);
        WarGrey::STEM::RGBA get_text_color();
        void set_background_color(const WarGrey::STEM::RGBA& color);
        WarGrey::STEM::RGBA get_background_color() { return this->background_color; }
        void set_border_color(const WarGrey::STEM::RGBA& color);
        WarGrey::STEM::RGBA get_border_color() { return this->border_color; }
        void set_corner_radius(float radius);

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
        shared_font_t text_font = nullptr;
        shared_texture_t texture = nullptr;
        WarGrey::STEM::RGBA foreground_color;
        WarGrey::STEM::RGBA background_color;
        WarGrey::STEM::RGBA border_color;
        float corner_radius = 0.0F;

    private:
        std::string raw;
    };

    class __lambda__ Labellet : public virtual WarGrey::STEM::ITextlet {
    public:
        Labellet(const char* fmt, ...);
        Labellet(shared_font_t font, const char* fmt, ...);
        Labellet(uint32_t hex, const char* fmt, ...);
        Labellet(const WarGrey::STEM::RGBA& rgb, const char* fmt, ...);
        Labellet(shared_font_t font, uint32_t hex, const char* fmt, ...);
        Labellet(shared_font_t font, const WarGrey::STEM::RGBA& rgb, const char* fmt, ...);
    };

    /*********************************************************************************************/
    __lambda__ Labellet* make_label_for_tooltip(shared_font_t font,
        const WarGrey::STEM::RGBA& background_color = SNOW,
        const WarGrey::STEM::RGBA& border_color = GOLD,
        const WarGrey::STEM::RGBA& foreground_color = BLACK);
}
