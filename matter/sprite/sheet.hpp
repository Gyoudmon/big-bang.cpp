#pragma once

#include "../sprite.hpp"
#include "../../virtualization/filesystem/imgdb.hpp"

namespace WarGrey::STEM {
    class ISpriteSheet : public WarGrey::STEM::ISprite {
    public:
        ISpriteSheet(const std::string& pathname);
        ISpriteSheet(const char* pathname);
        virtual ~ISpriteSheet() {}

        void construct(SDL_Renderer* renderer) override;
        const char* name() override;

    protected:
        virtual void on_sheet_load(WarGrey::STEM::shared_texture_t sheet) = 0;
        virtual void feed_costume_region(SDL_Rect* costume, size_t idx) = 0;

    protected:
        void feed_costume_extent(size_t idx, float* width, float* height) override;
        void draw_costume(SDL_Renderer* renderer, size_t idx, SDL_Rect* src, SpriteRenderArguments* argv) override;
        
    private:
        WarGrey::STEM::shared_texture_t sprite_sheet;
        SDL_Rect costume_region;

    private:
        std::string _pathname;
    };

    class SpriteGridSheet : public WarGrey::STEM::ISpriteSheet {
    public:
        SpriteGridSheet(const char* pathname, int row, int col, int xgap = 0, int ygap = 0, bool inset = false);
        SpriteGridSheet(const std::string& pathname, int row, int col, int xgap = 0, int ygap = 0, bool inset = false);

    public:
        size_t costume_count() override;
        int grid_cell_index(int x, int y, int* r = nullptr,  int* c = nullptr);
        int grid_cell_index(float x, float y, int* r = nullptr, int* c = nullptr);
        
    protected:
        void on_sheet_load(WarGrey::STEM::shared_texture_t sheet) override;
        void feed_costume_region(SDL_Rect* costume, size_t idx) override;
        const char* costume_index_to_name(size_t idx) override;
    
    protected:
        int row;
        int col;

    private:
        int cell_inset = false;
        int cell_width = 0;
        int cell_height = 0;
        int cell_xgap;
        int cell_ygap;

    private:
        std::string __virtual_name;
    };
}
