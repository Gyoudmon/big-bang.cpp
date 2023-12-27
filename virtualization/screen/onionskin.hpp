#pragma once

#include "../screen.hpp"

namespace GYDM {
    class __lambda__ OnionSkin : public GYDM::IScreen {
    public:
        OnionSkin(GYDM::IDisplay* display) : _display(display) {}

    public:
        GYDM::IDisplay* display() override { return this->_display; }
    
    public:
        void refresh() override { this->_display->refresh(); }
        void feed_extent(float* width, float* height) override { this->_display->feed_extent(width, height); }
        void feed_client_extent(float* width, float* height) override { this->_display->feed_client_extent(width, height); }
        uint32_t frame_rate() override { return this->_display->frame_rate(); }
    
    public:
        void begin_update_sequence() override { this->_display->begin_update_sequence(); }
        bool is_in_update_sequence() override { return this->_display->is_in_update_sequence(); }
        void end_update_sequence() override { this->_display->end_update_sequence(); }
        bool should_update() override { return this->_display->should_update(); }
        void notify_updated() override { this->_display->notify_updated(); }

    public:
        void log_message(GYDM::Log level, const std::string& message) override { this->_display->log_message(level, message); }
        void start_input_text(const std::string& prompt) override { this->_display->start_input_text(prompt); }

    private:
        GYDM::IDisplay* _display;
    };
}
