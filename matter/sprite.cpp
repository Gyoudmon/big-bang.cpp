#include "sprite.hpp"

#include "../plane.hpp"

#include "../datum/box.hpp"
#include "../datum/fixnum.hpp"
#include "../datum/flonum.hpp"
#include "../datum/string.hpp"

#include "../graphics/misc.hpp"

#include "../physics/random.hpp"
#include "../physics/mathematics.hpp"

using namespace GYDM;

/*************************************************************************************************/
void GYDM::ISprite::construct(GYDM::dc_t* renderer) {
    int idx = this->get_initial_costume_index();

    if (idx >= 0) {
        this->switch_to_costume(idx);
    } else {
        this->play_all();
    }
}

GYDM::Box GYDM::ISprite::get_original_bounding_box() {
    float width = 0.0F;
    float height = 0.0F;

    if (this->current_costume_idx < this->costume_count()) {
        if ((this->canvas_width > 0.0F) && (this->canvas_height > 0.0F)) {
            width = this->canvas_width;
            height = this->canvas_height;
        } else {
            this->feed_costume_extent(this->current_costume_idx, &width, &height);
        
            if (this->canvas_width > 0.0F) {
                width = this->canvas_width;
            }

            if (this->canvas_height > 0.0F) {
                height = this->canvas_height;
            }
        }
    }

    return Box(width, height);
}

Box GYDM::ISprite::get_bounding_box() {
    return Box(this->get_original_bounding_box(),
                flabs(this->xscale), flabs(this->yscale));
}

Margin GYDM::ISprite::get_margin() {
    return Margin(this->get_original_margin(),
                flabs(this->xscale), flabs(this->yscale));
}

void GYDM::ISprite::on_resize(float width, float height, float old_width, float old_height) {
    if (this->current_costume_idx < this->costume_count()) {
        float cwidth, cheight;

        this->feed_costume_extent(this->current_costume_idx, &cwidth, &cheight);

        if ((cwidth > 0.0F) && (cheight > 0.0F)) {
            this->xscale = width  / cwidth;
            this->yscale = height / cheight;
        }
    }
}

void GYDM::ISprite::draw(GYDM::dc_t* dc, float x, float y, float Width, float Height) {
    if (this->current_costume_idx < this->costume_count()) {
        SpriteRenderArguments argv;
        
        argv.dst = { x, y, Width, Height };
        argv.flip = this->current_flip_status();

        if ((this->canvas_width <= 0.0F) && (this->canvas_height <= 0.0F)) {
            this->draw_costume(dc, this->current_costume_idx, nullptr, &argv);
        } else {
            float width, height, xoff, yoff;
            float sx = flabs(this->xscale);
            float sy = flabs(this->yscale);
            float cwidth = this->canvas_width;
            float cheight = this->canvas_height;

            this->feed_costume_extent(this->current_costume_idx, &width, &height);

            if (cwidth <= 0.0F) {
                cwidth = width;
            }

            if (cheight <= 0.0F) {
                cheight = height;
            }
        
            xoff = (cwidth - width) * 0.5F * sx;
            yoff = (cheight - height) * 0.5F * sy;

            if (xoff > 0.0F) {
                argv.dst.x += xoff;
                argv.dst.w -= xoff * 2.0F;
            }

            if (yoff > 0.0F) {
                argv.dst.y += yoff;
                argv.dst.h -= yoff * 2.0F;
            }

            if ((xoff >= 0.0F) && (yoff >= 0.0F)) {
                this->draw_costume(dc, this->current_costume_idx, nullptr, &argv);
            } else {
                SDL_Rect src = { 0, 0, fl2fxi(width), fl2fxi(height) };
            
                if (xoff < 0.0F) {
                    src.x = fl2fxi(-xoff / sx);
                    src.w -= src.x * 2;
                }

                if (yoff < 0.0F) {
                    src.y = fl2fxi(-yoff / sy);
                    src.h -= src.y * 2;
                }

                this->draw_costume(dc, this->current_costume_idx, &src, &argv);
            }
        }
    }
}

void GYDM::ISprite::set_virtual_canvas(float width, float height) {
    if ((this->canvas_width != width) || (this->canvas_height != height)) {
        this->canvas_width = width;
        this->canvas_height = height;
        this->notify_updated();
    }
}

void GYDM::ISprite::auto_virtual_canvas(const char* action_name) {
    float cwidth = 0.0F;
    float cheight = 0.0F;
    float cw, ch;

    for (size_t idx = 0; idx < this->costume_count(); idx ++) {
        if ((action_name == nullptr) || string_ci_prefix(this->costume_index_to_name(idx), action_name)) {
            this->feed_costume_extent(idx, &cw, &ch);

            if (cw > cwidth) {
                cwidth = cw;
            }

            if (ch > cheight) {
                cheight = ch;
            }
        }
    }

    this->set_virtual_canvas(cwidth, cheight);
}

void GYDM::ISprite::switch_to_costume(int idx) {
    /** WARNING
     * `size_t` will implicitly convert the `actual_idx` into a nonnegative integer,
     *      and cause it always be true for `actual_idx >= maxsize`.
     **/

    long maxsize = long(this->costume_count());

    if (maxsize > 0) {
        long actual_idx = safe_index(long(idx), maxsize);
        
        if (actual_idx != this->current_costume_idx) {
            this->current_costume_idx = size_t(actual_idx);
            this->notify_updated();
        }
    }
}

void GYDM::ISprite::switch_to_costume(const char* name) {
    int cidx = this->costume_name_to_index(name);

    if (cidx >= 0) {
        this->switch_to_costume(cidx);
    }
}

void GYDM::ISprite::switch_to_random_costume(int idx0, int idxn) {
    this->switch_to_costume(random_uniform(idx0, idxn));
}


int GYDM::ISprite::costume_name_to_index(const char* name) {
    int cidx = -1;
    
    for (size_t idx = 0; idx < this->costume_count(); idx ++) {
        if (string_ci_equal(this->costume_index_to_name(idx), name)) {
            cidx = int(idx);
            break;
        }
    }

    if (cidx < 0) {
        for (int idx = 0; idx < this->costume_count(); idx ++) {
            if (string_ci_prefix(this->costume_index_to_name(idx), name)) {
                cidx = idx;
                break;
            }
        }
    }

    return cidx;
}

int GYDM::ISprite::update(uint64_t count, uint32_t interval, uint64_t uptime) {
    size_t frame_size = this->frame_refs.size();
    int duration = 0;

    if (frame_size > 0) {
        if (this->animation_rest != 0) {
            uint64_t frame_idx = count % frame_size;
        
            if (frame_idx == 0) {
                if (this->next_branch >= 0) {
                    this->next_branch = this->update_action_frames(this->frame_refs, this->next_branch);
                    if (this->frame_refs.size() > 0) {
                        this->notify_timeline_restart(1);
                    }
                } else {
                    if (this->animation_rest > 0) {
                        this->animation_rest -= 1;
                    }

                    if (this->animation_rest == 0) {
                        this->idle_time0 = uptime;
                        this->stop();
                    } else {
                        this->next_branch = this->submit_action_frames(this->frame_refs, this->current_action_name);
                        if (this->frame_refs.size() > 0) {
                            this->notify_timeline_restart(1);
                        }
                    }
                }

                if (this->frame_refs.size() > 0) {
                    this->switch_to_costume(this->frame_refs[0].first);
                    duration = this->frame_refs[0].second;
                }
            } else {
                this->switch_to_costume(this->frame_refs[frame_idx].first);
                duration = this->frame_refs[frame_idx].second;
            }
        } else {
            this->idle_time0 = uptime;
            this->stop();
        }
    } else if (frame_size == 0) {
        uint64_t idle_interval = this->preferred_idle_duration();

        if (idle_interval > 0U) {
            if (this->idle_time0 == 0) {
                this->idle_time0 = uptime;
            }

            if (idle_interval <= (uptime - this->idle_time0)) {
                int times = 1;
                
                this->next_branch = this->submit_idle_frames(this->frame_refs, times);

                if (this->frame_refs.size() > 0) {
                    this->switch_to_costume(this->frame_refs[0].first);
                    duration = this->frame_refs[0].second;
                    this->animation_rest = ((times < 1) ? 1 : times);
                    this->notify_timeline_restart(1);
                }
            }
        }
    }

    return duration;
}

size_t GYDM::ISprite::play(const char* action, int repetition) {
    this->current_action_name.clear();
    this->current_action_name.append((action == nullptr) ? "" : action);

    this->animation_rest = repetition;
    this->frame_refs.clear();
    this->next_branch = this->submit_action_frames(this->frame_refs, this->current_action_name);
    
    if (this->frame_refs.size() > 0) {
        this->switch_to_costume(this->frame_refs[0].first);
        this->notify_timeline_restart(1, this->frame_refs[0].second);
    }

    return this->frame_refs.size();
}

size_t GYDM::ISprite::play(int idx0, size_t count, int repetition) {
    size_t size = this->costume_count();

    this->current_action_name.clear();
    this->animation_rest = repetition;
    this->frame_refs.clear();
    this->next_branch = -1;

    if (count >= size) {
        count = count % size + size;
    }

    for (int off = 0; off < count; off ++) {
        this->frame_refs.push_back({ idx0 + off, 0 });
    }

    return this->frame_refs.size();
}

void GYDM::ISprite::stop(int rest) {
    this->animation_rest = (rest <= 0) ? 0 : rest;

    if (this->animation_rest == 0) {
        this->current_action_name.clear();
        this->frame_refs.clear();
    }
}

int GYDM::ISprite::submit_action_frames(std::vector<std::pair<int, int>>& frame_refs, const std::string& action) {
    for (int i = 0; i < this->costume_count(); i++) {
        if (string_ci_prefix(this->costume_index_to_name(i), action)) {
            frame_refs.push_back({ i, 0 });
        }
    }

    return -1;
}

int GYDM::ISprite::submit_idle_frames(std::vector<std::pair<int, int>>& frame_refs, int& times) {
    return ISprite::submit_action_frames(frame_refs, "idle");
}

uint64_t GYDM::ISprite::preferred_idle_duration() {
    return static_cast<uint64_t>(random_uniform(2000, 4000));
}

void GYDM::ISprite::flip(bool horizontal, bool vertical) {
    if (horizontal || vertical) {
        if (horizontal) {
            this->xscale *= -1.0F;
        }

        if (vertical) {
            this->yscale *= -1.0F;
        }

        this->notify_updated();
    }
}

SDL_RendererFlip GYDM::ISprite::current_flip_status() {
    return game_scales_to_flip(this->xscale, this->yscale);
}

float GYDM::ISprite::get_horizontal_scale() {
    return flabs(this->xscale);
}

float GYDM::ISprite::get_vertical_scale() {
    return flabs(this->yscale);
}

/**************************************************************************************************/
void GYDM::ISprite::shh() {
    if (this->info != nullptr) {
        this->info->master->shh(this);
    }
}

void GYDM::ISprite::say(IMatter* message, double sec, SpeechBubble type) {
    if (this->info != nullptr) {
        this->info->master->say(this, sec, message, type);
    }
}

void GYDM::ISprite::say(const std::string& sentence, const RGBA& color) {
    if (this->info != nullptr) {
        this->info->master->say(this, sentence, color);
    }
}

void GYDM::ISprite::say(double sec, const std::string& sentence, const RGBA& color) {
    if (this->info != nullptr) {
        this->info->master->say(this, sec, sentence, color);
    }
}

void GYDM::ISprite::think(const std::string& sentence, const RGBA& color) {
    if (this->info != nullptr) {
        this->info->master->think(this, sentence, color);
    }
}

void GYDM::ISprite::think(double sec, const std::string& sentence, const RGBA& color) {
    if (this->info != nullptr) {
        this->info->master->think(this, sentence, color);
    }
}

bool GYDM::ISprite::is_speaking() {
    bool yes = false;

    if (this->info != nullptr) {
        yes = this->info->master->is_speaking(this);
    }

    return yes;
}

bool GYDM::ISprite::is_thinking() {
    bool yes = false;

    if (this->info != nullptr) {
        yes = this->info->master->is_thinking(this);
    }

    return yes;
}

bool GYDM::ISprite::in_speech() {
    bool yes = false;
    
    if (this->info != nullptr) {
        yes = this->info->master->in_speech(this);
    }

    return yes;
}
