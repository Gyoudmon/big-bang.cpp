#include "sheet.hpp"

#include "../../datum/box.hpp"
#include "../../datum/path.hpp"
#include "../../datum/fixnum.hpp"
#include "../../datum/flonum.hpp"

#include "../../graphics/geometry.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::ISpriteSheet::ISpriteSheet(const char* pathname) : ISpriteSheet(std::string(pathname)) {}

WarGrey::STEM::ISpriteSheet::ISpriteSheet(const std::string& pathname) : _pathname(pathname) {
    this->enable_resize(true);
}

const char* WarGrey::STEM::ISpriteSheet::name() {
    static std::string _name;

    _name = file_basename_from_path(this->_pathname.c_str());

    return _name.c_str();
}

void WarGrey::STEM::ISpriteSheet::construct(SDL_Renderer* renderer) {
    this->sprite_sheet = imgdb_ref(this->_pathname, renderer);

    if (this->sprite_sheet->okay()) {
        this->on_sheet_load(this->sprite_sheet);
        ISprite::construct(renderer);
    }
}

void WarGrey::STEM::ISpriteSheet::feed_costume_extent(size_t idx, float* width, float* height) {
    this->feed_costume_region(&this->costume_region, idx);

    SET_BOX(width, float(this->costume_region.w));
    SET_BOX(height, float(this->costume_region.h));
}

void WarGrey::STEM::ISpriteSheet::draw_costume(SDL_Renderer* renderer, size_t idx, SDL_Rect* src, SpriteRenderArguments* argv) {
    this->feed_costume_region(&this->costume_region, idx);
    
    if (src == nullptr) {
        src = &this->costume_region;
    } else {
        src->x += this->costume_region.x;
        src->y += this->costume_region.y;
    }

    game_render_texture(renderer, this->sprite_sheet->self(), src, &argv->dst, argv->flip);
}

/*************************************************************************************************/
WarGrey::STEM::SpriteGridSheet::SpriteGridSheet(const char* pathname, int row, int col, int xgap, int ygap, bool inset)
    : SpriteGridSheet(std::string(pathname), row, col, xgap, ygap) {}

WarGrey::STEM::SpriteGridSheet::SpriteGridSheet(const std::string& pathname, int row, int col, int xgap, int ygap, bool inset)
    : ISpriteSheet(pathname), row(fxmax(row, 1)), col(fxmax(col, 1))
    , cell_inset(inset), cell_xgap(xgap), cell_ygap(ygap) {}

void WarGrey::STEM::SpriteGridSheet::on_sheet_load(shared_texture_t sprite_sheet) {
    int w, h;

    sprite_sheet->feed_extent(&w, &h);

    if (this->cell_inset) {
        w -= this->cell_xgap * 2;
        h -= this->cell_ygap * 2;
    }

    this->cell_width = (w - ((this->col - 1) * this->cell_xgap)) / this->col;
    this->cell_height = (h - ((this->row - 1) * this->cell_ygap)) / this->row;
}

size_t WarGrey::STEM::SpriteGridSheet::costume_count() {
    return (this->cell_width == 0) ? 0 : this->row * this->col;
}

int WarGrey::STEM::SpriteGridSheet::grid_cell_index(int x, int y, int* r,  int* c) {
    int xoff = (this->cell_inset ? this->cell_xgap : 0);
    int yoff = (this->cell_inset ? this->cell_ygap : 0);
    int cl = 0;
    int rw = 0;

    if ((this->cell_width > 0) && (this->cell_height > 0)) {
        cl = (x - xoff) / (this->cell_width  + this->cell_xgap);
        rw = (y - yoff) / (this->cell_height + this->cell_ygap);
    }
    
    SET_VALUES(r, rw, c, cl);
    
    return rw * this->col + cl;
}

int WarGrey::STEM::SpriteGridSheet::grid_cell_index(float x, float y, int* r, int* c) {
    return this->grid_cell_index(fl2fxi(x), fl2fxi(y), r, c);
}

void WarGrey::STEM::SpriteGridSheet::feed_costume_region(SDL_Rect* region, size_t idx) {
    int r = int(idx) / this->col;
    int c = int(idx) % this->col;
    int xoff = 0;
    int yoff = 0;

    if (this->cell_inset) {
        xoff = this->cell_xgap;
        yoff = this->cell_ygap;
    }

    region->x = c * (this->cell_width + this->cell_xgap)  + xoff;
    region->y = r * (this->cell_height + this->cell_ygap) + yoff;
    region->w = this->cell_width;
    region->h = this->cell_height;
}

const char* WarGrey::STEM::SpriteGridSheet::costume_index_to_name(size_t idx) {
    this->__virtual_name = std::to_string(idx);
    
    return this->__virtual_name.c_str();
}
