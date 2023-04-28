#pragma once

#include <SDL2/SDL.h>

#include <memory>

namespace WarGrey::STEM {
    class Texture {
    public:
        Texture(SDL_Texture* raw) : _self(raw) {}
        virtual ~Texture() { if (this->okay()) SDL_DestroyTexture(this->_self); }

    public:
        bool okay() { return this->_self != nullptr; }
        SDL_Texture* self() { return this->_self; }
        void feed_extent(int* width, int* height);
        void feed_extent(float* width, float* height);
        
    private:
        SDL_Texture* _self = nullptr;
    };

    typedef std::shared_ptr<Texture> shared_texture_t;
}
