#pragma once

#include "universe.hpp"
#include "plane.hpp"
#include "navigator.hpp"
#include "virtualization/screen.hpp"

namespace WarGrey::STEM {
    class Cosmos : public WarGrey::STEM::IUniverse, public WarGrey::STEM::INavigatorListener {
    public:
        Cosmos(WarGrey::STEM::INavigator* navigator, int fps = 60, uint32_t fgc = 0x000000U, uint32_t bgc = 0xFFFFFFU);
        Cosmos(int fps = 60, uint32_t fgc = 0x000000U, uint32_t bgc = 0xFFFFFFU) : Cosmos(nullptr, fps, fgc, bgc) {}
        virtual ~Cosmos();

    public:
        void reflow(float window, float height) override;
        void update(uint32_t count, uint32_t interval, uint32_t uptime) override {};
        void draw(SDL_Renderer* renderer, int x, int y, int width, int height) override;
        bool can_exit() override;

    public:
        void transfer(int delta_idx);
        void transfer_to_plane(const char* name) { this->transfer_to_plane(std::string(name)); }
        void transfer_to_plane(const std::string& name);
        void transfer_to_plane(int idx);
        void transfer_to_next_plane() { this->transfer(1); }
        void transfer_to_prev_plane() { this->transfer(-1); }
        void on_navigate(int from_index, int to_index) override;

    protected: // 常规事件处理和分派函数
        void on_mouse_event(SDL_MouseButtonEvent& mouse, bool pressed) override; 
        void on_mouse_move(uint32_t state, int x, int y, int dx, int dy) override;
        void on_scroll(int horizon, int vertical, float hprecise, float vprecise) override;

        void on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) override;
        void on_text(const char* text, size_t size, bool entire) override;
        void on_editing_text(const char* text, int pos, int span) override;
        
        void on_save() override;

    protected:
        void on_big_bang(int width, int height) override;
        void on_elapse(uint32_t count, uint32_t interval, uint32_t uptime) override;

    protected:
        void push_plane(WarGrey::STEM::IPlane* plane);

    private:
        void collapse();
        void notify_transfer(WarGrey::STEM::IPlane* from, WarGrey::STEM::IPlane* to);

    private:
        WarGrey::STEM::IScreen* screen = nullptr;
        WarGrey::STEM::IPlane* head_plane = nullptr;
        WarGrey::STEM::IPlane* recent_plane = nullptr;

    private:
        WarGrey::STEM::INavigator* navigator = nullptr;
        WarGrey::STEM::IPlane* from_plane = nullptr;
    };
}
