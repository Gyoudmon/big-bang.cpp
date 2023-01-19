#pragma once

#include "../sprite.hpp"

#include <vector>
#include <unordered_map>

namespace WarGrey::STEM {
    class Sprite : public WarGrey::STEM::ISprite {
    public:
        Sprite(const char* pathname, WarGrey::STEM::MatterAnchor resize_anchor = MatterAnchor::CC);
        Sprite(const std::string& pathname, WarGrey::STEM::MatterAnchor resize_anchor = MatterAnchor::CC);
        virtual ~Sprite();

        void pre_construct(SDL_Renderer* renderer) override;
    
    public:
        void wear(const char* name) { this->wear(std::string(name)); }
        void wear(const std::string& name);
        bool is_wearing() { return !this->current_decorate.empty(); }
        const std::string& decorate_name() { return this->current_decorate; }
        void take_off();

    public:
        size_t custome_count() override;

    protected:
        void feed_custome_extent(int idx, float* width, float* height) override;
        const std::string& custome_index_to_name(int idx) override;
        int custome_name_to_index(const char* name) override;
        void draw_custome(SDL_Renderer* renderer, int idx, float x, float y, float Width, float Height) override;

    private:
        void load_custome(SDL_Renderer* renderer, const std::string& png);
        void load_decorate(SDL_Renderer* renderer, const std::string& d_name, const std::string& png);
        void push_custome(const std::string& name, SDL_Texture* custome);
        
    private:
        std::vector<std::pair<std::string, SDL_Texture*>> customes;
        std::unordered_map<std::string, std::unordered_map<std::string, SDL_Texture*>> decorates;
        std::string current_decorate;

    private:
        std::string _pathname;
    };
}
