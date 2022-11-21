#pragma once

#include "../sprite.hpp"

namespace WarGrey::STEM {
    class IShapemon : public virtual WarGrey::STEM::Sprite {
        public:
            IShapemon(int32_t color = -1, int32_t border_color = -1);
            virtual ~IShapemon();

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
    class Linemon : public WarGrey::STEM::IShapemon {
        public:
	        Linemon(float ex, float ey, int32_t color);

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

    class HLinemon : public WarGrey::STEM::Linemon {
        public:
            HLinemon(float width, int32_t color) : Linemon(width, 0.0F, color) {}
    };

    class VLinemon : public WarGrey::STEM::Linemon {
        public:
            VLinemon(float height, int32_t color) : Linemon(0.0F, height, color) {}
    };

    /**********************************************************************************************/
    class Rectanglemon : public WarGrey::STEM::IShapemon {
        public:
	        Rectanglemon(float edge_size, int32_t color, int32_t border_color = -1);
	        Rectanglemon(float width, float height, int32_t color, int32_t border_color = -1);

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

    class RoundedRectanglemon : public WarGrey::STEM::IShapemon {
        public:
	        RoundedRectanglemon(float edge_size, float radius, int32_t color, int32_t border_color = -1);
	        RoundedRectanglemon(float width, float height, float radius, int32_t color, int32_t border_color = -1);

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

    class Ellipsemon : public WarGrey::STEM::IShapemon {
        public:
	        Ellipsemon(float radius, int32_t color, int32_t border_color = -1);
	        Ellipsemon(float aradius, float bradius, int32_t color, int32_t border_color = -1);

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
    
    class Circlemon : public WarGrey::STEM::Ellipsemon {
        public:
	        Circlemon(float radius, int32_t color, int32_t border_color = -1)
                : Ellipsemon(radius, radius, color, border_color) {}
    };
    
    class RegularPolygonmon : public WarGrey::STEM::IShapemon {
        public:
	        RegularPolygonmon(int n, float radius, int32_t color, int32_t border_color = -1);
	        RegularPolygonmon(int n, float radius, float rotation, int32_t color, int32_t border_color = -1);
            virtual ~RegularPolygonmon();

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

