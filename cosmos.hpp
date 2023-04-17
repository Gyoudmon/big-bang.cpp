#pragma once

#include "universe.hpp"
#include "plane.hpp"
#include "virtualization/screen.hpp"

namespace WarGrey::STEM {
    class Cosmos : public WarGrey::STEM::IUniverse {
    public:
        Cosmos(uint32_t fps = 60, uint32_t fgc = 0x000000U, uint32_t bgc = 0xFFFFFFU);
        virtual ~Cosmos();

    public:
        void reflow(float window, float height) override;
        void update(uint64_t count, uint32_t interval, uint64_t uptime) override {};
        void draw(SDL_Renderer* renderer, int x, int y, int width, int height) override;
        bool has_current_mission_completed();
        bool can_exit() override;

    public:
        void transfer(int delta_idx);
        void transfer_to_plane(const char* name) { this->transfer_to_plane(std::string(name)); }
        void transfer_to_plane(const std::string& name);
        void transfer_to_plane(int idx);
        void transfer_to_next_plane() { this->transfer(1); }
        void transfer_to_prev_plane() { this->transfer(-1); }

    public:
        size_t plane_count() { return this->chunk_count; }
        const char* plane_name(int idx);
        int plane_index(const std::string& name);
        int plane_index(const char* name) { return this->plane_index(std::string(name)); }
        
    protected: // 常规事件处理和分派函数
        void on_mouse_event(SDL_MouseButtonEvent& mouse, bool pressed) override; 
        void on_mouse_move(uint32_t state, int x, int y, int dx, int dy) override;
        void on_scroll(int horizon, int vertical, float hprecise, float vprecise) override;

        void on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) override;
        void on_text(const char* text, size_t size, bool entire) override;
        void on_editing_text(const char* text, int pos, int span) override;

        void on_save(const std::string& full_path, std::ofstream& dev_stdout) override;

    protected:
        void on_big_bang(int width, int height) override;
        void on_game_start() override;
        void on_elapse(uint64_t count, uint32_t interval, uint64_t uptime) override;
        const char* usrdata_extension() override;

    protected:
        WarGrey::STEM::IPlane* push_plane(WarGrey::STEM::IPlane* plane);

    private:
        void collapse();
        void notify_transfer(WarGrey::STEM::IPlane* from, WarGrey::STEM::IPlane* to);
        void on_navigate(int from_idx, int to_idx);

    private:
        WarGrey::STEM::IScreen* screen = nullptr;
        WarGrey::STEM::IPlane* head_plane = nullptr;
        WarGrey::STEM::IPlane* recent_plane = nullptr;
        size_t chunk_count = 0;
        int recent_plane_idx = 0;

    private:
        WarGrey::STEM::IPlane* from_plane = nullptr;
    };
}
