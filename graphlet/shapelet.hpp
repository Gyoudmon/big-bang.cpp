#pragma once

#include "graphlet.hpp"
#include "../matter/movable.hpp"

namespace WarGrey::STEM {
    class IShapelet : public WarGrey::STEM::IGraphlet, public WarGrey::STEM::IMovable {
        public:
            IShapelet(int32_t color = -1, int32_t border_color = -1);
            virtual ~IShapelet();

        public:
            void construct() override;
            void fill_extent(float x, float y, float* w, float* h) override;
            void draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) override;

        public:
            void set_color(int32_t color);
            int32_t get_color() { return this->color; }

            void set_border_color(int32_t color);
            int32_t get_border_color() { return this->border_color; }

            void set_alpha_key_color(uint32_t color);
 
        public:
            virtual void fill_shape_origin(float* x, float* y);

        protected:
            void on_shape_changed(SDL_Surface* g);

        protected:
            virtual void fill_shape_extent(float* width, float* height) = 0;
            virtual void draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;
            virtual void fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;

        private:
            SDL_Surface* geometry = nullptr;
            int32_t color = -1;
            int32_t border_color = -1;
            uint32_t alpha_color_key = 0xFFFFFFU;
    };

    /**********************************************************************************************/
    class Linelet : public WarGrey::STEM::IShapelet {
        public:
	        Linelet(float ex, float ey, int32_t color);

	    public:
	        void resize(float width, float height) override;

        protected:
            void fill_shape_extent(float* width, float* height) override;
            void draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override {}
            void fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

        private:
            float epx;
            float epy;
    };

    class HLinelet : public WarGrey::STEM::Linelet {
        public:
            HLinelet(float width, int32_t color) : Linelet(width, 0.0F, color) {}
    };

    class VLinelet : public WarGrey::STEM::Linelet {
        public:
            VLinelet(float height, int32_t color) : Linelet(0.0F, height, color) {}
    };

    /**********************************************************************************************/
    class Rectanglet : public WarGrey::STEM::IShapelet {
        public:
	        Rectanglet(float edge_size, int32_t color, int32_t border_color = -1);
	        Rectanglet(float width, float height, int32_t color, int32_t border_color = -1);

	    public:
	        void resize(float width, float height) override;

        protected:
            void fill_shape_extent(float* width, float* height) override;
            void draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;
            void fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

	    private:
	        float width;
	        float height;
    };

    class RoundedRectanglet : public WarGrey::STEM::IShapelet {
        public:
	        RoundedRectanglet(float edge_size, float radius, int32_t color, int32_t border_color = -1);
	        RoundedRectanglet(float width, float height, float radius, int32_t color, int32_t border_color = -1);

	    public:
	        void resize(float width, float height) override;

        protected:
            void fill_shape_extent(float* width, float* height) override;
            void draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;
            void fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

	    private:
	        float width;
	        float height;
            float radius;
    };

    class Ellipselet : public WarGrey::STEM::IShapelet {
        public:
	        Ellipselet(float radius, int32_t color, int32_t border_color = -1);
	        Ellipselet(float aradius, float bradius, int32_t color, int32_t border_color = -1);

	    public:
	        void resize(float width, float height) override;

        protected:
            void fill_shape_extent(float* width, float* height) override;
            void draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;
            void fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

	    private:
	        float aradius;
	        float bradius;
    };
    
    class Circlet : public WarGrey::STEM::Ellipselet {
        public:
	        Circlet(float radius, int32_t color, int32_t border_color = -1)
                : Ellipselet(radius, radius, color, border_color) {}
    };
    
    class RegularPolygonlet : public WarGrey::STEM::IShapelet {
        public:
	        RegularPolygonlet(int n, float radius, int32_t color, int32_t border_color = -1);
	        RegularPolygonlet(int n, float radius, float rotation, int32_t color, int32_t border_color = -1);
            virtual ~RegularPolygonlet();

	    public:
	        void resize(float width, float height) override;

        protected:
            void fill_shape_extent(float* width, float* height) override;
            void draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;
            void fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

        private:
            void initialize_vertice();

	    private:
            int n;
	        float aradius;
            float bradius;
            float rotation;
            short* xs = nullptr;
            short* ys = nullptr;
    };
}

