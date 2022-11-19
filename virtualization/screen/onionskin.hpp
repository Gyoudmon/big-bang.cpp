#pragma once

#include "../screen.hpp"

namespace WarGrey::STEM {
    class OnionSkin : public WarGrey::STEM::IScreen {
        public:
            OnionSkin(WarGrey::STEM::IDisplay* display) : _display(display) {}

        public:
            WarGrey::STEM::IDisplay* display() override { return this->_display; }
        
        public:
            void refresh() override { this->_display->refresh(); }
            void fill_extent(float* width, float* height) override { this->_display->fill_extent(width, height); };
        
        public:
            void begin_update_sequence() override { this->_display->begin_update_sequence(); }
            bool in_update_sequence() override { return this->_display->in_update_sequence(); }
            void end_update_sequence() override { this->_display->end_update_sequence(); }
            bool needs_update() override { return this->_display->needs_update(); }
            void notify_updated() override { this->_display->notify_updated(); }

        public:
            void log_message(int fgc, const std::string& message) override { this->_display->log_message(fgc, message); }
            void start_input_text(const std::string& prompt) override { this->_display->start_input_text(prompt); }

        private:
            WarGrey::STEM::IDisplay* _display;
    };
}

