#pragma once

#include "game.hpp"

namespace WarGrey::STEM {
    /* Constants */
    static const char* unknown_plane_name = "冒险越来越深入了";
    
    __lambda__ void the_big_bang_name(const char* name);
    __lambda__ const char* the_big_bang_name();

    /*********************************************************************************************/
    class __lambda__ TheBigBang : public WarGrey::STEM::Plane {
        public:
            TheBigBang(const char* name = unknown_plane_name, uint32_t title_color = BLACK)
                : WarGrey::STEM::Plane(name), title_color(title_color) {}
            virtual ~TheBigBang() {}

        public:
            void load(float width, float height) override;

        protected:
            float get_titlebar_height();

        protected:
            WarGrey::STEM::Linkmon* agent = nullptr;
            WarGrey::STEM::Labellet* title = nullptr;
            WarGrey::STEM::Labellet* tooltip = nullptr;

        private:
            uint32_t title_color;
    };
}
