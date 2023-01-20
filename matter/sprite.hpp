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
        void feed_original_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
        void update(uint32_t count, uint32_t interval, uint32_t uptime) override;
        void draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) override;

    public:
        virtual size_t costume_count() = 0;
        
    public:
        void switch_to_costume(int idx);
        void switch_to_costume(const char* name);
        void switch_to_costume(const std::string& name) { this->switch_to_costume(name.c_str()); }
        void switch_to_prev_costume() { this->switch_to_costume(this->current_costume_idx - 1); }
        void switch_to_next_costume() { this->switch_to_costume(this->current_costume_idx + 1); }
        int current_costume_index() { return this->current_costume_idx; }
        const char* current_costume_name() { return costume_index_to_name(this->current_costume_index()); };

    public:
        void set_fps(int fps = 16);
        size_t play(const std::string& action, int repetition = -1) { return this->play(action.c_str(), repetition); }
        size_t play(const char* action, int repetition = -1);
        bool in_playing() { return this->animation_rest != 0; }
        void stop();

    protected:
        virtual void feed_costume_extent(int idx, float* width, float* height) = 0;
        virtual const char* costume_index_to_name(int idx) = 0;
        virtual int costume_name_to_index(const char* name) = 0;
        virtual void draw_costume(SDL_Renderer* renderer, int idx, float x, float y, float Width, float Height) = 0;

    protected:
        void on_resize(float width, float height, float old_width, float old_height) override;

    protected:
        SDL_RendererFlip current_flip_status();

    private:
        int current_costume_idx = 0;
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
