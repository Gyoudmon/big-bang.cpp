#pragma once

#include "display.hpp"
#include "../forward.hpp"

namespace GYDM {
    class __lambda__ IScreen {
    public:
        IScreen() {}
        virtual ~IScreen() {}

    public:
        virtual GYDM::IDisplay* display() = 0;
    
    public:
        virtual void feed_extent(float* width, float* height) = 0;
        virtual void feed_client_extent(float* width, float* height) = 0;
        virtual uint32_t frame_rate() = 0;
        virtual void refresh() = 0;
        
    public:
        virtual void begin_update_sequence() = 0;
        virtual bool is_in_update_sequence() = 0;
        virtual void end_update_sequence() = 0;
        virtual bool should_update() = 0;
        virtual void notify_updated() = 0;

    public:
        virtual void log_message(GYDM::Log level, const std::string& message) = 0;
        virtual void start_input_text(const std::string& prompt) = 0;
    };
}
