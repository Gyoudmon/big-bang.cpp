#include "sprite.hpp"

#include "../datum/box.hpp"
#include "../datum/flonum.hpp"
#include "../datum/string.hpp"

#include "../graphics/geometry.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::ISprite::ISprite() {
    this->_sprite = this;
    this->current_custome_idx = 0;
    this->set_fps();
}

void WarGrey::STEM::ISprite::feed_extent(float x, float y, float* width, float* height) {
    float owidth, oheight;

    this->feed_original_extent(x, y, &owidth, &oheight);
    SET_BOX(width, owidth * flabs(this->xscale));
    SET_BOX(height, oheight * flabs(this->yscale));
}

void WarGrey::STEM::ISprite::feed_original_extent(float x, float y, float* width, float* height) {
    if (this->current_custome_idx >= this->custome_count()) {
        SET_BOXES(width, height, 0.0F);
    } else {
        this->feed_custome_extent(this->current_custome_idx, width, height);
    }
}

void WarGrey::STEM::ISprite::on_resize(float width, float height, float old_width, float old_height) {
    if (this->current_custome_idx < this->custome_count()) {
        float cwidth, cheight;

        this->feed_custome_extent(this->current_custome_idx, &cwidth, &cheight);

        if ((cwidth > 0.0F) && (cheight > 0.0F)) {
            this->xscale = width  / cwidth;
            this->yscale = height / cheight;
        }
    }
}

void WarGrey::STEM::ISprite::draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) {
    if (this->current_custome_idx < this->custome_count()) {
        this->draw_custome(renderer, this->current_custome_idx, x, y, Width, Height);
    }
}

void WarGrey::STEM::ISprite::switch_to_custome(int idx) {
    size_t maxsize = this->custome_count();

    if (maxsize > 0) {
        int actual_idx = idx;
        
        if (actual_idx >= maxsize) {
            actual_idx %= maxsize;
        } else if (actual_idx < 0) {
            actual_idx = maxsize - (-actual_idx % maxsize);
        }

        if (actual_idx != this->current_custome_idx) {
            this->current_custome_idx = actual_idx;
            this->notify_updated();
        }
    }
}

void WarGrey::STEM::ISprite::switch_to_custome(const char* name) {
    int cidx = this->custome_name_to_index(name);

    if (cidx >= 0) {
        this->switch_to_custome(cidx);
    }
}

void WarGrey::STEM::ISprite::update(uint32_t count, uint32_t interval, uint32_t uptime) {
    size_t frame_size = this->frame_refs.size();

    if (frame_size > 1) {
        if (interval * this->animation_subframe_count <= this->animation_interval) {
            this->animation_subframe_count ++;
        } else {
            this->animation_subframe_count = 1;

            if (this->current_subframe_idx >= frame_size) {
                if (this->animation_rest != 0) {
                    this->switch_to_custome(this->frame_refs[0]);
                    this->current_subframe_idx = 1;
                    if (this->animation_rest > 0) {
                        this->animation_rest --;
                    }
                }
            } else {
                this->switch_to_custome(this->frame_refs[this->current_subframe_idx]);
                this->current_subframe_idx ++;
            }
        }
    }
}

void WarGrey::STEM::ISprite::set_fps(int fps) {
    this->animation_interval = 1000 / fps;
    this->animation_subframe_count = 1;
}

size_t WarGrey::STEM::ISprite::play(const char* action, int repetition) {
    this->animation_rest = repetition;
    this->animation_subframe_count = 1;
    this->frame_refs.clear();
    this->current_subframe_idx = 0;
    
    for (int i = 0; i < this->custome_count(); i++) {
        if (string_prefix(this->custome_index_to_name(i), action)) {
            this->frame_refs.push_back(i);
        }
    }

    if (this->frame_refs.size()) {
        this->switch_to_custome(this->frame_refs[0]);
        this->current_subframe_idx = 1;
    }

    return this->frame_refs.size();
}

void WarGrey::STEM::ISprite::stop() {
    this->animation_rest = 0;
    this->frame_refs.clear();
}

SDL_RendererFlip WarGrey::STEM::ISprite::current_flip_status() {
    return game_scales_to_flip(this->xscale, this->yscale);
}
