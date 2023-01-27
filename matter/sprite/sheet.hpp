#pragma once

#include "../sprite.hpp"
#include "../../virtualization/filesystem/imgdb.hpp"

namespace WarGrey::STEM {
    class ISpriteSheet : public WarGrey::STEM::ISprite {
    public:
        ISpriteSheet(const std::string& pathname);
        ISpriteSheet(const char* pathname) : ISpriteSheet(std::string(pathname)) {}
        virtual ~ISpriteSheet() {}

        void pre_construct(SDL_Renderer* renderer) override;

    protected:
        virtual void on_sheet_load(WarGrey::STEM::shared_costume_t sheet) = 0;
        virtual void feed_costume_region(SDL_Rect* costume, int idx) = 0;

    protected:
        void feed_costume_extent(int idx, float* width, float* height) override;
        void draw_costume(SDL_Renderer* renderer, int idx, float x, float y, float Width, float Height) override;
        
    private:
        WarGrey::STEM::shared_costume_t sprite_sheet;
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

    protected:
        void on_sheet_load(WarGrey::STEM::shared_costume_t sheet) override;
        void feed_costume_region(SDL_Rect* costume, int idx) override;
        const char* costume_index_to_name(int idx) override;
        int costume_name_to_index(const char* name) override;
    
    protected:
        int row;
        int col;

    private:
        int grid_inset = false;
        int grid_width = 0;
        int grid_height = 0;
        int grid_xgap;
        int grid_ygap;

    private:
        std::string __virtual_name;
    };
}
