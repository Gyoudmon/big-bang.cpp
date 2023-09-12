#pragma once

#include "game.hpp"

namespace WarGrey::STEM {
    /* Constants */
    static const char* unknown_plane_name = "冒险越来越深入了";

    /*********************************************************************************************/
    class __lambda__ TheBigBang : public WarGrey::STEM::Plane {
    public:
        TheBigBang(const char* name = unknown_plane_name, uint32_t title_color = BLACK)
            : WarGrey::STEM::Plane(name), title_color(title_color) {}
        virtual ~TheBigBang() {}

    public:
        void load(float width, float height) override;

    public:
        void the_name(const std::string name) { this->_the_name = name; }
        void the_name(const char* name) { this->_the_name = std::string(name); }
        const char* the_name() { return this->_the_name.c_str(); }

    protected:
        float get_titlebar_height();

    protected:
        WarGrey::STEM::Linkmon* agent = nullptr;
        WarGrey::STEM::Labellet* title = nullptr;
        WarGrey::STEM::Labellet* tooltip = nullptr;

    private:
        uint32_t title_color;

    private:
        std::string _the_name = "青少计算机科学";
    };
}
