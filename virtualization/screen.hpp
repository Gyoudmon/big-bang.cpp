#pragma once

#include "../display.hpp"

namespace WarGrey::STEM {
    class IScreen {
        public:
            IScreen() {}
            virtual ~IScreen() {}

        public:
            virtual WarGrey::STEM::IDisplay* display() = 0;
        
        public:
            virtual void fill_extent(float* width, float* height) = 0;
            virtual void refresh() = 0;
            
        public:
            virtual void begin_update_sequence() = 0;
            virtual bool in_update_sequence() = 0;
            virtual void end_update_sequence() = 0;
            virtual bool needs_update() = 0;
            virtual void notify_updated() = 0;

        public:
            virtual void log_message(int fgc, const std::string& message) = 0;
            virtual void start_input_text(const std::string& prompt) = 0;
    };
}

