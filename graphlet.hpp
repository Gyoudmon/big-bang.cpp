#pragma once

#include "sprite.hpp"

namespace WarGrey::STEM {
    class IGraphletInfo {
        public:
            virtual ~IGraphletInfo() {};
            IGraphletInfo(IPlanet* master) : master(master) {};
        
        public:
            IPlanet* master;
    };

    class IGraphlet : public WarGrey::STEM::ISprite {
        public:
            virtual ~IGraphlet();

        public:
            WarGrey::STEM::IPlanet* master();

        public:
            virtual void own_caret(bool is_own) {}
            virtual bool is_colliding_with_mouse(float local_x, float local_y) { return true; }

        public:
            bool has_caret();
            void moor(WarGrey::STEM::GraphletAnchor anchor);
            void clear_moor(); /* the notify_updated() will clear the moor,
                                  but the notification is not always guaranteed to be done,
                                  use this method to do it manually. */

        public:
            void fill_location(float* x, float* y, WarGrey::STEM::GraphletAnchor a = GraphletAnchor::LT);

        public:
            void notify_updated();

        public:
            void send_message(int fgc, const char* fmt, ...);
            void send_message(int fgc, const std::string& msg);
            void send_message(const char* fmt, ...);

        public:
            IGraphletInfo* info = nullptr;

        private:
            WarGrey::STEM::GraphletAnchor anchor = WarGrey::STEM::GraphletAnchor::LT;
            float anchor_x;
            float anchor_y;
    };
}

