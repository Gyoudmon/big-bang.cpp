#include "atlas.hpp"

#include "../datum/box.hpp"
#include "../datum/path.hpp"
#include "../datum/string.hpp"
#include "../datum/flonum.hpp"
#include "../datum/fixnum.hpp"

#include "../plane.hpp"
#include "../graphics/geometry.hpp"
#include "../graphics/colorspace.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::IAtlas::IAtlas(const std::string& pathname) : _pathname(pathname) {
    this->enable_resize(true);
    this->camouflage(true);
}

const char* WarGrey::STEM::IAtlas::name() {
    return file_basename_from_path(this->_pathname.c_str()).c_str();
}

void WarGrey::STEM::IAtlas::construct(SDL_Renderer* renderer) {
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
        this->on_map_resize(this->map_width, this->map_height);
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

size_t WarGrey::STEM::IAtlas::logic_tile_count() {
    return this->logic_row * this->logic_col;
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
        int xoff = 0;
        int yoff = 0;
        int primitive_tile_idx = this->get_atlas_tile_index(idx, xoff, yoff);

        if (primitive_tile_idx >= 0) {
            /** NOTE
             * The source rectangle can be larger than the tilemap,
             *   and it's okay, the larger part is simply ignored. 
             **/

            this->feed_atlas_tile_region(&src, primitive_tile_idx % idxmax);
            this->feed_map_tile_region(&dest, idx);

            if (xoff != 0) {
                src.x += xoff;
            }

            if (yoff != 0) {
                src.y += yoff;
            }

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

    if ((this->logic_grid_alpha > 0.0F) && (this->logic_col > 0) && (this->logic_row > 0)) {
        RGB_SetRenderDrawColor(renderer, this->logic_grid_color, this->logic_grid_alpha);
        game_draw_grid(renderer, this->logic_row, this->logic_col,
            this->logic_tile_width * flabs(this->xscale),
            this->logic_tile_height * flabs(this->yscale),
            x, y);
    }
}

/*************************************************************************************************/
void WarGrey::STEM::IAtlas::create_logic_grid(int row, int col) {
    float map_width, map_height;

    this->feed_map_extent(&map_width, &map_height);
    
    this->logic_row = row;
    this->logic_col = col;
    this->on_map_resize(map_width, map_height);
}

int WarGrey::STEM::IAtlas::logic_tile_index(int x, int y, int* r,  int* c, bool local) {
    if (!local) {
        auto master = this->master();

        if (master != nullptr) {
            float dx, dy;

            master->feed_matter_location(this, &dx, &dy, MatterAnchor::LT);
            x -= fl2fxi(dx);
            y -= fl2fxi(dy);
        }
    }
    
    int cl = x / int(this->logic_tile_width  * flabs(this->xscale));
    int rw = y / int(this->logic_tile_height * flabs(this->yscale));
    
    SET_VALUES(r, rw, c, cl);
    
    return rw * this->logic_col + cl;
}

int WarGrey::STEM::IAtlas::logic_tile_index(float x, float y, int* r, int* c, bool local) {
    return this->logic_tile_index(fl2fxi(x), fl2fxi(y), r, c, local);
}

void WarGrey::STEM::IAtlas::feed_logic_tile_location(int idx, float* x, float* y, MatterAnchor a, bool local) {
    int total = this->logic_col * this->logic_row;
    
    if (total > 0) {
        if (idx > total) {
            idx = idx % total;
        } else if (idx < 0) {
            idx = total - (-idx % total);
        }

        this->feed_logic_tile_location(idx / this->logic_col, idx / this->logic_row, x, y, a, local);
    }
}

void WarGrey::STEM::IAtlas::feed_logic_tile_location(int row, int col, float* x, float* y, MatterAnchor a, bool local) {
    float dx = 0.0F;
    float dy = 0.0F;
    float fx, fy;
    
    if (this->logic_row > 0) {
        if (row > this->logic_row) {
            row = row % this->logic_row;
        } else if (row < 0) {
            row = this->logic_row - (-row % this->logic_row);
        }
    }

    if (this->logic_col > 0) {
        if (col > this->logic_col) {
            col = col % this->logic_col;
        } else if (col < 0) {
            col = this->logic_col - (-col % this->logic_col);
        }
    }
    
    if (!local) {
        auto master = this->master();

        if (master != nullptr) {
            master->feed_matter_location(this, &dx, &dy, MatterAnchor::LT);
        }
    }
    
    matter_anchor_fraction(a, &fx, &fy);
    SET_BOX(x, this->logic_tile_width  * flabs(this->xscale) * (float(col) + fx) + dx);
    SET_BOX(y, this->logic_tile_height * flabs(this->yscale) * (float(row) + fy) + dy);
}

void WarGrey::STEM::IAtlas::on_map_resize(float map_width, float map_height) {
    if ((this->logic_row > 0) && (this->logic_col > 0)) {
        this->logic_tile_width  = map_width  / float(this->logic_col);
        this->logic_tile_height = map_height / float(this->logic_row);
    } else {
        this->logic_row = 0;
        this->logic_col = 0;
    }
}

/*************************************************************************************************/
WarGrey::STEM::GridAtlas::GridAtlas(const char* pathname, int row, int col, int xgap, int ygap, bool inset)
    : GridAtlas(std::string(pathname), row, col, xgap, ygap, inset) {}

WarGrey::STEM::GridAtlas::GridAtlas(const std::string& pathname, int row, int col, int xgap, int ygap, bool inset)
    : IAtlas(pathname), atlas_row(fxmax(row, 1)), atlas_col(fxmax(col, 1))
    , atlas_inset(inset), atlas_tile_xgap(xgap), atlas_tile_ygap(ygap) {}

void WarGrey::STEM::GridAtlas::on_tilemap_load(shared_costume_t atlas) {
    int w, h;

    atlas->feed_extent(&w, &h);

    if (this->atlas_inset) {
        w -= this->atlas_tile_xgap * 2;
        h -= this->atlas_tile_ygap * 2;
    }

    this->atlas_tile_width = (w - ((this->atlas_col - 1) * this->atlas_tile_xgap)) / this->atlas_col;
    this->atlas_tile_height = (h - ((this->atlas_row - 1) * this->atlas_tile_ygap)) / this->atlas_row;

    if (this->map_row <= 0) {
        this->map_row = this->atlas_row;
    }

    if (this->map_col <= 0) {
        this->map_col = this->atlas_col;
    }

    if (this->map_tile_width <= 0.0F) {
        this->map_tile_width = float(this->atlas_tile_width);
    }

    if (this->map_tile_height <= 0.0F) {
        this->map_tile_height = float(this->atlas_tile_height);
    }

    this->create_logic_grid(this->map_row, this->map_col);
}

void WarGrey::STEM::GridAtlas::feed_map_extent(float* width, float* height) {
    SET_BOX(width,  float(this->map_col) * (this->map_tile_width  + this->map_tile_xgap) - this->map_tile_xgap);
    SET_BOX(height, float(this->map_row) * (this->map_tile_height + this->map_tile_ygap) - this->map_tile_ygap);
}

size_t WarGrey::STEM::GridAtlas::atlas_tile_count() {
    return (this->atlas_tile_width <= 0) ? 0 : (this->atlas_row * this->atlas_col);
}

size_t WarGrey::STEM::GridAtlas::map_tile_count() {
    return (this->map_tile_width <= 0.0F) ? 0 : (this->map_row * this->map_col);
}

void WarGrey::STEM::GridAtlas::feed_atlas_tile_region(SDL_Rect* region, size_t idx) {
    int r = int(idx) / this->atlas_col;
    int c = int(idx) % this->atlas_col;
    int xoff = 0;
    int yoff = 0;

    if (this->atlas_inset) {
        xoff = this->atlas_tile_xgap;
        yoff = this->atlas_tile_ygap;
    }

    region->x = c * (this->atlas_tile_width + this->atlas_tile_xgap)  + xoff;
    region->y = r * (this->atlas_tile_height + this->atlas_tile_ygap) + yoff;
    region->w = this->atlas_tile_width;
    region->h = this->atlas_tile_height;
}

void WarGrey::STEM::GridAtlas::feed_map_tile_region(SDL_FRect* region, size_t idx) {
    size_t r = idx / this->map_col;
    size_t c = idx % this->map_col;

    region->x = float(c) * (this->map_tile_width + this->map_tile_xgap);
    region->y = float(r) * (this->map_tile_height + this->map_tile_ygap);
    region->w = this->map_tile_width;
    region->h = this->map_tile_height;
}

/*************************************************************************************************/
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

int WarGrey::STEM::GridAtlas::map_tile_index(int x, int y, int* r,  int* c, bool local) {
    if (!local) {
        auto master = this->master();

        if (master != nullptr) {
            float dx, dy;

            master->feed_matter_location(this, &dx, &dy, MatterAnchor::LT);
            x -= fl2fxi(dx);
            y -= fl2fxi(dy);
        }
    }
    
    int cl = x / fl2fxi((this->map_tile_width  + this->map_tile_xgap) * flabs(this->xscale));
    int rw = y / fl2fxi((this->map_tile_height + this->map_tile_ygap) * flabs(this->yscale));
    
    SET_VALUES(r, rw, c, cl);
    
    return rw * this->map_col + cl;
}

int WarGrey::STEM::GridAtlas::map_tile_index(float x, float y, int* r, int* c, bool local) {
    return this->map_tile_index(fl2fxi(x), fl2fxi(y), r, c, local);
}

void WarGrey::STEM::GridAtlas::feed_map_tile_location(int idx, float* x, float* y, MatterAnchor a, bool local) {
    int total = this->map_col * this->map_row;
    SDL_FRect region;
    float fx, fy;
    float dx = 0.0F;
    float dy = 0.0F;
    
    if (idx > total) {
        idx = idx % total;
    } else if (idx < 0) {
        idx = total - (-idx % total);
    }

    if (!local) {
        auto master = this->master();

        if (master != nullptr) {
            master->feed_matter_location(this, &dx, &dy, MatterAnchor::LT);
        }
    }

    matter_anchor_fraction(a, &fx, &fy);
    this->feed_map_tile_region(&region, idx);
    
    SET_BOX(x, (region.x + region.w * fx + dx) * flabs(this->xscale));
    SET_BOX(y, (region.y + region.h * fy + dy) * flabs(this->yscale));
}

void WarGrey::STEM::GridAtlas::feed_map_tile_location(int row, int col, float* x, float* y, MatterAnchor a, bool local) {
    if (row > this->map_row) {
        row = row % this->map_row;
    } else if (row < 0) {
        row = this->map_row - (-row % this->map_row);
    }

    if (col > this->map_col) {
        col = col % this->map_col;
    } else if (col < 0) {
        col = this->map_col - (-col % this->map_col);
    }

    this->feed_map_tile_location(row * this->map_col + col, x, y, a, local);
}
