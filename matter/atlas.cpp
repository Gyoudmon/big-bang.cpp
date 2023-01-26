#include "atlas.hpp"

#include "../datum/box.hpp"
#include "../datum/string.hpp"
#include "../datum/flonum.hpp"
#include "../datum/fixnum.hpp"

#include "../graphics/geometry.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::IAtlas::IAtlas(const std::string& pathname) : _pathname(pathname) {
    this->enable_resize(true);
    this->camouflage(true);
}

void WarGrey::STEM::IAtlas::pre_construct(SDL_Renderer* renderer) {
    this->atlas = imgdb_ref(this->_pathname, renderer);

    if (this->atlas->okay()) {
        this->on_tilemap_load(this->atlas);
    }
}

void WarGrey::STEM::IAtlas::feed_extent(float x, float y, float* width, float* height) {
    float owidth, oheight;

    this->feed_original_extent(x, y, &owidth, &oheight);
    SET_BOX(width, owidth * flabs(this->xscale));
    SET_BOX(height, oheight * flabs(this->yscale));
}

void WarGrey::STEM::IAtlas::feed_original_extent(float x, float y, float* width, float* height) {
    if (this->map_width < 0.0F) {
        this->feed_map_extent(&this->map_width, &this->map_height);
    }

    SET_BOX(width, this->map_width);
    SET_BOX(height, this->map_height);
}

void WarGrey::STEM::IAtlas::feed_map_extent(float* width, float* height) {
    SDL_FRect map_tile_region;
    float map_width = 0.0F;
    float map_height = 0.0F;

    for (size_t idx = 0U; idx < this->map_tile_count(); idx ++) {
        this->feed_map_tile_region(&map_tile_region, idx);
        map_width = flmax(map_width, map_tile_region.x + map_tile_region.w);
        map_height = flmax(map_height, map_tile_region.y + map_tile_region.h);
    }

    SET_BOX(width, map_width);
    SET_BOX(height, map_height);
}

SDL_RendererFlip WarGrey::STEM::IAtlas::current_flip_status() {
    return game_scales_to_flip(this->xscale, this->yscale);
}

void WarGrey::STEM::IAtlas::on_resize(float width, float height, float old_width, float old_height) {
    float cwidth, cheight;

    this->feed_original_extent(0.0F, 0.0F, &cwidth, &cheight);

    if ((cwidth > 0.0F) && (cheight > 0.0F)) {
        this->xscale = width  / cwidth;
        this->yscale = height / cheight;
    }
}

void WarGrey::STEM::IAtlas::draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) {
    SDL_Texture* tilemap = this->atlas->texture();
    SDL_RendererFlip flip = this->current_flip_status();
    float sx = flabs(this->xscale);
    float sy = flabs(this->yscale);
    size_t idxmax = this->atlas_tile_count();
    SDL_Rect src;
    SDL_FRect dest;
    
    for (size_t idx = 0U; idx < this->map_tile_count(); idx ++) {
        int primitive_tile_idx = this->get_atlas_tile_index(idx);

        if (primitive_tile_idx >= 0) {
            /** NOTE
             * The source rectangle can be larger than the tilemap,
             *   and it's okay, the larger part is simply ignored. 
             **/

            this->feed_atlas_tile_region(&src, primitive_tile_idx % idxmax);
            this->feed_map_tile_region(&dest, idx);

            dest.w *= sx;
            dest.h *= sy;

            if (this->xscale >= 0.0F) {
                dest.x = dest.x * sx + x;    
            } else {
                dest.x = x + Width - dest.x * sx - dest.w;
            }

            if (this->yscale >= 0.0F) {
                dest.y = dest.y * sy + y;
            } else {
                dest.y = y + Height - dest.y * sy - dest.h;
            }

            game_render_texture(renderer, tilemap, &src, &dest, flip);
        }
    }
}

/*************************************************************************************************/
WarGrey::STEM::GridAtlas::GridAtlas(const char* pathname, int row, int col, int xgap, int ygap)
    : GridAtlas(std::string(pathname), row, col, xgap, ygap) {}

WarGrey::STEM::GridAtlas::GridAtlas(const std::string& pathname, int row, int col, int xgap, int ygap)
    : IAtlas(pathname)
    , atlas_row(fxmax(row, 1)), atlas_col(fxmax(col, 1)), map_row(atlas_row), map_col(atlas_col)
    , atlas_tile_xgap(xgap), atlas_tile_ygap(ygap), map_tile_xgap(0.0F), map_tile_ygap(0.0F) {}

void WarGrey::STEM::GridAtlas::on_tilemap_load(shared_costume_t atlas) {
    float w, h;

    atlas->feed_extent(&w, &h);
    this->atlas_tile_width = (w - (float(this->atlas_col - 1) * this->atlas_tile_xgap)) / this->atlas_col;
    this->atlas_tile_height = (h - (float(this->atlas_row - 1) * this->atlas_tile_ygap)) / this->atlas_row;
    this->map_tile_width = this->atlas_tile_width;
    this->map_tile_height = this->atlas_tile_height; 
}

void WarGrey::STEM::GridAtlas::feed_map_extent(float* width, float* height) {
    SET_BOX(width, float(this->map_col) * this->map_tile_width + float(this->map_col - 1) * this->map_tile_xgap);
    SET_BOX(height, float(this->map_row) * this->map_tile_height + float(this->map_row - 1) * this->map_tile_ygap);
}

size_t WarGrey::STEM::GridAtlas::atlas_tile_count() {
    return (this->atlas_tile_width == 0) ? 0 : (this->atlas_row * this->atlas_col);
}

size_t WarGrey::STEM::GridAtlas::map_tile_count() {
    return (this->map_tile_width == 0) ? 0 : (this->map_row * this->map_col);
}

void WarGrey::STEM::GridAtlas::feed_atlas_tile_region(SDL_Rect* region, int idx) {
    int r = idx / this->atlas_col;
    int c = idx % this->atlas_col;

    region->x = c * (this->atlas_tile_width + this->atlas_tile_xgap);
    region->y = r * (this->atlas_tile_height + this->atlas_tile_ygap);
    region->w = this->atlas_tile_width;
    region->h = this->atlas_tile_height;
}

void WarGrey::STEM::GridAtlas::feed_map_tile_region(SDL_FRect* region, int idx) {
    int r = idx / this->map_col;
    int c = idx % this->map_col;

    region->x = float(c) * (this->map_tile_width + this->map_tile_xgap);
    region->y = float(r) * (this->map_tile_height + this->map_tile_ygap);
    region->w = this->map_tile_width;
    region->h = this->map_tile_height;
}

void WarGrey::STEM::GridAtlas::create_map_grid(int row, int col, float tile_width, float tile_height, float xgap, float ygap) {
    if (row > 0) {
        this->map_row = row;
    }

    if (col > 0) {
        this->map_col = col;
    }

    if (tile_width > 0.0F) {
        this->map_tile_width = tile_width;
    }

    if (tile_height > 0.0F) {
        this->map_tile_height = tile_height;
    }

    this->map_tile_xgap = xgap;
    this->map_tile_ygap = ygap;

    this->invalidate_map_size();
}
