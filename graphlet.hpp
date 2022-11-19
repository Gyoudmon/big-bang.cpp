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
            IGraphlet() { this->set_border_collision_strategy(BorderCollisionStrategy::IGNORE); }
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
            void fill_extent(float x, float y, float* w = nullptr, float* h = nullptr) override;
            void fill_location(float* x, float* y, WarGrey::STEM::GraphletAnchor a = GraphletAnchor::LT);

        public:
            void notify_updated();

        public:
            void send_message(int fgc, const char* fmt, ...);
            void send_message(int fgc, const std::string& msg);
            void send_message(const char* fmt, ...);

        public: // gameplay events
            virtual void on_border(float hoffset, float voffset);

        public:
            void camouflage(bool yes_no) { this->findable = !yes_no; }
            bool concealled() { return !this->findable; }

        public:
            void set_speed(float xspd, float yspd) { this->xspeed = xspd; this->yspeed = yspd; }
            void set_border_collision_strategy(WarGrey::STEM::BorderCollisionStrategy s);
            void set_border_collision_strategy(WarGrey::STEM::BorderCollisionStrategy vs, WarGrey::STEM::BorderCollisionStrategy hs);
            void set_border_collision_strategy(BorderCollisionStrategy ts, BorderCollisionStrategy rs, BorderCollisionStrategy bs, BorderCollisionStrategy ls);
            void fill_speed(float* xspd, float* yspd) { if (xspd != nullptr) (*xspd) = this->xspeed; if (yspd != nullptr) (*yspd) = this->yspeed; }

        public:
            IGraphletInfo* info = nullptr;

        private:
            WarGrey::STEM::GraphletAnchor anchor = WarGrey::STEM::GraphletAnchor::LT;
            float anchor_x;
            float anchor_y;

        private:
            bool findable = true;
            float xspeed = 0.0F;
            float yspeed = 0.0F;
            WarGrey::STEM::BorderCollisionStrategy border_collision_strategies[4];
    };
}

