#pragma once // 确保只被 include 一次

#include "../planet.hpp"
#include "../../graphics/font.hpp"

namespace Plteen {
    /*********************************************************************************************/
    class __lambda__ Consolet : public Plteen::Planelet {
    public:
        Consolet(int row = 10, int col = 20, const char* name = nullptr);
        Consolet(shared_font_t font, int row = 10, int col = 20, const char* name = nullptr);
        virtual ~Consolet() noexcept {}

    public:
        void sync_cursor();
        void linefeed(bool sync = true);
        void display(const std::string& message, bool sync_cursor = true);
        void clear_screen();
        
    private:
        void try_scroll_screen();
        void clear_term_cell(int r, int c, int count);

    private:
        class GhostConsolePlane;
        Plteen::Consolet::GhostConsolePlane* self = nullptr;
    };
}
