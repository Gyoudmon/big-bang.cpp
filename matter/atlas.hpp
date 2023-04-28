#pragma once

#include "../matter.hpp"
#include "../virtualization/filesystem/imgdb.hpp"

namespace WarGrey::STEM {
    class IAtlas : public WarGrey::STEM::IMatter {
    public:
        IAtlas(const std::string& pathname);
        IAtlas(const char* pathname) : IAtlas(std::string(pathname)) {}
        virtual ~IAtlas() {}

        void construct(SDL_Renderer* renderer) override;
        const char* name() override;

    public:
        void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
        void feed_original_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
        void feed_margin(float x, float y, float* top = nullptr, float* right = nullptr, float* bottom = nullptr, float* left = nullptr) override;
        void draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) override;
        
    public:
        virtual size_t atlas_tile_count() = 0;
        virtual size_t map_tile_count() = 0;

    public:
        int preferred_local_fps() override { return 4; }
        
    public:
        size_t logic_tile_count();
        void create_logic_grid(int row, int col, float top = 0.0F, float right = 0.0F, float bottom = 0.0F, float left = 0.0F);
        int logic_tile_index(int x, int y, int* r = nullptr, int* c = nullptr, bool local = true);
        int logic_tile_index(float x, float y, int* r = nullptr, int* c = nullptr, bool local = true);
        void feed_logic_tile_extent(float* width = nullptr, float* height = nullptr);
        void feed_logic_tile_fraction(int idx, float* fx, float* fy, MatterAnchor a = MatterAnchor::CC);
        void feed_logic_tile_fraction(int row, int col, float* fx, float* fy, MatterAnchor a = MatterAnchor::CC);
        void feed_logic_tile_location(int idx, float* x, float* y, MatterAnchor a = MatterAnchor::CC, bool local = true);
        void feed_logic_tile_location(int row, int col, float* x, float* y, MatterAnchor a = MatterAnchor::CC, bool local = true);
        void set_logic_grid_color(uint32_t color, float a = 1.0F) { this->logic_grid_color = color; this->logic_grid_alpha = a; }

    public:
        void move_to_logic_tile(IMatter* m, int idx, MatterAnchor ta, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
        void move_to_logic_tile(IMatter* m, int row, int col, MatterAnchor ta, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
        void glide_to_logic_tile(double sec, IMatter* m, int idx, MatterAnchor ta, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
        void glide_to_logic_tile(double sec, IMatter* m, int row, int col, MatterAnchor ta, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
        
    protected:
        virtual int get_atlas_tile_index(size_t map_idx, int& xoff, int& yoff) { return int(map_idx); }
        virtual void feed_map_extent(float* width, float* height) = 0;
        virtual void on_tilemap_load(WarGrey::STEM::shared_texture_t atlas) = 0;
        virtual void feed_atlas_tile_region(SDL_Rect* tile, size_t idx) = 0;
        virtual void feed_map_tile_region(SDL_FRect* tile, size_t idx) = 0;

    protected:
        void on_resize(float width, float height, float old_width, float old_height) override;
        
    protected:
        void invalidate_map_size() { this->map_width = -1.0F; }
        void on_map_resize(float map_width, float map_height);
        SDL_RendererFlip current_flip_status();
        float get_horizontal_scale();
        float get_vertical_scale();
        
    protected:
        float xscale = 1.0F;
        float yscale = 1.0F;

    private:
        WarGrey::STEM::shared_texture_t atlas;

    private:
        float map_width = -1.0F;
        float map_height = 0.0F;

    private:
        int logic_row = 0;
        int logic_col = 0;
        float logic_top = 0.0F;
        float logic_right = 0.0F;
        float logic_bottom = 0.0F;
        float logic_left = 0.0F;
        float logic_tile_width = 0.0F;
        float logic_tile_height = 0.0F;
        uint32_t logic_grid_color = 0U;
        float logic_grid_alpha = 0.0F;

    private:
        std::string _pathname;
    };

    class GridAtlas : public WarGrey::STEM::IAtlas {
    public:
        GridAtlas(const char* pathname, int row = 1, int col = 1, int xgap = 0, int ygap = 0, bool inset = false);
        GridAtlas(const std::string& pathname, int row = 1, int col = 1, int xgap = 0, int ygap = 0, bool inset = false);

    public:
        size_t atlas_tile_count() override;
        size_t map_tile_count() override;
        float atlas_tile_size_ratio();
        float map_tile_size_ratio();

    public:
        int map_tile_index(int x, int y, int* r = nullptr, int* c = nullptr, bool local = true);
        int map_tile_index(float x, float y, int* r = nullptr, int* c = nullptr, bool local = true);
        void feed_map_tile_fraction(int idx, float* fx, float* fy, MatterAnchor a = MatterAnchor::CC);
        void feed_map_tile_fraction(int row, int col, float* fx, float* fy, MatterAnchor a = MatterAnchor::CC);
        void feed_map_tile_location(int idx, float* x, float* y, MatterAnchor a = MatterAnchor::CC, bool local = true);
        void feed_map_tile_location(int row, int col, float* x, float* y, MatterAnchor a = MatterAnchor::CC, bool local = true);
        void feed_map_overlay(float* top = nullptr, float* right = nullptr, float* bottom = nullptr, float* left = nullptr);
        
    public:
        void move_to_map_tile(IMatter* m, int idx, MatterAnchor ta, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
        void move_to_map_tile(IMatter* m, int row, int col, MatterAnchor ta, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
        void glide_to_map_tile(double sec, IMatter* m, int idx, MatterAnchor ta, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);
        void glide_to_map_tile(double sec, IMatter* m, int row, int col, MatterAnchor ta, MatterAnchor a, float dx = 0.0F, float dy = 0.0F);

    protected:
        virtual void feed_original_map_overlay(float* top, float* right, float* bottom, float* left);

    protected:
        void create_map_grid(int row, int col, float tile_width = 0.0F, float tile_height = 0.0F, float xgap = 0.0F, float ygap = 0.0F);
        
    protected:
        void on_tilemap_load(WarGrey::STEM::shared_texture_t atlas) override;
        void feed_map_extent(float* width, float* height) override;
        void feed_atlas_tile_region(SDL_Rect* tile, size_t idx) override;
        void feed_map_tile_region(SDL_FRect* tile, size_t idx) override;
    
    protected:
        int atlas_row;
        int atlas_col;
        int map_row = 0;
        int map_col = 0;

    protected:
        float map_tile_xgap = 0.0F;
        float map_tile_ygap = 0.0F;
        float map_tile_width = 0.0F;
        float map_tile_height = 0.0F;

    private:
        bool atlas_inset; 
        int atlas_tile_xgap = 0;
        int atlas_tile_ygap = 0;
        int atlas_tile_width = 0;
        int atlas_tile_height = 0;
    };
}
