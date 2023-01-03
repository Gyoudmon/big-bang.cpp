#pragma once

#include "../matter.hpp"
#include "../graphics/image.hpp"

#include "movable.hpp"

#include <vector>
#include <string>

namespace WarGrey::STEM {
    class ISprite : public WarGrey::STEM::IMatter, public WarGrey::STEM::IMovable {
        public:
            ISprite();
            virtual ~ISprite();

        public:
            void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
            void draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) override;

        public:
            size_t custome_count();
            void switch_to_custome(int idx);
            void switch_to_custome(const std::string& name);
            void switch_to_custome(const char* name);
            void switch_to_prev_custome() { this->switch_to_custome(this->current_custome_idx - 1); }
            void switch_to_next_custome() { this->switch_to_custome(this->current_custome_idx + 1); }

        protected:
            void push_custome(const std::string& name, SDL_Surface* custome);
            void push_custome(const char* name, SDL_Surface* custome);

        private:
            std::vector<std::string> names;
            std::vector<SDL_Surface*> customes;

        private:
            int current_custome_idx = 0;
    };

    class Sprite : public WarGrey::STEM::ISprite {
        public:
            Sprite(const char* pathname) : Sprite(std::string(pathname)) {}
            Sprite(const std::string& pathname) : _pathname(pathname) {}

            void pre_construct() override;

        private:
            void load_custome(std::string& png);
            
        private:
            std::string _pathname;
    };
}
