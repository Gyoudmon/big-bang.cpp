#pragma once

#include "../canvaslet.hpp"
#include "../../graphics/geometry.hpp"

#include <optional>

namespace WarGrey::STEM {
    class __lambda__ IShapelet : public WarGrey::STEM::ICanvaslet {
    public:
        IShapelet(const std::optional<uint32_t>& color = {}, const std::optional<uint32_t>& border_color = {});
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
	    Linelet(float ex, float ey, const std::optional<uint32_t>& color);

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
        HLinelet(float width, const std::optional<uint32_t>& color) : Linelet(width, 0.0F, color) {}
    };

    class __lambda__ VLinelet : public WarGrey::STEM::Linelet {
    public:
        VLinelet(float height, const std::optional<uint32_t>& color) : Linelet(0.0F, height, color) {}
    };

    /*********************************************************************************************/
    class __lambda__ Rectanglet : public WarGrey::STEM::IShapelet {
    public:
	    Rectanglet(float edge_size, const std::optional<uint32_t>& color, const std::optional<uint32_t>& border_color = {});
	    Rectanglet(float width, float height, const std::optional<uint32_t>& color, const std::optional<uint32_t>& border_color = {});
	    
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
        Squarelet(float edge_size, const std::optional<uint32_t>& color, const std::optional<uint32_t>& border_color = {})
            : Rectanglet(edge_size, color, border_color) {}
    };

    class __lambda__ RoundedRectanglet : public WarGrey::STEM::IShapelet {
    public:
	    RoundedRectanglet(float edge_size, float radius, const std::optional<uint32_t>& color, const std::optional<uint32_t>& border_color = {});
	    RoundedRectanglet(float width, float height, float radius, const std::optional<uint32_t>& color, const std::optional<uint32_t>& border_color = {});
	    
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
        RoundedSquarelet(float edge_size, float radius, const std::optional<uint32_t>& color, const std::optional<uint32_t>& border_color = {})
            : RoundedRectanglet(edge_size, edge_size, radius, color, border_color) {}
    };

    class __lambda__ Ellipselet : public WarGrey::STEM::IShapelet {
    public:
	    Ellipselet(float radius, const std::optional<uint32_t>& color, const std::optional<uint32_t>& border_color = {});
	    Ellipselet(float aradius, float bradius, const std::optional<uint32_t>& color, const std::optional<uint32_t>& border_color = {});

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
	    Circlet(float radius, const std::optional<uint32_t>& color, const std::optional<uint32_t>& border_color = {})
            : Ellipselet(radius, radius, color, border_color) {}
    };
    
    /*********************************************************************************************/
    class __lambda__ Polygonlet : public WarGrey::STEM::IShapelet {
    public:
	    Polygonlet(const WarGrey::STEM::polygon_vertices& vertices, const std::optional<uint32_t>& color, const std::optional<uint32_t>& border_color = {});
	    virtual ~Polygonlet();

	public:
	    void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;

    public:
        size_t side_count() { return this->n; }
        float leftmost_x() { return this->lx; }
        float topmost_y() { return this->ty; }
	    
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
	    RegularPolygonlet(size_t n, float radius, const std::optional<uint32_t>& color, const std::optional<uint32_t>& border_color = {});
	    RegularPolygonlet(size_t n, float radius, float rotation, const std::optional<uint32_t>& color, const std::optional<uint32_t>& border_color = {});
	    virtual ~RegularPolygonlet() noexcept {}

    public:
        float radius() { return this->_radius; }

    private:
        float _radius;
	};

    class __lambda__ Trianglet : public WarGrey::STEM::RegularPolygonlet {
    public:
	    Trianglet(float side_length, const std::optional<uint32_t>& color, const std::optional<uint32_t>& border_color = {});
	    Trianglet(float side_length, float rotation, const std::optional<uint32_t>& color, const std::optional<uint32_t>& border_color = {});
    };
}
