#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <cstdint>

#include "../graphlet.hpp"

#include "../../graphics/named_colors.hpp"

namespace WarGrey::STEM {
    class ITextlet : public virtual WarGrey::STEM::IGraphlet {
    public:
        ITextlet();
        virtual ~ITextlet();

        void construct(SDL_Renderer* render) override;

    public:
        void set_text(const char* fmt, ...);
        void set_text(WarGrey::STEM::MatterAnchor anchor, const char* fmt, ...);
        void set_text(const std::string& content, WarGrey::STEM::MatterAnchor anchor = MatterAnchor::LT);
        
    public:
        void set_font(TTF_Font* font, WarGrey::STEM::MatterAnchor anchor = MatterAnchor::LT);
        void set_text_color(uint32_t color_hex = SILVER, float alpha = 1.0F);
        void set_background_color(uint32_t bg_hex, float alpha = 1.0F);
        void set_border_color(uint32_t border_hex, float alpha = 1.0F);

    public:
        void feed_extent(float x, float y, float* w = nullptr, float* h = nullptr) override;
        void draw(SDL_Renderer* ds, float x, float y, float Width, float Height) override;

    protected:
        virtual void on_font_changed() {}

    private:
        void update_texture();

    protected:
        SDL_Color text_color;
        TTF_Font* text_font = nullptr;
        SDL_Texture* texture = nullptr;
        uint32_t bg_color;
        float bg_alpha = 0.0F;
        u_int32_t border_color;
        float border_alpha = 0.0F;

    private:
        std::string raw;
    };

    class Labellet : public virtual WarGrey::STEM::ITextlet {
    public:
        Labellet(const char* fmt, ...);
        Labellet(TTF_Font* font, const char* fmt, ...);
        Labellet(unsigned int color_hex, const char* fmt, ...);
        Labellet(unsigned int color_hex, float alpha, const char* fmt, ...);
        Labellet(TTF_Font* font, unsigned int color_hex, const char* fmt, ...);
        Labellet(TTF_Font* font, unsigned int color_hex, float alpha, const char* fmt, ...);
    };
}
