#include "conveyerlet.hpp"

#include "../../../datum/box.hpp"
#include "../../../datum/flonum.hpp"

#include "../../../graphics/image.hpp"
#include "../../../graphics/geometry.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::ConveyerBeltlet::ConveyerBeltlet(float radius, float blength, double hue)
        : radius(radius), belt_length(blength), hue(flnan) {
    this->set_color_hue(hue);
    this->enable_resize(true);
}

void WarGrey::STEM::ConveyerBeltlet::feed_extent(float x, float y, float* w, float* h) {
    float diameter = this->radius * 2.0F;

    SET_VALUES(w, this->belt_length + diameter, h, diameter);
}

void WarGrey::STEM::ConveyerBeltlet::on_resize(float w, float h, float width, float height) {
    this->radius = flabs(h) * 0.5F;
    this->belt_length = w - h;
    this->invalidate_geometry();
}

void WarGrey::STEM::ConveyerBeltlet::draw(SDL_Renderer* renderer, float flx, float fly, float flwidth, float flheight) {
    if (this->sketch.use_count() == 0) {
        float width, height;

        this->feed_extent(0.0F, 0.0F, &width, &height);
        this->sketch = std::make_shared<Texture>(game_blank_image(renderer, fl2fxi(width) + 1, fl2fxi(height) + 1));
    }

    if (this->sketch->okay()) {
        if (this->needs_refresh_sketch) {
            SDL_Texture* origin = SDL_GetRenderTarget(renderer);
            
            
            
            SDL_SetRenderTarget(renderer, this->sketch->self());
            SDL_SetRenderTarget(renderer, origin);

            this->needs_refresh_sketch = false;
        }

        game_render_texture(renderer, this->sketch->self(), flx, fly, flwidth, flheight);
    } else {
        fprintf(stderr, "无法绘制传送带：%s\n", SDL_GetError());
    }
}

/*************************************************************************************************/
void WarGrey::STEM::ConveyerBeltlet::invalidate_geometry() {
    if (this->sketch.use_count() > 0) {
        this->sketch.reset();
        this->clear_geometry();
    }
}

void WarGrey::STEM::ConveyerBeltlet::clear_geometry() {
    this->needs_refresh_sketch = true;
}

void WarGrey::STEM::ConveyerBeltlet::set_color_hue(double hue) {
    if (this->hue != hue) {
        this->hue = hue;
        this->clear_geometry();
        this->notify_updated();
    }
}
