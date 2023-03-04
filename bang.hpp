#pragma once

#include "game.hpp"
#include "physics/random.hpp"

namespace WarGrey::STEM {
    /* Constants */
    static const char* unknown_plane_name = "冒险越来越深入了";
    
    struct bang_fontsize { // follow the CSS convention
        static const int medium = 21;
        static const int xx_large = bang_fontsize::medium * 2;
        static const int x_large  = bang_fontsize::medium * 3 / 2;
        static const int large    = bang_fontsize::medium * 6 / 5;
        static const int small    = bang_fontsize::medium * 8 / 9;
        static const int x_small  = bang_fontsize::medium * 3 / 4;
        static const int xx_small = bang_fontsize::medium * 3 / 5;
    };

    struct bang_font {
        static TTF_Font* title;
        static TTF_Font* tooltip;
        static TTF_Font* huge;
        static TTF_Font* large;
        static TTF_Font* normal;
        static TTF_Font* small;
        static TTF_Font* tiny;
        static TTF_Font* mono;
    };

    void the_big_bang_name(const char* name);
    const char* the_big_bang_name();

    void bang_fonts_initialize();
    void bang_fonts_destroy();

    /*********************************************************************************************/
    class TheBigBang : public WarGrey::STEM::Plane {
        public:
            TheBigBang(const char* name = unknown_plane_name, uint32_t title_color = BLACK)
                : WarGrey::STEM::Plane(name), title_color(title_color) {}
            virtual ~TheBigBang() {}

        public:
            void load(float width, float height) override;

        protected:
            WarGrey::STEM::AgentSpriteSheet* agent;
            WarGrey::STEM::Labellet* title;

        private:
            uint32_t title_color;
    };
}
