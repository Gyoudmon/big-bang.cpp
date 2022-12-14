#pragma once

#include "forward.hpp"
#include "matter/movable.hpp"

#include <SDL2/SDL.h>

#include <cstdint>
#include <string>

namespace WarGrey::STEM {
    class IMatterInfo {
        public:
            virtual ~IMatterInfo() {};
            IMatterInfo(IPlane* master) : master(master) {};
        
        public:
            IPlane* master;
    };

    class IMatter {
        public:
            IMatter() {}
            virtual ~IMatter();

            virtual void pre_construct() {}     // pseudo constructor for special derived classes before constructing
            virtual void post_construct() {}    // pseudo constructor for special derived classes after constructing

        public:
            WarGrey::STEM::IPlane* master();

        public:
            virtual void construct() {}
            virtual void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr);
            virtual void feed_margin(float x, float y, float* top = nullptr, float* right = nullptr, float* bottom = nullptr, float* left = nullptr);
            virtual void update(uint32_t count, uint32_t interval, uint32_t uptime) {}
            virtual void draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) = 0;
            virtual bool ready() { return true; }

        public:
            virtual void own_caret(bool is_own) {}
            
        public:
            virtual bool is_colliding_with_mouse(float local_x, float local_y) { return true; }
            virtual bool on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) { return false; }
            virtual bool on_text(const char* text, size_t size, bool entire) { return false; }
            virtual bool on_editing_text(const char* text, int pos, int span) { return false; }
            virtual bool on_hover(float local_x, float local_y) { return false; }
            virtual bool on_tap(float local_x, float local_y) { return false; }
            virtual bool on_goodbye(float local_x, float local_y) { return false; }

        public: // low-level events
            virtual bool on_pointer_pressed(uint8_t button, float local_x, float local_y, uint8_t clicks) { return false; }
            virtual bool on_pointer_move(uint32_t state, float local_x, float local_y, float dx, float dy, bool bye) { return false; }
            virtual bool on_pointer_released(uint8_t button, float local_x, float local_y, uint8_t clicks) { return false; }

        public:
            void resize(float ratio);
            void resize(float width, float height);
            void enable_resizing(bool yes_no, WarGrey::STEM::MatterAnchor anchor = MatterAnchor::CC) { this->can_resize = yes_no; this->resize_anchor = anchor; }
            bool resizable(WarGrey::STEM::MatterAnchor* anchor) { (*anchor) = this->resize_anchor; return this->can_resize; }

        public:
            void enable_events(bool yes_no, bool low_level = false) { this->deal_with_events = yes_no; this->deal_with_low_level_events = low_level; }
            bool events_allowed() { return this->deal_with_events; }
            bool low_level_events_allowed() { return (this->events_allowed() && this->deal_with_low_level_events); }
        
        public:
            bool has_caret();
            void moor(WarGrey::STEM::MatterAnchor anchor);
            void clear_moor(); /* the notify_updated() will clear the moor,
                                  but the notification is not always guaranteed to be done,
                                  use this method to do it manually. */

        public:
            void camouflage(bool yes_no) { this->findable = !yes_no; }
            bool concealled() { return !this->findable; }

        public:
            void notify_updated();
            void feed_location(float* x, float* y, WarGrey::STEM::MatterAnchor a = MatterAnchor::LT);
            void log_message(int fgc, const char* fmt, ...);
            void log_message(int fgc, const std::string& msg);
            void log_message(const char* fmt, ...);

        public:
            WarGrey::STEM::IMovable* as_sprite() { return this->_sprite; }

        public:
            IMatterInfo* info = nullptr;

        public:
            SDL_Surface* snapshot();
            bool save_snapshot(const std::string& pname);
            bool save_snapshot(const char* pname);

        protected:
            virtual void on_resize(float width, float height, float old_width, float old_height) {}

        protected:
            WarGrey::STEM::IMovable* _sprite = nullptr;

        private:
            WarGrey::STEM::MatterAnchor resize_anchor;
            bool findable = true;
            bool can_resize = false;
            bool deal_with_events = false;
            bool deal_with_low_level_events = false;
            // bool wheel_translation = true;
        
        private:
            WarGrey::STEM::MatterAnchor anchor = WarGrey::STEM::MatterAnchor::LT;
            float anchor_x = 0.0F;
            float anchor_y = 0.0F;
    };
}
