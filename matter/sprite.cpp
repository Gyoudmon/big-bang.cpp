#include "sprite.hpp"

#include "../datum/box.hpp"
#include "../datum/flonum.hpp"
#include "../datum/string.hpp"

#include "../graphics/geometry.hpp"

#include "../physics/random.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::ISprite::ISprite() {
    this->_sprite = this;
    this->current_costume_idx = 0;
}

void WarGrey::STEM::ISprite::feed_extent(float x, float y, float* width, float* height) {
    float owidth, oheight;

    this->feed_original_extent(x, y, &owidth, &oheight);
    SET_BOX(width, owidth * flabs(this->xscale));
    SET_BOX(height, oheight * flabs(this->yscale));
}

void WarGrey::STEM::ISprite::feed_original_extent(float x, float y, float* width, float* height) {
    if (this->current_costume_idx >= this->costume_count()) {
        SET_BOXES(width, height, 0.0F);
    } else {
        this->feed_costume_extent(this->current_costume_idx, width, height);
    }
}

void WarGrey::STEM::ISprite::on_resize(float width, float height, float old_width, float old_height) {
    if (this->current_costume_idx < this->costume_count()) {
        float cwidth, cheight;

        this->feed_costume_extent(this->current_costume_idx, &cwidth, &cheight);

        if ((cwidth > 0.0F) && (cheight > 0.0F)) {
            this->xscale = width  / cwidth;
            this->yscale = height / cheight;
        }
    }
}

void WarGrey::STEM::ISprite::draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) {
    if (this->current_costume_idx < this->costume_count()) {
        this->draw_costume(renderer, this->current_costume_idx, x, y, Width, Height);
    }
}

void WarGrey::STEM::ISprite::switch_to_costume(int idx) {
    /** WARNING
     * `size_t` will implicitly convert the `actual_idx` into a nonnegative integer,
     *      and cause it always be true for `actual_idx >= maxsize`.
     **/

    long maxsize = this->costume_count();

    if (maxsize > 0) {
        int actual_idx = idx;
        
        if (actual_idx >= maxsize) {
            actual_idx %= maxsize;
        } else if (actual_idx < 0) {
            actual_idx = maxsize - ((-actual_idx) % maxsize);
        }

        if (actual_idx != this->current_costume_idx) {
            this->current_costume_idx = actual_idx;
            this->notify_updated();
        }
    }
}

void WarGrey::STEM::ISprite::switch_to_costume(const char* name) {
    int cidx = this->costume_name_to_index(name);

    if (cidx >= 0) {
        this->switch_to_costume(cidx);
    }
}

void WarGrey::STEM::ISprite::switch_to_random_costume(int idx0, int idxn) {
    this->switch_to_costume(random_uniform(idx0, idxn));
}

void WarGrey::STEM::ISprite::update(uint32_t count, uint32_t interval, uint32_t uptime) {
    size_t frame_size = this->frame_refs.size();

    if (frame_size > 1) {
        uint32_t frame_idx = count % frame_size;
        
        if (frame_idx >= frame_size) {
            if (this->animation_rest != 0) {
                this->switch_to_costume(this->frame_refs[0]);
                if (this->animation_rest > 0) {
                    this->animation_rest --;
                }
            }
        } else {
            this->switch_to_costume(this->frame_refs[frame_idx]);
        }
    }
}

size_t WarGrey::STEM::ISprite::play(const char* action0, int repetition) {
    const char* action = (action0 == nullptr) ? "" : action0;
    this->animation_rest = repetition;
    this->frame_refs.clear();
    
    for (int i = 0; i < this->costume_count(); i++) {
        if (string_prefix(this->costume_index_to_name(i), action)) {
            this->frame_refs.push_back(i);
        }
    }

    if (this->frame_refs.size() > 0) {
        this->switch_to_costume(this->frame_refs[0]);
        this->notify_timeline_restart(1);
    }

    return this->frame_refs.size();
}

size_t WarGrey::STEM::ISprite::play(int idx0, size_t count, int repeition) {
    size_t size = this->costume_count();

    this->animation_rest = repeition;
    this->frame_refs.clear();

    if (count >= size) {
        count = count % size + size;
    }

    for (int off = 0; off < count; off ++) {
        this->frame_refs.push_back(idx0 + off);
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
