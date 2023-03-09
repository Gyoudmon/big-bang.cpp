#pragma once

#include "../graphlet.hpp"
#include "../../graphics/colorspace.hpp"

namespace WarGrey::STEM {
    class IShapelet : public WarGrey::STEM::IGraphlet {
    public:
        IShapelet(int32_t color = -1, int32_t border_color = -1);
        virtual ~IShapelet() { this->invalidate_geometry(); }

    public:
        void draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) override;

    public:
        void set_color_mixture(WarGrey::STEM::ColorMixture mixture);
        void set_alpha(unsigned char alpha);
        void set_alpha(float alpha);
        unsigned char get_alpha() { return this->alpha; }

        void set_color(int32_t color);
        int32_t get_color() { return this->color; }

        void set_border_color(int32_t color);
        int32_t get_border_color() { return this->border_color; }
 
    protected:
        virtual void on_moved(float new_x, float new_y) {}
        virtual void draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;
        virtual void fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;

    protected:
        void invalidate_geometry();

    private:
        ColorMixture mixture = ColorMixture::Alpha;
        int32_t color = -1;
        int32_t border_color = -1;
        unsigned char alpha = 0xFFU;

    private:
        SDL_Texture* geometry;
    };

    /**********************************************************************************************/
    class Linelet : public WarGrey::STEM::IShapelet {
    public:
	    Linelet(float ex, float ey, int32_t color);

	public:
        void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
        
    protected:
        void on_resize(float new_width, float new_height, float old_width, float old_height) override;
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
        void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
	    
    protected:
        void on_resize(float new_width, float new_height, float old_width, float old_height) override;
        void draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;
        void fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

	private:
	    float width;
	    float height;
    };

    class Squarelet : public WarGrey::STEM::Rectanglet {
    public:
        Squarelet(float edge_size, int32_t color, int32_t border_color = -1)
            : Rectanglet(edge_size, color, border_color) {}
    };

    class RoundedRectanglet : public WarGrey::STEM::IShapelet {
    public:
	    RoundedRectanglet(float edge_size, float radius, int32_t color, int32_t border_color = -1);
	    RoundedRectanglet(float width, float height, float radius, int32_t color, int32_t border_color = -1);

	public:
	    void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
	    
    protected:
        void on_resize(float new_width, float new_height, float old_width, float old_height) override;
        void draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;
        void fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

	private:
	    float width;
	    float height;
        float radius;
    };

    class RoundedSquarelet : public WarGrey::STEM::RoundedRectanglet {
    public:
        RoundedSquarelet(float edge_size, float radius, int32_t color, int32_t border_color = -1)
            : RoundedRectanglet(edge_size, edge_size, radius, color, border_color) {}
    };

    class Ellipselet : public WarGrey::STEM::IShapelet {
    public:
	    Ellipselet(float radius, int32_t color, int32_t border_color = -1);
	    Ellipselet(float aradius, float bradius, int32_t color, int32_t border_color = -1);

	public:
	    void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
	    
    protected:
        void on_resize(float new_width, float new_height, float old_width, float old_height) override;
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
    
    class Trianglet : public WarGrey::STEM::IShapelet {
    public:
	    Trianglet(float x2, float y2, float x3, float y3, int32_t color, int32_t border_color = -1);

	public:
	    void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
	    
    protected:
        void on_resize(float new_width, float new_height, float old_width, float old_height) override;
        void draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;
        void fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

    private:
        float x2;
        float y2;
        float x3;
        float y3;
    };

    class RegularPolygonlet : public WarGrey::STEM::IShapelet {
    public:
	    RegularPolygonlet(int n, float radius, int32_t color, int32_t border_color = -1);
	    RegularPolygonlet(int n, float radius, float rotation, int32_t color, int32_t border_color = -1);
        virtual ~RegularPolygonlet();

	public:
	    void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
	    
    protected:
        void on_resize(float new_width, float new_height, float old_width, float old_height) override;
        void draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;
        void fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

    private:
        void initialize_vertice();

	private:
        int n;
	    float aradius;
        float bradius;
        float rotation;
        float* xs = nullptr;
        float* ys = nullptr;
        short* txs = nullptr;
        short* tys = nullptr;

    private:
        float lx;
        float ty;
        float rx;
        float by;
    };
}
