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
}

void WarGrey::STEM::ISprite::post_construct(SDL_Renderer* renderer) {
    this->switch_to_costume(this->get_initial_costume_index());
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

int WarGrey::STEM::ISprite::update(uint32_t count, uint32_t interval, uint32_t uptime) {
    size_t frame_size = this->frame_refs.size();
    int duration = 0;

    if (frame_size > 1) {
        if (this->animation_rest != 0) {
            uint32_t frame_idx = count % frame_size;
        
            if (frame_idx == 0) {
                if (this->animation_rest > 0) {
                    this->animation_rest -= 1;
                }

                if ((this->animation_rest != 0) && !this->current_action_name.empty()) {
                    int start_idx = this->update_action_frames(this->frame_refs, this->current_action_name.c_str());

                    if ((start_idx >= 0) && (start_idx < this->frame_refs.size())) {
                        this->switch_to_costume(this->frame_refs[start_idx].first);
                        duration = this->frame_refs[start_idx].second;
                    }
                }
            } else {
                this->switch_to_costume(this->frame_refs[frame_idx].first);
                duration = this->frame_refs[frame_idx].second;
            }
        }
    }

    return duration;
}

size_t WarGrey::STEM::ISprite::play(const std::string& action, int repetition) {
    int start_idx = 0;
    
    this->current_action_name = action;
    this->animation_rest = repetition;
    this->frame_refs.clear();
    
    start_idx = this->submit_action_frames(this->frame_refs, action);
    if ((start_idx >= 0) && (start_idx < this->frame_refs.size())) {
        this->switch_to_costume(this->frame_refs[start_idx].first);
        this->notify_timeline_restart(1, this->frame_refs[start_idx].second);
    }

    return this->frame_refs.size();
}

size_t WarGrey::STEM::ISprite::play(int idx0, size_t count, int repetition) {
    size_t size = this->costume_count();

    this->current_action_name.clear();
    this->animation_rest = repetition;
    this->frame_refs.clear();

    if (count >= size) {
        count = count % size + size;
    }

    for (int off = 0; off < count; off ++) {
        this->frame_refs.push_back({ idx0 + off, 0 });
    }

    return this->frame_refs.size();
}

void WarGrey::STEM::ISprite::stop() {
    this->animation_rest = 0;
    this->frame_refs.clear();
    this->current_action_name.clear();
}

SDL_RendererFlip WarGrey::STEM::ISprite::current_flip_status() {
    return game_scales_to_flip(this->xscale, this->yscale);
}

int WarGrey::STEM::ISprite::submit_action_frames(std::vector<std::pair<int, int>>& frame_refs, const std::string& action) {
    for (int i = 0; i < this->costume_count(); i++) {
        if (string_prefix(this->costume_index_to_name(i), action)) {
            frame_refs.push_back({ i, 0 });
        }
    }

    return 0;
}
