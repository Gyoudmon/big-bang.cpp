#pragma once

#include "../matter.hpp"
#include "../graphics/image.hpp"

#include "movable.hpp"

#include <vector>
#include <string>

namespace WarGrey::STEM {
    class ISprite : public WarGrey::STEM::IMatter, public WarGrey::STEM::IMovable {
    public:
        ISprite();
        virtual ~ISprite() {}

    public:
        void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
        void update(uint32_t count, uint32_t interval, uint32_t uptime) override;
        void draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) override;

    public:
        virtual size_t custome_count() = 0;
        
    public:
        void switch_to_custome(int idx);
        void switch_to_custome(const char* name);
        void switch_to_custome(const std::string& name) { this->switch_to_custome(name.c_str()); }
        void switch_to_prev_custome() { this->switch_to_custome(this->current_custome_idx - 1); }
        void switch_to_next_custome() { this->switch_to_custome(this->current_custome_idx + 1); }

    public:
        void set_fps(int fps = 16);
        size_t play(const std::string& action, int repetition = -1) { return this->play(action.c_str(), repetition); }
        size_t play(const char* action, int repetition = -1);
        bool in_playing() { return this->animation_rest != 0; }
        void stop();

    protected:
        virtual void feed_custome_extent(int idx, float* width, float* height) = 0;
        virtual bool is_key_frame(int idx, const char* action) = 0;
        virtual int custome_name_to_index(const char* name) = 0;
        virtual void draw_custome(SDL_Renderer* renderer, int idx, float x, float y, float Width, float Height) = 0;

    protected:
        void on_resize(float width, float height, float old_width, float old_height) override;

    protected:
        SDL_RendererFlip current_flip_status();

    private:
        int current_custome_idx = 0;
        float xscale = 1.0F;
        float yscale = 1.0F;

    private:
        int animation_interval = 0;
        int animation_subframe_count = 1;
        int animation_rest = 0;
        std::vector<int> frame_refs;
        int current_subframe_idx;
    };
}
