#pragma once

#include "cosmos.hpp"
#include "plane.hpp"

namespace WarGrey::STEM {
    class World : public WarGrey::STEM::Cosmos {
    public:
        World(const char* title,
                int fps = 60, uint32_t fgc = 0x000000U, uint32_t bgc = 0xFFFFFFU,
                unsigned int initial_mode = 0U);

    public:
        void shift_to_mode(unsigned int mode) { this->plane->shift_to_mode(mode); }
        unsigned int current_mode() { return this->plane->current_mode(); }
        bool matter_unmasked(WarGrey::STEM::IMatter* m) { return this->plane->matter_unmasked(m); }

    public:
        void construct(int argc, char* argv[]) override {}
        void reflow(float Width, float Height) override {}

    public:
        virtual void load(float Width, float Height) {}

    public:
        virtual bool can_interactive_move(IMatter* m, float local_x, float local_y) { return false; }
        virtual bool can_select(IMatter* m) { return false; }
        virtual bool can_select_multiple() { return false; }
        virtual void before_select(IMatter* m, bool on_or_off) {}
        virtual void after_select(IMatter* m, bool on_or_off) {}

    public:
        bool feed_matter_location(IMatter* m, float* x, float* y, MatterAnchor a) {
            return this->plane->feed_matter_location(m, x, y, a);
        }

        void move(IMatter* m, float x, float y) {
            this->plane->move(m, x, y);
        }

        void move_to(IMatter* m, float x, float y, MatterAnchor a, float dx = 0.0F, float dy = 0.0F) {
            this->plane->move_to(m, x, y, a, dx, dy);
        }

        void move_to(IMatter* m, IMatter* tm, MatterAnchor ta, MatterAnchor a, float dx = 0.0F, float dy = 0.0F) {
            this->plane->move_to(m, tm, ta, a, dx, dy);
        }

        void move_to(IMatter* m, IMatter* tm, MatterAnchor ta, float fx = 0.0F, float fy = 0.0F, float dx = 0.0F, float dy = 0.0F) {
            this->plane->move_to(m, tm, ta, fx, fy, dx, dy);
        }

        void move_to(IMatter* m, IMatter* tm, float tfx, float tfy, MatterAnchor a, float dx = 0.0F, float dy = 0.0F) {
            this->plane->move_to(m, tm, tfx, tfy, a, dx, dy);
        }

        void move_to(IMatter* m, IMatter* xtm, float xfx, IMatter* ytm, float yfy, MatterAnchor a, float dx = 0.0F, float dy = 0.0F) {
            this->plane->move_to(m, xtm, xfx, ytm, yfy, a, dx, dy);
        }

        void remove(IMatter* m) {
            this->plane->remove(m);
        }

        void erase() {
            this->plane->erase();
        }
    
    public:
        void create_grid(int col, float x = 0.0F, float y = 0.0F, float width = 0.0F) {
            this->plane->create_grid(col, x, y, width);
        }

        void create_grid(int row, int col, float x = 0.0F, float y = 0.0F, float width = 0.0F, float height = 0.0F) {
            this->plane->create_grid(row, col, x, y, width, height);
        }

        void create_grid(float cell_width, float x = 0.0F, float y = 0.0F, int col = 0) {
            this->plane->create_grid(cell_width, x, y, col);
        }

        void create_grid(float cell_width, float cell_height, float x = 0.0F, float y = 0.0F, int row = 0, int col = 0) {
            this->plane->create_grid(cell_width, cell_height, x, y, row, col);
        }

        int grid_cell_index(float x, float y, int* r = nullptr, int* c = nullptr) {
            return this->plane->grid_cell_index(x, y, r, c);
        }
        
        int grid_cell_index(IMatter* m, int* r = nullptr, int* c = nullptr, MatterAnchor a = MatterAnchor::CC) {
            return this->plane->grid_cell_index(m, r, c, a);
        }        

        void feed_grid_cell_extent(float* width, float* height) {
            this->plane->feed_grid_cell_extent(width, height);
        }

        void feed_grid_cell_location(int idx, float* x, float* y, MatterAnchor a = MatterAnchor::CC) {
            this->plane->feed_grid_cell_location(idx, x, y, a);
        }

        void feed_grid_cell_location(int row, int col, float* x, float* y, MatterAnchor a = MatterAnchor::CC) {
            this->plane->feed_grid_cell_location(row, col, x, y, a);
        }

        void move_to_grid(IMatter* m, int idx, MatterAnchor a = MatterAnchor::CC, float dx = 0.0F, float dy = 0.0F) {
            this->plane->move_to_grid(m, idx, a, dx, dy);
        }

        void move_to_grid(IMatter* m, int row, int col, MatterAnchor a = MatterAnchor::CC, float dx = 0.0F, float dy = 0.0F) {
            this->plane->move_to_grid(m, row, col, a, dx, dy);
        }

        void set_grid_color(uint32_t color, float a = 1.0F) {
             this->plane->set_grid_color(color, a);
        }

    public:
        template<class M>
        M* insert(M* m, float x = 0.0F, float y = 0.0F, MatterAnchor a = MatterAnchor::LT, float dx = 0.0F, float dy = 0.0F) {
            return this->plane->insert(m, x, y, a, dx, dy);
        }
        
        template<class M>
        M* insert(M* m, int idx, MatterAnchor a = MatterAnchor::CC, float dx = 0.0F, float dy = 0.0F) {
            return this->plane->insert(m, idx, a, dx, dy);
        }

        template<class M>
        M* insert(M* m, int row, int col, MatterAnchor a = MatterAnchor::CC, float dx = 0.0F, float dy = 0.0F) {
            return this->plane->insert(m, row, col, a, dx, dy);
        }

    protected:
        WarGrey::STEM::Plane* plane;
    };
}
