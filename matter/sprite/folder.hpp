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

        void pre_construct(SDL_Renderer* renderer) override;
    
    public:
        void wear(const char* name) { this->wear(std::string(name)); }
        void wear(const std::string& name);
        bool is_wearing() { return !this->current_decorate.empty(); }
        const char* decorate_name() { return this->current_decorate.c_str(); }
        void take_off();

    public:
        size_t costume_count() override;

    protected:
        void feed_costume_extent(int idx, float* width, float* height) override;
        const char* costume_index_to_name(int idx) override;
        int costume_name_to_index(const char* name) override;
        void draw_costume(SDL_Renderer* renderer, int idx, float x, float y, float Width, float Height) override;

    private:
        void load_costume(SDL_Renderer* renderer, const std::string& png);
        void load_decorate(SDL_Renderer* renderer, const std::string& d_name, const std::string& png);
        
    private:
        std::vector<std::pair<std::string, shared_costume_t>> costumes;
        std::unordered_map<std::string, std::unordered_map<std::string, shared_costume_t>> decorates;
        std::string current_decorate;

    private:
        std::string _pathname;
    };
}
