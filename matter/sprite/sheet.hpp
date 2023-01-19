#pragma once

#include "../sprite.hpp"

namespace WarGrey::STEM {
    class ISpriteSheet : public WarGrey::STEM::ISprite {
    public:
        ISpriteSheet(const std::string& pathname, WarGrey::STEM::MatterAnchor resize_anchor);
        virtual ~ISpriteSheet();

        void pre_construct(SDL_Renderer* renderer) override;

    protected:
        virtual void on_sheet_load(SDL_Texture* sheet) = 0;
        virtual void feed_custome_region(SDL_Rect* custome, int idx) = 0;

    protected:
        void feed_custome_extent(int idx, float* width, float* height) override;
        void draw_custome(SDL_Renderer* renderer, int idx, float x, float y, float Width, float Height) override;
        
    private:
        SDL_Texture* sprite_sheet = nullptr;
        SDL_Rect custome_region;

    private:
        std::string _pathname;
    };

    class SpriteGridSheet : public WarGrey::STEM::ISpriteSheet {
    public:
        SpriteGridSheet(const char* pathname, int row, int col, int xgap = 0, int ygap = 0);
        SpriteGridSheet(const std::string& pathname, int row, int col, int xgap = 0, int ygap = 0);

    public:
        size_t custome_count() override;

    protected:
        void on_sheet_load(SDL_Texture* sheet) override;
        void feed_custome_region(SDL_Rect* custome, int idx) override;
        const std::string& custome_index_to_name(int idx) override;
        int custome_name_to_index(const char* name) override;

    protected:
        virtual const std::string& custome_grid_to_name(int r, int c);
    
    protected:
        int row;
        int col;

    private:
        int grid_width = 0;
        int grid_height = 0;
        int grid_xgap;
        int grid_ygap;

    private:
        std::string __virtual_name;
    };
}
