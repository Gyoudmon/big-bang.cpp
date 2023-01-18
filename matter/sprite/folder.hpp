#pragma once

#include "../sprite.hpp"

#include <vector>

namespace WarGrey::STEM {
    class Sprite : public WarGrey::STEM::ISprite {
    public:
        Sprite(const char* pathname, WarGrey::STEM::MatterAnchor resize_anchor = MatterAnchor::CC);
        Sprite(const std::string& pathname, WarGrey::STEM::MatterAnchor resize_anchor = MatterAnchor::CC);
        virtual ~Sprite();

        void pre_construct(SDL_Renderer* renderer) override;
    
    public:
        size_t custome_count() override;

    protected:
        void feed_custome_extent(int idx, float* width, float* height) override;
        const std::string& custome_index_to_name(int idx) override;
        int custome_name_to_index(const char* name) override;
        void draw_custome(SDL_Renderer* renderer, int idx, float x, float y, float Width, float Height) override;

    private:
        void load_custome(SDL_Renderer* renderer, std::string& png);
        void push_custome(const std::string& name, SDL_Texture* custome);
        
    private:
        std::vector<std::pair<std::string, SDL_Texture*>> customes;
        std::string _pathname;
    };
}
