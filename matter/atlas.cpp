#include "atlas.hpp"

#include "../datum/box.hpp"
#include "../datum/path.hpp"
#include "../datum/string.hpp"
#include "../datum/flonum.hpp"
#include "../datum/fixnum.hpp"

#include "../plane.hpp"
#include "../graphics/misc.hpp"
#include "../physics/mathematics.hpp"

using namespace GYDM;

/*************************************************************************************************/
GYDM::IAtlas::IAtlas(const std::string& pathname) : _pathname(pathname) {
    this->enable_resize(true);
    this->camouflage(true);
}

const char* GYDM::IAtlas::name() {
    static std::string _name;

    _name = file_basename_from_path(this->_pathname.c_str());

    return _name.c_str();
}

void GYDM::IAtlas::construct(GYDM::dc_t* dc) {
    this->atlas = imgdb_ref(this->_pathname, dc->self());

    if (this->atlas->okay()) {
        this->on_tilemap_load(this->atlas);
    }
}

Box GYDM::IAtlas::get_original_bounding_box() {
    if (this->map_region.is_empty()) {
        this->map_region = this->get_map_region();
        this->on_map_resize(this->map_region.width(), this->map_region.height());
    }

    return this->map_region;
}

Box GYDM::IAtlas::get_bounding_box() {
    return Box(this->get_original_bounding_box(),
                flabs(this->xscale), flabs(this->yscale));
}

Margin GYDM::IAtlas::get_margin() {
    return Margin(this->get_original_margin(),
                flabs(this->xscale), flabs(this->yscale));
}

Box GYDM::IAtlas::get_map_region() {
    Box map_tile_region;
    
    for (size_t idx = 0U; idx < this->map_tile_count(); idx ++) {
        map_tile_region += this->get_map_tile_region(idx);
    }

    return map_tile_region;
}

size_t GYDM::IAtlas::logic_tile_count() {
    return this->logic_row * this->logic_col;
}

SDL_RendererFlip GYDM::IAtlas::current_flip_status() {
    return game_scales_to_flip(this->xscale, this->yscale);
}

float GYDM::IAtlas::get_horizontal_scale() {
    return flabs(this->xscale);
}

float GYDM::IAtlas::get_vertical_scale() {
    return flabs(this->yscale);
}

void GYDM::IAtlas::on_resize(float width, float height, float old_width, float old_height) {
    Box box = this->get_original_bounding_box();

    if (!box.is_empty()) {
        this->xscale = width  / box.width();
        this->yscale = height / box.height();
    }
}

void GYDM::IAtlas::draw(GYDM::dc_t* dc, float x, float y, float Width, float Height) {
    SDL_Texture* tilemap = this->atlas->self();
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
             * The source rectangle could be larger than the tilemap,
             *   and it's okay, the larger part is simply ignored. 
             **/
            
            feed_rect(&src, this->get_atlas_tile_region(primitive_tile_idx % idxmax));
            feed_rect(&dest, this->get_map_tile_region(idx));

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

            dc->stamp(tilemap, &src, &dest, flip);
        }
    }

    if (this->logic_grid_color.is_opacity() && (this->logic_col > 0) && (this->logic_row > 0)) {
        dc->draw_grid(this->logic_row, this->logic_col,
            this->logic_tile_width * sx, this->logic_tile_height * sy,
            this->logic_grid_color,
            x + this->logic_margin.left * sx, y + this->logic_margin.top * sy);
    }
}

/*************************************************************************************************/
void GYDM::IAtlas::create_logic_grid(int row, int col, const Margin& margin) {
    Box map = this->get_map_region();
    
    this->logic_margin = margin;
    this->logic_row = row;
    this->logic_col = col;
    this->on_map_resize(map.width(), map.height());
}

int GYDM::IAtlas::logic_tile_index(int x, int y, int* r,  int* c, bool local) {
    return this->logic_tile_index(float(x), float(y), r, c, local);
}

int GYDM::IAtlas::logic_tile_index(float x, float y, int* r, int* c, bool local) {
    int idx = -1;

    if (!local) {
        auto master = this->master();

        if (master != nullptr) {
            Dot dot = master->get_matter_location(this, MatterPort::LT);

            x -= dot.x;
            y -= dot.y;
        }
    }
    
    if ((x >= this->logic_margin.left) && (y >= this->logic_margin.top)) {
        int cl = int(flfloor((x - this->logic_margin.left) / (this->logic_tile_width  * flabs(this->xscale))));
        int rw = int(flfloor((y - this->logic_margin.top) / (this->logic_tile_height * flabs(this->yscale))));
    
        if ((rw < this->logic_row) && (cl < this->logic_col)) {
            SET_VALUES(r, rw, c, cl);
            idx = rw * this->logic_col + cl;
        }
    }

    return idx;
}

Dot GYDM::IAtlas::get_logic_tile_location(int idx, const Port& p, bool local) {
    int total = this->logic_col * this->logic_row;
    Dot dot;
    
    if (total > 0) {
        idx = safe_index(idx, total);
        dot = this->get_logic_tile_location(idx / this->logic_col, idx / this->logic_row, p, local);
    }

    return dot;
}

Dot GYDM::IAtlas::get_logic_tile_location(int row, int col, const Port& p, bool local) {
    Dot dot;
    
    if (this->logic_row > 0) {
        row = safe_index(row, this->logic_row);
    }

    if (this->logic_col > 0) {
        col = safe_index(col, this->logic_col);
    }
    
    if (!local) {
        auto master = this->master();

        if (master != nullptr) {
            dot = master->get_matter_location(this, MatterPort::LT);
        }
    }
    
    return dot + Dot((this->logic_tile_width * (float(col) + p.fx) + this->logic_margin.left) * flabs(this->xscale),
                     (this->logic_tile_height * (float(row) + p.fy) + this->logic_margin.top) * flabs(this->yscale));
}

Port GYDM::IAtlas::get_logic_tile_fraction(int idx, const Port& a) {
    int total = this->logic_col * this->logic_row;
    Port port;
    
    if (total > 0) {
        idx = safe_index(idx, total);
        port = this->get_logic_tile_fraction(idx / this->logic_col, idx / this->logic_row, a);
    }

    return port;
}

Port GYDM::IAtlas::get_logic_tile_fraction(int row, int col, const Port& p) {
    Box box = this->get_bounding_box();
    Dot dot = this->get_logic_tile_location(row, col, p, true);
    
    return { dot.x / box.width(),
             dot.y / box.height() };
}

void GYDM::IAtlas::move_to_logic_tile(IMatter* m, int idx, const Port& tp, const Port& p, const Vector& vec) {
    int total = this->logic_col * this->logic_row;
    
    if (total > 0) {
        idx = safe_index(idx, total);
        this->move_to_logic_tile(m, idx / this->logic_col, idx / this->logic_row, tp, p, vec);
    }
}

void GYDM::IAtlas::move_to_logic_tile(IMatter* m, int row, int col, const Port& tp, const Port& p, const Vector& vec) {
    auto master = this->master();
    
    if (master != nullptr) {
        master->move_to(m, this->get_logic_tile_location(row, col, tp, false), p, vec);
    }
}

void GYDM::IAtlas::glide_to_logic_tile(double sec, IMatter* m, int idx, const Port& tp, const Port& p, const Vector& vec) {
    int total = this->logic_col * this->logic_row;
    
    if (total > 0) {
        idx = safe_index(idx, total);
        this->glide_to_logic_tile(sec, m, idx / this->logic_col, idx / this->logic_row, tp, p, vec);
    }
}

void GYDM::IAtlas::glide_to_logic_tile(double sec, IMatter* m, int row, int col, const Port& tp, const Port& p, const Vector& vec) {
    auto master = this->master();
    
    if (master != nullptr) {
        master->glide_to(sec, m, get_logic_tile_location(row, col, tp, false), p, vec);
    }
}

Box GYDM::IAtlas::get_logic_tile_region() {
    Box box;

    if ((this->logic_col > 0) && (this->logic_row > 0)) {
        box = { this->logic_tile_width  * flabs(this->xscale),
                this->logic_tile_height * flabs(this->yscale) };
    }

    return box;
}

void GYDM::IAtlas::on_map_resize(float map_width, float map_height) {
    if ((this->logic_row > 0) && (this->logic_col > 0)) {
        this->logic_tile_width  = (map_width  - this->logic_margin.horizon()) / float(this->logic_col);
        this->logic_tile_height = (map_height - this->logic_margin.vertical()) / float(this->logic_row);
    } else {
        this->logic_row = 0;
        this->logic_col = 0;
    }
}

/*************************************************************************************************/
GYDM::GridAtlas::GridAtlas(const char* pathname, int row, int col, int xgap, int ygap, bool inset)
    : GridAtlas(std::string(pathname), row, col, xgap, ygap, inset) {}

GYDM::GridAtlas::GridAtlas(const std::string& pathname, int row, int col, int xgap, int ygap, bool inset)
    : IAtlas(pathname), atlas_row(fxmax(row, 1)), atlas_col(fxmax(col, 1))
    , atlas_inset(inset), atlas_tile_xgap(xgap), atlas_tile_ygap(ygap) {}

void GYDM::GridAtlas::on_tilemap_load(shared_texture_t atlas) {
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

    this->create_logic_grid(this->map_row, this->map_col, this->get_original_map_overlay());
}

Box GYDM::GridAtlas::get_map_region() {
    Margin margin = this->get_original_map_overlay();
    float hmargin = margin.horizon()  - this->map_tile_xgap;
    float vmargin = margin.vertical() - this->map_tile_ygap;

    return { float(this->map_col) * (this->map_tile_width  - hmargin) + hmargin,
             float(this->map_row) * (this->map_tile_height - vmargin) + vmargin };
}

size_t GYDM::GridAtlas::atlas_tile_count() {
    return (this->atlas_tile_width <= 0) ? 0 : (this->atlas_row * this->atlas_col);
}

float GYDM::GridAtlas::atlas_tile_size_ratio() {
    return float(this->atlas_tile_width) / float(this->atlas_tile_height);   
}

size_t GYDM::GridAtlas::map_tile_count() {
    return (this->map_tile_width <= 0.0F) ? 0 : (this->map_row * this->map_col);
}

float GYDM::GridAtlas::map_tile_size_ratio() {
    return float(this->map_tile_width) / float(this->map_tile_height);   
}

AABox<int> GYDM::GridAtlas::get_atlas_tile_region(size_t idx) {
    int r = int(idx) / this->atlas_col;
    int c = int(idx) % this->atlas_col;
    int xoff = 0;
    int yoff = 0;
    int x, y;

    if (this->atlas_inset) {
        xoff = this->atlas_tile_xgap;
        yoff = this->atlas_tile_ygap;
    }

    x = c * (this->atlas_tile_width + this->atlas_tile_xgap)  + xoff;
    y = r * (this->atlas_tile_height + this->atlas_tile_ygap) + yoff;

    return AABox<int>(x, y, this->atlas_tile_width, this->atlas_tile_height);
}

Box GYDM::GridAtlas::get_map_tile_region(size_t idx) {
    Margin margin = this->get_original_map_overlay();
    size_t row = idx / this->map_col;
    size_t col = idx % this->map_col;
    float x = float(col) * (this->map_tile_width + this->map_tile_xgap - margin.horizon());
    float y = float(row) * (this->map_tile_height + this->map_tile_ygap - margin.vertical());
    
    return Box(x, y, this->map_tile_width, this->map_tile_height);
}

/*************************************************************************************************/
void GYDM::GridAtlas::create_map_grid(int row, int col, float tile_width, float tile_height, float xgap, float ygap) {
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

int GYDM::GridAtlas::map_tile_index(int x, int y, int* r, int* c, bool local) {
    return this->map_tile_index(float(x), float(y), r, c, local);
}

int GYDM::GridAtlas::map_tile_index(float x, float y, int* r, int* c, bool local) {
    float htile_step = (this->map_tile_width  + this->map_tile_xgap) * flabs(this->xscale);
    float vtile_step = (this->map_tile_height + this->map_tile_ygap) * flabs(this->yscale);
    Margin margin = this->get_map_overlay();
    int cl ,rw;
    
    if (!local) {
        auto master = this->master();

        if (master != nullptr) {
            Dot dot = master->get_matter_location(this, MatterPort::LT);

            x -= dot.x;
            y -= dot.y;
        }
    }
    
    htile_step -= margin.horizon();
    vtile_step -= margin.vertical();

    cl = (x < margin.left) ? 0 : fxmin(int(flfloor((x - margin.left) / htile_step)), this->map_col - 1);
    rw = (y < margin.top) ? 0 : fxmin(int(flfloor((y - margin.top) / vtile_step)), this->map_row - 1);
    
    SET_VALUES(r, rw, c, cl);
    
    return rw * this->map_col + cl;
}

Port GYDM::GridAtlas::get_map_tile_fraction(int idx, const Port& p) {
    Box box = this->get_bounding_box();
    Dot dot = this->get_map_tile_location(idx, p, true);

    return { dot.x / box.width(), dot.y / box.height() };
}

Port GYDM::GridAtlas::get_map_tile_fraction(int row, int col, const Port& a) {
    row = safe_index(row, this->map_row);
    col = safe_index(col, this->map_col);
    
    return this->get_map_tile_fraction(row * this->map_col + col, a);
}

Dot GYDM::GridAtlas::get_map_tile_location(int idx, const Port& p, bool local) {
    int total = this->map_col * this->map_row;
    Box region;
    Dot dot;
    
    idx = safe_index(idx, total);
    region = this->get_map_tile_region(idx);

    if (!local) {
        auto master = this->master();

        if (master != nullptr) {
            dot = master->get_matter_location(this, MatterPort::LT);
        }
    }
    
    return { (region.x() + region.width()  * p.fx + dot.x) * flabs(this->xscale),
             (region.y() + region.height() * p.fy + dot.y) * flabs(this->yscale) };
}

Dot GYDM::GridAtlas::get_map_tile_location(int row, int col, const Port& p, bool local) {
    row = safe_index(row, this->map_row);
    col = safe_index(col, this->map_col);

    return this->get_map_tile_location(row * this->map_col + col, p, local);
}

void GYDM::GridAtlas::move_to_map_tile(IMatter* m, int idx, const Port& tp, const Port& p, const Vector& vec) {
    auto master = this->master();

    if (master != nullptr) {
        master->move_to(m, this->get_map_tile_location(idx, tp, false), p, vec);
    }
}

void GYDM::GridAtlas::move_to_map_tile(IMatter* m, int row, int col, const Port& tp, const Port& p, const Vector& vec) {
    row = safe_index(row, this->map_row);
    col = safe_index(col, this->map_col);
    this->move_to_map_tile(m, row * this->map_col + col, tp, p, vec);
}

void GYDM::GridAtlas::glide_to_map_tile(double sec, IMatter* m, int idx, const Port& tp, const Port& p, const Vector& vec) {
    auto master = this->master();

    if (master != nullptr) {
        master->glide_to(sec, m, this->get_map_tile_location(idx, tp, false), p, vec);
    }
}

void GYDM::GridAtlas::glide_to_map_tile(double sec, IMatter* m, int row, int col, const Port& tp, const Port& p, const Vector& vec) {
    row = safe_index(row, this->map_row);
    col = safe_index(col, this->map_col);
    this->glide_to_map_tile(sec, m, row * this->map_col + col, tp, p, vec);
}

GYDM::Margin GYDM::GridAtlas::get_map_overlay() {
    return this->get_original_map_overlay().scale(this->xscale, this->yscale);
}
