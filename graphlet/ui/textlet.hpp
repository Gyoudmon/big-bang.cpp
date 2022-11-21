#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <cstdint>

#include "../graphlet.hpp"

#include "../../named_colors.hpp"

namespace WarGrey::STEM {
    class ITextlet : public virtual WarGrey::STEM::IGraphlet {
        public:
            ITextlet();
            virtual ~ITextlet();

        public:
            void set_text(const char* fmt, ...);
            void set_text(std::string& content, WarGrey::STEM::MatterAnchor anchor = MatterAnchor::LT);
        
        public:
            void set_font(TTF_Font* font, WarGrey::STEM::MatterAnchor anchor = MatterAnchor::LT);
            void set_text_color(uint32_t color_hex = SILVER, float alpha = 1.0F);

        public:
            void fill_extent(float x, float y, float* w = nullptr, float* h = nullptr) override;
            void draw(SDL_Renderer* ds, float x, float y, float Width, float Height) override;

        protected:
            virtual void on_font_changed() {}

        private:
            void update_text_surface();

        protected:
            SDL_Color text_color;
            TTF_Font* text_font = nullptr;
            SDL_Surface* text_surface = nullptr;

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

