#pragma once

#include "../matter.hpp"
#include "../graphics/image.hpp"

#include <vector>
#include <string>

namespace WarGrey::STEM {
    struct SpriteRenderArguments {
        SDL_FRect dst;
        SDL_RendererFlip flip;
    };

    /*********************************************************************************************/
    class ISprite : public WarGrey::STEM::IMatter {
    public:
        ISprite() {}
        virtual ~ISprite() {}

        void construct(SDL_Renderer* renderer) override;

    public:
        void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
        void feed_original_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
        void feed_margin(float x, float y, float* top = nullptr, float* right = nullptr, float* bottom = nullptr, float* left = nullptr) override;
        int update(uint64_t count, uint32_t interval, uint64_t uptime) override;
        void draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) override;

    public:
        virtual size_t costume_count() = 0;
        
    public:
        void set_virtual_canvas(float width, float height);
        void switch_to_costume(int idx);
        void switch_to_costume(const char* name);
        void switch_to_costume(const std::string& name) { this->switch_to_costume(name.c_str()); }
        void switch_to_prev_costume() { this->switch_to_costume(int(this->current_costume_idx) - 1); }
        void switch_to_next_costume() { this->switch_to_costume(int(this->current_costume_idx) + 1); }
        void switch_to_random_costume(int idx0, int idxn);
        void switch_to_random_costume(int idx0 = 0) { this->switch_to_random_costume(idx0, int(this->costume_count()) - 1); }
        size_t current_costume_index() { return this->current_costume_idx; }
        const char* current_costume_name() { return this->costume_index_to_name(this->current_costume_index()); };
        void auto_virtual_canvas(const char* action_name = nullptr);
        void auto_virtual_canvas(const std::string& action_name) { this->auto_virtual_canvas(action_name.c_str()); }

    public:
        int preferred_local_fps() override { return 10; }
        size_t play(const std::string& action = "", int repetition = -1) { return this->play(action.c_str(), repetition); }
        size_t play(const char* action, int repetition = -1);
        size_t play(int idx0, size_t count, int repetition = -1);
        size_t play_all(int repetition = -1) { return this->play(0, this->costume_count(), repetition); }
        bool in_playing() { return this->animation_rest != 0; }
        void stop(int rest = 0);

    public:
        virtual void flip(bool horizontal = true, bool vertical = false);
        virtual void greetings(int repeat = 1) {} 
        virtual void goodbye(int repeat = 1) {}

    protected:
        virtual uint64_t preferred_idle_duration();
        virtual void feed_costume_extent(size_t idx, float* width, float* height) = 0;
        virtual const char* costume_index_to_name(size_t idx) = 0;
        virtual int costume_name_to_index(const char* name);
        virtual void draw_costume(SDL_Renderer* renderer, size_t idx, SDL_Rect* src, SpriteRenderArguments* argv) = 0;

    protected:
        virtual int get_initial_costume_index() { return 0; }
        virtual int submit_idle_frames(std::vector<std::pair<int, int>>& frame_refs, int& times);
        virtual int submit_action_frames(std::vector<std::pair<int, int>>& frame_refs, const std::string& action);
        virtual int update_action_frames(std::vector<std::pair<int, int>>& frame_refs, int next_branch) { return -1; }
        
    protected:
        void on_resize(float width, float height, float old_width, float old_height) override;

    protected:
        SDL_RendererFlip current_flip_status();
        float get_horizontal_scale();
        float get_vertical_scale();

    protected:
        std::string current_action_name;
        std::vector<std::pair<int, int>> frame_refs;
        int animation_rest = 0;
        int next_branch = -1;
        uint64_t idle_time0 = 0;

    private:
        size_t current_costume_idx = 0;
        float canvas_width = 0.0F;
        float canvas_height = 0.0F;
        float xscale = 1.0F;
        float yscale = 1.0F;
    };
}
