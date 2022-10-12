#ifndef _WARGREY_STEM_SCREEN_H
#define _WARGREY_STEM_SCREEN_H

#include "../display.hpp"

namespace WarGrey::STEM {
    class IScreen {
        public:
            IScreen() {}
            virtual ~IScreen() {}

        public:
            virtual WarGrey::STEM::IDisplay* display() = 0;
        
        public:
            virtual void begin_update_sequence() = 0;
            virtual bool in_update_sequence() = 0;
            virtual void end_update_sequence() = 0;
            virtual bool needs_update() = 0;
            virtual void notify_updated() = 0;

        public:
            virtual void refresh() = 0;
    };
}

#endif

