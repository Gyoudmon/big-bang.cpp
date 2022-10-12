#ifndef _WARGREY_STEM_ONIONSKIP_H
#define _WARGREY_STEM_ONIONSKIP_H

#include "../screen.hpp"

namespace WarGrey::STEM {
    class OnionSkip : public WarGrey::STEM::IScreen {
        public:
            OnionSkip(WarGrey::STEM::IDisplay* display)
                : _display(display), update_sequence_depth(0), update_is_needed(false) {}

        public:
            WarGrey::STEM::IDisplay* display() override { return this->_display; }
        
        public:
            void begin_update_sequence() override;
            bool in_update_sequence() override { return update_sequence_depth > 0; }
            void end_update_sequence() override;
            bool needs_update() override { return this->update_is_needed; }
            void notify_updated() override;

        public:
            void refresh() override;

        private:
            WarGrey::STEM::IDisplay* _display;

        private:
            int update_sequence_depth;
            bool update_is_needed;
    };
}

#endif 

