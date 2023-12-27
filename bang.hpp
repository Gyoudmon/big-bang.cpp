#pragma once

#include "game.hpp"

namespace GYDM {
    /* Constants */
    static const char* unknown_plane_name = "冒险越来越深入了";

    /*********************************************************************************************/
    class __lambda__ TheBigBang : public GYDM::Plane {
    public:
        TheBigBang(const char* name = unknown_plane_name, uint32_t title_color = BLACK)
            : GYDM::Plane(name), title_color(title_color) {}
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
        GYDM::Linkmon* agent = nullptr;
        GYDM::Labellet* title = nullptr;
        GYDM::Labellet* tooltip = nullptr;

    private:
        uint32_t title_color;

    private:
        std::string _the_name = "青少计算机科学";
    };
}
