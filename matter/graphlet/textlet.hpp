#pragma once

#include <SDL2/SDL.h>

#include <cstdint>

#include "../graphlet.hpp"

#include "../../graphics/font.hpp"
#include "../../graphics/texture.hpp"
#include "../../physics/color/rgba.hpp"
#include "../../physics/color/names.hpp"
#include "../../physics/geometry/aabox.hpp"

namespace GYDM {
    class __lambda__ ITextlet : public virtual GYDM::IGraphlet {
    public:
        ITextlet();
        virtual ~ITextlet() noexcept {}

        void construct(GYDM::dc_t* dc) override;
        const char* name() override { return this->c_str(); }

    public:
        void set_text(const char* fmt, ...);
        void set_text(const GYDM::RGBA& color, const char* fmt, ...);
        void set_text(GYDM::MatterPort port, const char* fmt, ...);
        void set_text(const std::string& content, GYDM::MatterPort port = MatterPort::LT);
        
    public:
        void set_font(shared_font_t font, GYDM::MatterPort port = MatterPort::LT);
        void set_text_color(const GYDM::RGBA& color = SILVER);
        void set_foreground_color(const GYDM::RGBA& color = SILVER) { this->set_text_color(color); }
        void set_text_alpha(double alpha);
        void set_foreground_alpha(double alpha) { this->set_text_alpha(alpha); }
        GYDM::RGBA get_foreground_color() { return this->foreground_color; }
        void set_background_color(const GYDM::RGBA& color);
        GYDM::RGBA get_background_color() { return this->background_color; }
        void set_border_color(const GYDM::RGBA& color);
        GYDM::RGBA get_border_color() { return this->border_color; }
        void set_corner_radius(float radius);

    public:
        GYDM::Box get_bounding_box() override;
        void draw(GYDM::dc_t* dc, float x, float y, float Width, float Height) override;

    public:
        size_t content_size() { return this->raw.size(); }
        const char* c_str() const { return this->raw.c_str(); }

    protected:
        virtual void on_font_changed() {}

    private:
        void update_texture();

    protected:
        shared_font_t text_font = nullptr;
        shared_texture_t texture = nullptr;
        GYDM::RGBA foreground_color;
        GYDM::RGBA background_color;
        GYDM::RGBA border_color;
        float corner_radius = 0.0F;

    private:
        std::string raw;
    };

    class __lambda__ Labellet : public virtual GYDM::ITextlet {
    public:
        Labellet(const char* fmt, ...);
        Labellet(shared_font_t font, const char* fmt, ...);
        Labellet(uint32_t hex, const char* fmt, ...);
        Labellet(const GYDM::RGBA& rgb, const char* fmt, ...);
        Labellet(shared_font_t font, uint32_t hex, const char* fmt, ...);
        Labellet(shared_font_t font, const GYDM::RGBA& rgb, const char* fmt, ...);
    };

    /*********************************************************************************************/
    __lambda__ Labellet* make_label_for_tooltip(shared_font_t font,
        const GYDM::RGBA& background_color = SNOW,
        const GYDM::RGBA& border_color = GOLD,
        const GYDM::RGBA& foreground_color = BLACK);
}
