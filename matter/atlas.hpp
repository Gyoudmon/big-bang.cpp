#pragma once

#include "../matter.hpp"
#include "../virtualization/filesystem/imgdb.hpp"

namespace WarGrey::STEM {
    class IAtlas : public WarGrey::STEM::IMatter {
    public:
        IAtlas(const std::string& pathname);
        IAtlas(const char* pathname) : IAtlas(std::string(pathname)) {}
        virtual ~IAtlas() {}

        void pre_construct(SDL_Renderer* renderer) override;

    public:
        void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
        void feed_original_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
        //void update(uint32_t count, uint32_t interval, uint32_t uptime) override;
        void draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) override;

    public:
        virtual size_t atlas_tile_count() = 0;
        virtual size_t map_tile_count() = 0;

    public:
        int preferred_local_fps() override { return 4; }

    protected:
        virtual void feed_map_extent(float* width, float* height);
        virtual int get_atlas_tile_index(int map_idx) { return map_idx; }
        virtual void on_tilemap_load(WarGrey::STEM::shared_costume_t atlas) = 0;
        virtual void feed_atlas_tile_region(SDL_Rect* tile, int idx) = 0;
        virtual void feed_map_tile_region(SDL_FRect* tile, int idx) = 0;

    protected:
        void on_resize(float width, float height, float old_width, float old_height) override;

    protected:
        void invalidate_map_size() { this->map_width = -1.0F; }
        SDL_RendererFlip current_flip_status();
        
    private:
        WarGrey::STEM::shared_costume_t atlas;

    private:
        float map_width = -1.0F;
        float map_height = 0.0F;
        float xscale = 1.0F;
        float yscale = 1.0F;

    private:
        std::string _pathname;
    };

    class GridAtlas : public WarGrey::STEM::IAtlas {
    public:
        GridAtlas(const char* pathname, int row = 1, int col = 1, int xgap = 0, int ygap = 0);
        GridAtlas(const std::string& pathname, int row = 1, int col = 1, int xgap = 0, int ygap = 0);

    public:
        size_t atlas_tile_count() override;
        size_t map_tile_count() override;

    protected:
        void create_map_grid(int row, int col, float tile_width = 0.0F, float tile_height = 0.0F, float xgap = 0.0F, float ygap = 0.0F);

    protected:
        void on_tilemap_load(WarGrey::STEM::shared_costume_t atlas) override;
        void feed_map_extent(float* width, float* height) override;
        void feed_atlas_tile_region(SDL_Rect* tile, int idx) override;
        void feed_map_tile_region(SDL_FRect* tile, int idx) override;
    
    protected:
        int atlas_row;
        int atlas_col;
        int map_row;
        int map_col;

    private:
        int atlas_tile_xgap = 0;
        int atlas_tile_ygap = 0;
        int atlas_tile_width = 0;
        int atlas_tile_height = 0;

    private:
        float map_tile_xgap = 0.0F;
        float map_tile_ygap = 0.0F;
        float map_tile_width = 0.0F;
        float map_tile_height = 0.0F;
    };
}
