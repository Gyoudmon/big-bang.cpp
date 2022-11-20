#pragma once

#include "universe.hpp"
#include "planet.hpp"
#include "virtualization/screen.hpp"

namespace WarGrey::STEM {
    class Cosmos : public WarGrey::STEM::IUniverse {
        public:
            Cosmos(int fps = 60, uint32_t fgc = 0x000000U, uint32_t bgc = 0xFFFFFFU);
            virtual ~Cosmos();

        public:
            void reflow(float window, float height) override;
            void update(uint32_t interval, uint32_t count, uint32_t uptime) override {};
            void draw(SDL_Renderer* renderer, int x, int y, int width, int height) override;
            bool can_exit() override;

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
            void on_elapse(uint32_t interval, uint32_t count, uint32_t uptime) override;

        protected:
            void push_planet(WarGrey::STEM::IPlanet* planet);

        private:
            void collapse();

        private:
            WarGrey::STEM::IScreen* screen = nullptr;
            WarGrey::STEM::IPlanet* head_planet = nullptr;
            WarGrey::STEM::IPlanet* recent_planet = nullptr;
    };
}

