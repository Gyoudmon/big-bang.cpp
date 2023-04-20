#pragma once

#include "forward.hpp"
#include "physics/motion.hpp"

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

    class IMatterMetadata {};

    class IMatter : public WarGrey::STEM::IMovable {
    public:
        IMatter() {}
        virtual ~IMatter();

    public:
        WarGrey::STEM::IPlane* master();
        SDL_Renderer* master_renderer();

    public:
        void attach_metadata(IMatterMetadata* metadata);
        IMatterMetadata* metadata() const { return this->_metatdata; }

        template<typename MD>
        MD* unsafe_metadata() const { return static_cast<MD*>(this->metadata()); }

    public:
        virtual void construct(SDL_Renderer* renderer) {}
        virtual void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr);
        virtual void feed_original_extent(float x, float y, float* width = nullptr, float* height = nullptr) { this->feed_extent(x, y, width, height); }
        virtual void feed_margin(float x, float y, float* top = nullptr, float* right = nullptr, float* bottom = nullptr, float* left = nullptr) { this->feed_original_margin(x, y, top, right, bottom, left); }
        virtual void feed_original_margin(float x, float y, float* top = nullptr, float* right = nullptr, float* bottom = nullptr, float* left = nullptr);
        virtual int update(uint64_t count, uint32_t interval, uint64_t uptime) { return 0; }
        virtual void draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) = 0;
        virtual void draw_in_progress(SDL_Renderer* renderer, float x, float y, float Width, float Height) {}
        virtual bool ready() { return true; }

    public:
        virtual void own_caret(bool is_own) {}
        virtual int preferred_local_fps() { return 0; }
        virtual const char* name();
        
    public:
        virtual bool is_colliding_with_mouse(float local_x, float local_y) { return true; }
        virtual void on_location_changed(float x, float y, float old_x, float old_y) {}
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
        bool resizable() { return this->can_resize; }
        void scale(float ratio, WarGrey::STEM::MatterAnchor anchor = MatterAnchor::CC) { this->scale(ratio, ratio, anchor); }
        void scale(float x_ratio, float y_ratio, WarGrey::STEM::MatterAnchor anchor = MatterAnchor::CC);
        void scale_to(float ratio, WarGrey::STEM::MatterAnchor anchor = MatterAnchor::CC) { this->scale_to(ratio, ratio, anchor); }
        void scale_to(float x_ratio, float y_ratio, WarGrey::STEM::MatterAnchor anchor = MatterAnchor::CC);
        void resize(float size, WarGrey::STEM::MatterAnchor anchor = MatterAnchor::CC) { this->resize(size, size, anchor); }
        void resize(float width, float height, WarGrey::STEM::MatterAnchor anchor = MatterAnchor::CC);
        void resize_by_width(float size, WarGrey::STEM::MatterAnchor anchor = MatterAnchor::CC) { this->scale_by_size(size, true, anchor); }
        void resize_by_height(float size, WarGrey::STEM::MatterAnchor anchor = MatterAnchor::CC) { this->scale_by_size(size, false, anchor); }
        
    public:
        bool events_allowed() { return this->deal_with_events; }
        bool low_level_events_allowed() { return (this->events_allowed() && this->deal_with_low_level_events); }
    
    public:
        bool has_caret();
        void moor(WarGrey::STEM::MatterAnchor anchor);
        void clear_moor(); /* the notify_updated() will clear the moor,
                              but the notification is not always guaranteed to be done,
                              use this method to do it manually. */

    public:
        void camouflage(bool yes = true) { this->findable = !yes; }
        bool concealled() { return !this->findable; }
        void show(bool yes = true);
        bool visible() { return !this->invisible; }

    public:
        void notify_updated();
        void notify_timeline_restart(uint32_t count0 = 0, int duration = 0);
        void feed_location(float* x, float* y, WarGrey::STEM::MatterAnchor a = MatterAnchor::LT);
        void log_message(int fgc, const char* fmt, ...);
        void log_message(int fgc, const std::string& msg);
        void log_message(const char* fmt, ...);

    public:
        IMatterInfo* info = nullptr;

    private:
        void scale_by_size(float size, bool given_width, WarGrey::STEM::MatterAnchor anchor);

    protected:
        void enable_events(bool yes = true, bool low_level = false) { this->deal_with_events = yes; this->deal_with_low_level_events = low_level; }
        void enable_resize(bool yes = true) { this->can_resize = yes; }
        virtual void on_resize(float width, float height, float old_width, float old_height) {}

    private:
        bool findable = true;
        bool invisible = false;
        bool can_resize = false;
        bool deal_with_events = false;
        bool deal_with_low_level_events = false;
        // bool wheel_translation = true;
    
    private:
        WarGrey::STEM::MatterAnchor anchor = WarGrey::STEM::MatterAnchor::LT;
        float anchor_x = 0.0F;
        float anchor_y = 0.0F;

    private:
        WarGrey::STEM::IMatterMetadata* _metatdata = nullptr;
    };
}
