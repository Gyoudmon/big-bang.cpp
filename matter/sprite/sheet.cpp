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

WarGrey::STEM::ISpriteSheet::~ISpriteSheet() {
    if (this->sprite_sheet != nullptr) {
        game_unload_image(this->sprite_sheet);
    }
}

void WarGrey::STEM::ISpriteSheet::pre_construct(SDL_Renderer* renderer) {
    this->sprite_sheet = game_load_image_as_texture(renderer, this->_pathname);

    if (this->sprite_sheet != nullptr) {
        this->on_sheet_load(this->sprite_sheet);
    }
}

void WarGrey::STEM::ISpriteSheet::feed_custome_extent(int idx, float* width, float* height) {
    this->feed_custome_region(&this->custome_region, idx);

    SET_BOX(width, float(this->custome_region.w));
    SET_BOX(height, float(this->custome_region.h));
}

void WarGrey::STEM::ISpriteSheet::draw_custome(SDL_Renderer* renderer, int idx, float x, float y, float Width, float Height) {
    SDL_FRect dest;

    dest.x = x;
    dest.y = y;
    dest.w = Width;
    dest.h = Height;

    this->feed_custome_region(&this->custome_region, idx);
    game_render_texture(renderer, this->sprite_sheet, &this->custome_region, &dest, this->current_flip_status());
}

/*************************************************************************************************/
WarGrey::STEM::SpriteGridSheet::SpriteGridSheet(const char* pathname, int row, int col, int xgap, int ygap)
    : SpriteGridSheet(std::string(pathname), row, col, xgap, ygap) {}

WarGrey::STEM::SpriteGridSheet::SpriteGridSheet(const std::string& pathname, int row, int col, int xgap, int ygap)
    : ISpriteSheet(pathname, MatterAnchor::CC), row(row), col(col), grid_xgap(xgap), grid_ygap(ygap) {}

void WarGrey::STEM::SpriteGridSheet::on_sheet_load(SDL_Texture* texture) {
    SDL_QueryTexture(texture, nullptr, nullptr, &this->grid_width, &this->grid_height);
    this->grid_width = (this->grid_width - ((this->col - 1) * this->grid_xgap)) / this->col;
    this->grid_height = (this->grid_height - ((this->row - 1) * this->grid_ygap)) / this->row;
}

size_t WarGrey::STEM::SpriteGridSheet::custome_count() {
    return (this->grid_width == 0) ? 0 : this->row * this->col;
}

void WarGrey::STEM::SpriteGridSheet::feed_custome_region(SDL_Rect* region, int idx) {
    int r = idx / this->col;
    int c = idx % this->col;

    region->x = c * (this->grid_width + this->grid_xgap);
    region->y = r * (this->grid_height + this->grid_ygap);
    region->w = this->grid_width;
    region->h = this->grid_height;
}

const std::string& WarGrey::STEM::SpriteGridSheet::custome_index_to_name(int idx) {
    static std::string name = std::to_string(idx);

    return name;
}

int WarGrey::STEM::SpriteGridSheet::custome_name_to_index(const char* name) {
    return std::atoi(name);
}
