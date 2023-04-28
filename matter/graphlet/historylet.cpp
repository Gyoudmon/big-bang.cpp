#include "historylet.hpp"

#include "../../datum/box.hpp"
#include "../../datum/flonum.hpp"

#include "../../graphics/image.hpp"
#include "../../graphics/geometry.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::Historylet::Historylet(float width, float height, uint32_t hex, double alpha)
        : width(flabs(width)), height(flabs(height)), color(hex), alpha(alpha) {
    if (this->height == 0.0F) {
        this->height = this->width;
    }

    this->capacity = 0;
    this->clear();
    this->enable_resize(true);
}

void WarGrey::STEM::Historylet::feed_extent(float x, float y, float* w, float* h) {
    SET_VALUES(w, this->width, h, this->height);
}

void WarGrey::STEM::Historylet::on_resize(float w, float h, float width, float height) {
    this->width = flabs(w);
    this->height = flabs(h);
    this->invalidate_geometry();
}

void WarGrey::STEM::Historylet::draw(SDL_Renderer* renderer, float flx, float fly, float flwidth, float flheight) {
    if (this->diagram.use_count() == 0) {
        this->diagram = std::make_shared<Texture>(game_blank_image(renderer, fl2fxi(this->width) + 1, fl2fxi(this->height) + 1));
    }

    if (this->diagram->okay()) {
        if (this->needs_refresh_diagram) {
            SDL_Texture* origin = SDL_GetRenderTarget(renderer);
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

                SDL_SetRenderTarget(renderer, this->diagram->self());

                game_clear(renderer, 0U, 0.0);
                game_draw_lines(renderer, dots.data(), int(n), this->color, this->alpha);

                SDL_SetRenderTarget(renderer, origin);
            }

            this->needs_refresh_diagram = false;
        }

        game_render_texture(renderer, this->diagram->self(), flx, fly, flwidth, flheight);
    } else {
        fprintf(stderr, "无法绘制历史曲线：%s\n", SDL_GetError());
    }
}

/*************************************************************************************************/
void WarGrey::STEM::Historylet::invalidate_geometry() {
    if (this->diagram.use_count() > 0) {
        this->diagram.reset();
        this->clear_geometry();
    }
}

void WarGrey::STEM::Historylet::clear_geometry() {
    this->needs_refresh_diagram = true;
}

void WarGrey::STEM::Historylet::clear() {
    this->xmax = this->ymax = -infinity;
    this->xmin = this->ymin = +infinity;

    if (!this->raw_dots.empty()) {
        this->raw_dots.clear();
        this->clear_geometry();
        
        this->notify_updated();
    }
}

void WarGrey::STEM::Historylet::set_color(uint32_t hex, double alpha) {
    if ((this->color != hex) || (this->alpha != alpha)) {
        this->color = hex;
        this->alpha = alpha;
        this->clear_geometry();
        this->notify_updated();
    }
}

void WarGrey::STEM::Historylet::set_capacity(size_t n) {
    if (this->capacity != n) {
        this->capacity = n;

        if (this->capacity < this->raw_dots.size()) {
            this->raw_dots.erase(this->raw_dots.begin(), this->raw_dots.end() - this->capacity);
        }

        this->clear_geometry();
        this->notify_updated();
    }
}

void WarGrey::STEM::Historylet::push_back_datum(float x, float y) {
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

        this->clear_geometry();
        this->notify_updated();
    }
}
