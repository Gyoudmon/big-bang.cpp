#pragma once

#include <SDL2/SDL.h>
#include <string>

/**************************************************************************************************/
namespace WarGrey::STEM {
    class IDisplay {
    public:
        IDisplay() {}
        virtual ~IDisplay() {}

    public:
        virtual void feed_extent(float* width, float* height) = 0;
        virtual void feed_client_extent(float* width, float* height) { this->feed_extent(width, height); }
        virtual uint32_t frame_rate() = 0;
        virtual void refresh() = 0;

    public:
        virtual void log_message(int fgc, const std::string& message) = 0;
        virtual void start_input_text(const std::string& prompt) = 0;
        virtual SDL_Surface* snapshot() = 0;
        virtual SDL_Renderer* master_renderer() = 0;

    public:
        void begin_update_sequence() { this->update_sequence_depth += 1; }
        bool is_in_update_sequence() { return (this->update_sequence_depth > 0); }
        void end_update_sequence();
        bool should_update() { return this->update_is_needed; }
        void notify_updated();

    public:
        bool save_snapshot(const std::string& path);
        bool save_snapshot(const char* path);

    public:
        void log_message(int fgc, const char* fmt, ...);
        void log_message(const char* fmt, ...);
        void log_message(const std::string& message);
    
    private:
        int update_sequence_depth = 0;
        bool update_is_needed = false;
    };
}
