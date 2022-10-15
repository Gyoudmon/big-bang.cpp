#ifndef _WARGREY_STEM_SPRITE_H
#define _WARGREY_STEM_SPRITE_H

#include "forward.hpp"

#include <SDL2/SDL.h>

#include <string>

namespace WarGrey::STEM {
    class ISprite {
        public:
            ISprite() {}
            virtual ~ISprite() {}

            virtual void sprite() {}           // pseudo constructor for special derived classes before constructing
            virtual void sprite_construct() {} // pseudo constructor for special derived classes after constructing

        public:
            virtual void construct() {}
            virtual void fill_extent(float x, float y, float* width = nullptr, float* height = nullptr) = 0;
            virtual void fill_margin(float x, float y, float* top = nullptr, float* right = nullptr, float* bottom = nullptr, float* left = nullptr);
            virtual void resize(float width, float height) {}
            virtual void update(long long count, long long interval, long long uptime) {}
            virtual void draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) = 0;
            virtual bool ready() { return true; }

        public:
            virtual bool on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) { return false; }
            virtual bool on_text(const char* text, bool entire) { return false; }
            virtual bool on_text(const char* text, int pos, int span) { return false; }
            virtual void on_hover(float local_x, float local_y) {}
            virtual void on_tap(float local_x, float local_y) {}
            virtual void on_goodbye(float local_x, float local_y) {}

        public:
            void enable_resizing(bool yes_no, WarGrey::STEM::GraphletAnchor anchor = GraphletAnchor::CC) { this->can_resize = yes_no; this->resize_anchor = anchor; }
            bool resizable(WarGrey::STEM::GraphletAnchor* anchor) { (*anchor) = this->resize_anchor; return this->can_resize; }

        public:
            void enable_events(bool yes_no, bool low_level = false) { this->deal_with_events = yes_no; this->deal_with_low_level_events = low_level; }
            bool handle_events() { return this->deal_with_events; }
            bool handle_low_level_events() { return (this->handle_events() && this->deal_with_low_level_events); }

        public:
            void camouflage(bool yes_no) { this->findable = !yes_no; }
            bool concealled() { return !this->findable; }

        public:
            SDL_Surface* snapshot();
            bool save_snapshot(const std::string& pname);
            bool save_snapshot(const char* pname);

        private:
            WarGrey::STEM::GraphletAnchor resize_anchor;
            bool can_resize = false;
            bool deal_with_events = false;
            bool deal_with_low_level_events = false;
            bool wheel_translation = true;
            bool findable = true;
    };
}

#endif

