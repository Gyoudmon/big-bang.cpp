#include "historylet.hpp"

#include "../../../datum/box.hpp"
#include "../../../datum/flonum.hpp"

#include "../../../graphics/image.hpp"

using namespace GYDM;

/*************************************************************************************************/
GYDM::Historylet::Historylet(float width, float height, const RGBA& line_color)
        : width(flabs(width)), height(flabs(height)) {
    if (this->height == 0.0F) {
        this->height = this->width;
    }

    this->capacity = 0;
    this->clear();
    this->set_pen_color(line_color);
}

Box GYDM::Historylet::get_bounding_box() {
    return { this->width, this->height };
}

void GYDM::Historylet::on_resize(float w, float h, float width, float height) {
    ICanvaslet::on_resize(w, h, width, height);

    this->width = flabs(w);
    this->height = flabs(h);
}

void GYDM::Historylet::draw_on_canvas(GYDM::dc_t* dc, float flwidth, float flheight) {
    size_t n = this->raw_dots.size();
    float xrange = flmax(this->xmax - this->xmin, flwidth);
    float yrange = flmax(this->ymax - this->ymin, flheight);

    if (n > 1) {
        float xratio = flwidth / xrange;
        float yratio = flheight / yrange;
        std::vector<SDL_FPoint> dots(n);

        for (size_t idx = 0; idx < n; idx ++) {
            float X = this->raw_dots[idx].first;
            float Y = this->raw_dots[idx].second;
                    
            dots[idx] = { (X - this->xmin) * xratio, flheight - (Y - this->ymin) * yratio };
        }

        if (this->pen_okay()) {
            dc->draw_lines(dots.data(), int(n), this->get_pen_color());
        }
    }
}

/*************************************************************************************************/
void GYDM::Historylet::clear() {
    this->xmax = this->ymax = -infinity;
    this->xmin = this->ymin = +infinity;

    if (!this->raw_dots.empty()) {
        this->raw_dots.clear();
        this->dirty_canvas();
    }
}

void GYDM::Historylet::set_capacity(size_t n) {
    if (this->capacity != n) {
        this->capacity = n;

        if (this->capacity < this->raw_dots.size()) {
            this->raw_dots.erase(this->raw_dots.begin(), this->raw_dots.end() - this->capacity);
        }

        this->dirty_canvas();
    }
}

void GYDM::Historylet::push_back_datum(float x, float y) {
    if (this->raw_dots.empty() || (this->raw_dots.back().first != x)) {
        this->raw_dots.push_back({ x , y });

        if (this->capacity > 1) {
            // Yes, the ranges are not affected
            if (this->raw_dots.size() > this->capacity) {
                this->raw_dots.erase(this->raw_dots.begin());
            }
        }
        
        // don't merge with `else if`
        if (x < this->xmin) this->xmin = x;
        if (x > this->xmax) this->xmax = x;
        if (y < this->ymin) this->ymin = y;
        if (y > this->ymax) this->ymax = y;

        this->dirty_canvas();
    }
}
