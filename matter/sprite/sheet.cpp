#include "sheet.hpp"

#include "../../datum/box.hpp"
#include "../../datum/fixnum.hpp"

#include "../../graphics/geometry.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::ISpriteSheet::ISpriteSheet(const std::string& pathname) : _pathname(pathname) {
    this->enable_resize(true);
}

void WarGrey::STEM::ISpriteSheet::pre_construct(SDL_Renderer* renderer) {
    this->sprite_sheet = imgdb_ref(this->_pathname, renderer);

    if (this->sprite_sheet->okay()) {
        this->on_sheet_load(this->sprite_sheet);
    }
}

void WarGrey::STEM::ISpriteSheet::feed_costume_extent(int idx, float* width, float* height) {
    this->feed_costume_region(&this->costume_region, idx);

    SET_BOX(width, float(this->costume_region.w));
    SET_BOX(height, float(this->costume_region.h));
}

void WarGrey::STEM::ISpriteSheet::draw_costume(SDL_Renderer* renderer, int idx, float x, float y, float Width, float Height) {
    SDL_FRect dest;

    dest.x = x;
    dest.y = y;
    dest.w = Width;
    dest.h = Height;

    this->feed_costume_region(&this->costume_region, idx);
    game_render_texture(renderer, this->sprite_sheet->texture(), &this->costume_region, &dest, this->current_flip_status());
}

/*************************************************************************************************/
WarGrey::STEM::SpriteGridSheet::SpriteGridSheet(const char* pathname, int row, int col, int xgap, int ygap, bool inset)
    : SpriteGridSheet(std::string(pathname), row, col, xgap, ygap) {}

WarGrey::STEM::SpriteGridSheet::SpriteGridSheet(const std::string& pathname, int row, int col, int xgap, int ygap, bool inset)
    : ISpriteSheet(pathname)
    , row(fxmax(row, 1)), col(fxmax(col, 1))
    , grid_inset(inset)
    , grid_xgap(xgap), grid_ygap(ygap) {}

void WarGrey::STEM::SpriteGridSheet::on_sheet_load(shared_costume_t sprite_sheet) {
    int w, h;

    sprite_sheet->feed_extent(&w, &h);

    if (this->grid_inset) {
        w -= this->grid_xgap * 2;
        h -= this->grid_ygap * 2;
    }

    this->grid_width = (w - ((this->col - 1) * this->grid_xgap)) / this->col;
    this->grid_height = (h - ((this->row - 1) * this->grid_ygap)) / this->row;
}

size_t WarGrey::STEM::SpriteGridSheet::costume_count() {
    return (this->grid_width == 0) ? 0 : this->row * this->col;
}

void WarGrey::STEM::SpriteGridSheet::feed_costume_region(SDL_Rect* region, int idx) {
    int r = idx / this->col;
    int c = idx % this->col;
    int xoff = 0;
    int yoff = 0;

    if (this->grid_inset) {
        xoff = this->grid_xgap;
        yoff = this->grid_ygap;
    }

    region->x = c * (this->grid_width + this->grid_xgap)  + xoff;
    region->y = r * (this->grid_height + this->grid_ygap) + yoff;
    region->w = this->grid_width;
    region->h = this->grid_height;
}

const char* WarGrey::STEM::SpriteGridSheet::costume_index_to_name(int idx) {
    this->__virtual_name = std::to_string(idx);
    
    return this->__virtual_name.c_str();
}

int WarGrey::STEM::SpriteGridSheet::costume_name_to_index(const char* name) {
    int idx = -1;

    for (int i = 0; i < this->row * this->col; i++) {
        if (strcmp(this->costume_index_to_name(i), name) == 0) {
            idx = i;
            break;
        }
    }

    return idx;
}
