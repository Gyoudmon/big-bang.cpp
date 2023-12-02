#pragma once

#include "../canvaslet.hpp"
#include "../../graphics/geometry.hpp"

namespace WarGrey::STEM {
    class __lambda__ IShapelet : public WarGrey::STEM::ICanvaslet {
    public:
        IShapelet(int64_t color = -1, int64_t border_color = -1);
        virtual ~IShapelet() noexcept {}

    public:
        void draw_on_canvas(SDL_Renderer* renderer, float Width, float Height) override;

    protected:
        virtual void draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;
        virtual void fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;
    };

    /*********************************************************************************************/
    class __lambda__ Linelet : public WarGrey::STEM::IShapelet {
    public:
	    Linelet(float ex, float ey, int64_t color);
	    Linelet(float ex, float ey, uint32_t color);
	    Linelet(float ex, float ey, double hue, double saturation = 1.0, double brightness = 1.0);

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

    class __lambda__ HLinelet : public WarGrey::STEM::Linelet {
    public:
        HLinelet(float width, int64_t color) : Linelet(width, 0.0F, color) {}
        HLinelet(float width, uint32_t color) : Linelet(width, 0.0F, color) {}
	    HLinelet(float width, double hue, double saturation = 1.0, double brightness = 1.0) : Linelet(width, 0.0F, hue, saturation, brightness) {}
    };

    class __lambda__ VLinelet : public WarGrey::STEM::Linelet {
    public:
        VLinelet(float height, int64_t color) : Linelet(0.0F, height, color) {}
        VLinelet(float height, uint32_t color) : Linelet(0.0F, height, color) {}
	    VLinelet(float height, double hue, double saturation = 1.0, double brightness = 1.0) : Linelet(0.0F, height, hue, saturation, brightness) {}
    };

    /*********************************************************************************************/
    class __lambda__ Rectanglet : public WarGrey::STEM::IShapelet {
    public:
	    Rectanglet(float edge_size, int64_t color, int64_t border_color = -1);
	    Rectanglet(float edge_size, uint32_t color, int64_t border_color = -1);
	    Rectanglet(float edge_size, double hue, double saturation = 1.0, double brightness = 1.0, int64_t border_color = -1);
	    Rectanglet(float width, float height, int64_t color, int64_t border_color = -1);
	    Rectanglet(float width, float height, uint32_t color, int64_t border_color = -1);
	    Rectanglet(float width, float height, double hue, double saturation = 1.0, double brightness = 1.0, int64_t border_color = -1);

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

    class __lambda__ Squarelet : public WarGrey::STEM::Rectanglet {
    public:
        Squarelet(float edge_size, int64_t color, int64_t border_color = -1)
            : Rectanglet(edge_size, color, border_color) {}

        Squarelet(float edge_size, uint32_t color, int64_t border_color = -1)
            : Rectanglet(edge_size, color, border_color) {}

        Squarelet(float edge_size, double hue, double saturation = 1.0, double brightness = 1.0, int64_t border_color = -1)
            : Rectanglet(edge_size, hue, saturation, brightness, border_color) {}
    };

    class __lambda__ RoundedRectanglet : public WarGrey::STEM::IShapelet {
    public:
	    RoundedRectanglet(float edge_size, float radius, int64_t color, int64_t border_color = -1);
	    RoundedRectanglet(float edge_size, float radius, uint32_t color, int64_t border_color = -1);
	    RoundedRectanglet(float edge_size, float radius, double hue, double saturation = 1.0, double brightness = 1.0, int64_t border_color = -1);
	    RoundedRectanglet(float width, float height, float radius, int64_t color, int64_t border_color = -1);
	    RoundedRectanglet(float width, float height, float radius, uint32_t color, int64_t border_color = -1);
	    RoundedRectanglet(float width, float height, float radius, double hue, double saturation = 1.0, double brightness = 1.0, int64_t border_color = -1);

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

    class __lambda__ RoundedSquarelet : public WarGrey::STEM::RoundedRectanglet {
    public:
        RoundedSquarelet(float edge_size, float radius, int64_t color, int64_t border_color = -1)
            : RoundedRectanglet(edge_size, edge_size, radius, color, border_color) {}

        RoundedSquarelet(float edge_size, float radius, uint32_t color, int64_t border_color = -1)
            : RoundedRectanglet(edge_size, edge_size, radius, color, border_color) {}

        RoundedSquarelet(float edge_size, float radius, double hue, double saturation = 1.0, double brightness = 1.0, int64_t border_color = -1)
            : RoundedRectanglet(edge_size, edge_size, radius, hue, saturation, brightness, border_color) {}
    };

    class __lambda__ Ellipselet : public WarGrey::STEM::IShapelet {
    public:
	    Ellipselet(float radius, int64_t color, int64_t border_color = -1);
	    Ellipselet(float radius, uint32_t color, int64_t border_color = -1);
	    Ellipselet(float radius, double hue, double saturation = 1.0, double brightness = 1.0, int64_t border_color = -1);
        Ellipselet(float aradius, float bradius, int64_t color, int64_t border_color = -1);
	    Ellipselet(float aradius, float bradius, uint32_t color, int64_t border_color = -1);
	    Ellipselet(float aradius, float bradius, double hue, double saturation = 1.0, double brightness = 1.0, int64_t border_color = -1);

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
    
    class __lambda__ Circlet : public WarGrey::STEM::Ellipselet {
    public:
	    Circlet(float radius, int64_t color, int64_t border_color = -1)
            : Ellipselet(radius, radius, color, border_color) {}

        Circlet(float radius, uint32_t color, int64_t border_color = -1)
            : Ellipselet(radius, radius, color, border_color) {}

	    Circlet(float radius, double hue, double saturation = 1.0, double brightness = 1.0, int64_t border_color = -1)
            : Ellipselet(radius, radius, hue, saturation, brightness, border_color) {}
    };
    

    /*********************************************************************************************/
    class __lambda__ Trianglet : public WarGrey::STEM::IShapelet {
    public:
	    Trianglet(float x2, float y2, float x3, float y3, int64_t color, int64_t border_color = -1);
	    Trianglet(float x2, float y2, float x3, float y3, uint32_t color, int64_t border_color = -1);
	    Trianglet(float x2, float y2, float x3, float y3, double hue, double saturation = 1.0, double brightness = 1.0, int64_t border_color = -1);

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

    /*********************************************************************************************/
    class __lambda__ Polygonlet : public WarGrey::STEM::IShapelet {
    public:
	    Polygonlet(const WarGrey::STEM::polygon_vertices& vertices, int64_t color, int64_t border_color = -1);
	    Polygonlet(const WarGrey::STEM::polygon_vertices& vertices, uint32_t color, int64_t border_color = -1);
	    Polygonlet(const WarGrey::STEM::polygon_vertices& vertices, double hue, double saturation = 1.0, double brightness = 1.0, int64_t border_color = -1);
        virtual ~Polygonlet();

	public:
	    void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;

    public:
        size_t get_side_count() { return this->n; }
	    
    protected:
        void on_resize(float new_width, float new_height, float old_width, float old_height) override;
        void draw_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;
        void fill_shape(SDL_Renderer* renderer, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

    private:
        void initialize_vertices(float xscale, float yscale);

	private:
        size_t n;
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

    class __lambda__ RegularPolygonlet : public WarGrey::STEM::Polygonlet {
    public:
	    RegularPolygonlet(size_t n, float radius, int64_t color, int64_t border_color = -1);
	    RegularPolygonlet(size_t n, float radius, uint32_t color, int64_t border_color = -1);
	    RegularPolygonlet(size_t n, float radius, double hue, double saturation = 1.0, double brightness = 1.0, int64_t border_color = -1);
        RegularPolygonlet(size_t n, float radius, float rotation, int64_t color, int64_t border_color = -1);
	    RegularPolygonlet(size_t n, float radius, float rotation, uint32_t color, int64_t border_color = -1);
	    RegularPolygonlet(size_t n, float radius, float rotation, double hue, double saturation = 1.0, double brightness = 1.0, int64_t border_color = -1);
        virtual ~RegularPolygonlet() noexcept {}

    public:
        float get_radius() { return this->radius; }

    private:
        float radius;
	};
}
