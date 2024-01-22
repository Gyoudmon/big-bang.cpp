#include "tracklet.hpp"

#include "../../datum/box.hpp"
#include "../../datum/flonum.hpp"

// https://www.ferzkopp.net/Software/SDL2_gfx/Docs/html/index.html
#include <SDL2/SDL2_gfxPrimitives.h>

using namespace GYDM;

/*************************************************************************************************/
GYDM::Tracklet::Tracklet(float width, float height, uint32_t hex, double alpha)
        : width(flabs(width)), height(flabs(height)), line_width(1) {
    if (this->height == 0.0F) {
        this->height = this->width;
    }

    this->erase();
    this->set_pen_color(0U);
    this->enable_resize(false);
    this->camouflage(true);
}

Box GYDM::Tracklet::get_bounding_box() {
    return { this->width, this->height };
}

void GYDM::Tracklet::add_line(float x1, float y1, float x2, float y2) {
    if (this->is_drawing()) {
        if (this->canvas->okay()) {
            auto master = this->drawing_context();

            if (master != nullptr) {
                SDL_Texture* origin = master->get_target();
                short fx1 = fl2fx<short>(x1);
                short fy1 = fl2fx<short>(y1);
                short fx2 = fl2fx<short>(x2);
                short fy2 = fl2fx<short>(y2);
                uint8_t r, g, b, a;

                if (this->pen_okay(&r, &g, &b, &a)) {
                    master->set_target(this->canvas->self());
                
                    if (this->line_width <= 1) {
                        aalineRGBA(master->self(), fx1, fy1, fx2, fy2, r, g, b, a);
                    } else {
                        int radius = this->line_width / 2;

                        filledCircleRGBA(master->self(), fx1, fy1, radius, r, g, b, a);
                        filledCircleRGBA(master->self(), fx2, fy2, radius, r, g, b, a);
                        thickLineRGBA(master->self(), fx1, fy1, fx2, fy2, this->line_width, r, g, b, a);
                    }

                    SDL_SetRenderTarget(master->self(), origin);

                    this->resolve_boundary(x1, y1);
                    this->resolve_boundary(x2, y2);

                    this->dirty_canvas(0U, -1.0);
                }
            }
        }
    }
}

void GYDM::Tracklet::stamp(GYDM::IMatter* matter, float x, float y) {
    if (this->canvas->okay()) {
        auto master = this->drawing_context();

        if (master != nullptr) {
            SDL_Texture* origin = master->get_target();
            Box mbox = matter->get_bounding_box();
            float mwidth = mbox.width();
            float mheight = mbox.height();
                
            master->set_target(this->canvas->self());
            matter->draw(master, x, y, mwidth, mheight);
            master->set_target(origin);

            this->resolve_boundary(x, y);
            this->resolve_boundary(x + mwidth, y + mheight);

            this->dirty_canvas(0U, -1.0);
        }
    }
}

/*************************************************************************************************/
void GYDM::Tracklet::erase() {
    if (this->xmax != -infinity) {
        this->xmax = this->ymax = -infinity;
        this->xmin = this->ymin = +infinity;
        this->dirty_canvas(0U, 0.0);
    }
}

void GYDM::Tracklet::set_pen_width(uint8_t lwidth) {
    if (this->line_width != lwidth) {
        this->line_width = lwidth;
    }
}

void GYDM::Tracklet::resolve_boundary(float x, float y) {
    // don't merge with `else if`
    if (x < this->xmin) this->xmin = x;
    if (x > this->xmax) this->xmax = x;
    if (y < this->ymin) this->ymin = y;
    if (y > this->ymax) this->ymax = y;
}
