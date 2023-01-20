#include "sheet.hpp"

#include "../../datum/box.hpp"
#include "../../datum/path.hpp"
#include "../../datum/string.hpp"

#include "../../graphics/geometry.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::ISpriteSheet::ISpriteSheet(const std::string& pathname, MatterAnchor resize_anchor) : _pathname(pathname) {
    this->enable_resize(true, resize_anchor);
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
WarGrey::STEM::SpriteGridSheet::SpriteGridSheet(const char* pathname, int row, int col, int xgap, int ygap)
    : SpriteGridSheet(std::string(pathname), row, col, xgap, ygap) {}

WarGrey::STEM::SpriteGridSheet::SpriteGridSheet(const std::string& pathname, int row, int col, int xgap, int ygap)
    : ISpriteSheet(pathname, MatterAnchor::CC), row(row), col(col), grid_xgap(xgap), grid_ygap(ygap) {}

void WarGrey::STEM::SpriteGridSheet::on_sheet_load(shared_costume_t sprite_sheet) {
    sprite_sheet->feed_extent(&this->grid_width, &this->grid_height);
    this->grid_width = (this->grid_width - ((this->col - 1) * this->grid_xgap)) / this->col;
    this->grid_height = (this->grid_height - ((this->row - 1) * this->grid_ygap)) / this->row;
}

size_t WarGrey::STEM::SpriteGridSheet::costume_count() {
    return (this->grid_width == 0) ? 0 : this->row * this->col;
}

void WarGrey::STEM::SpriteGridSheet::feed_costume_region(SDL_Rect* region, int idx) {
    int r = idx / this->col;
    int c = idx % this->col;

    region->x = c * (this->grid_width + this->grid_xgap);
    region->y = r * (this->grid_height + this->grid_ygap);
    region->w = this->grid_width;
    region->h = this->grid_height;
}

const char* WarGrey::STEM::SpriteGridSheet::costume_index_to_name(int idx) {
    int r = idx / this->col;
    int c = idx % this->col;
    
    return this->costume_grid_to_name(r, c);
}

int WarGrey::STEM::SpriteGridSheet::costume_name_to_index(const char* name) {
    int idx = -1;

    for (int r = 0; r < this->row; r++) {
        for (int c = 0; c < this->col; c++) {
            if (strcmp(this->costume_grid_to_name(r, c), name) == 0) {
                idx = r * this->col + c;
                break;
            }
        }
    }

    return idx;
}

const char* WarGrey::STEM::SpriteGridSheet::costume_grid_to_name(int r, int c) {
    this->__virtual_name = std::to_string(r) + "-" + std::to_string(c);

    return this->__virtual_name.c_str();
}
