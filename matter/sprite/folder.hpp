#pragma once

#include "../sprite.hpp"
#include "../../virtualization/filesystem/imgdb.hpp"

#include <vector>
#include <unordered_map>

namespace WarGrey::STEM {
    class Sprite : public WarGrey::STEM::ISprite {
    public:
        Sprite(const std::string& pathname);
        Sprite(const char* pathname_fmt, ...);
        virtual ~Sprite() {}

        void construct(SDL_Renderer* renderer) override;
        const char* name() override;
    
    public:
        void wear(const char* name) { this->wear(std::string(name)); }
        void wear(const std::string& name);
        bool is_wearing() { return !this->current_decorate.empty(); }
        const char* decorate_name() { return this->current_decorate.c_str(); }
        void take_off();

    public:
        size_t costume_count() override;

    protected:
        void feed_costume_extent(size_t idx, float* width, float* height) override;
        const char* costume_index_to_name(size_t idx) override;
        void draw_costume(SDL_Renderer* renderer, size_t idx, SDL_Rect* src, SpriteRenderArguments* argv) override;
    
    protected:
        virtual void on_costumes_load() {}

    private:
        void load_costume(SDL_Renderer* renderer, const std::string& png);
        void load_decorate(SDL_Renderer* renderer, const std::string& d_name, const std::string& png);
        
    private:
        std::vector<std::pair<std::string, shared_texture_t>> costumes;
        std::unordered_map<std::string, std::unordered_map<std::string, shared_texture_t>> decorates;
        std::string current_decorate;

    private:
        std::string _pathname;
    };
}
