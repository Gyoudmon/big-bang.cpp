#pragma once

#include "../canvaslet.hpp"
#include "../../physics/algebra/point.hpp"
#include "../../physics/geometry/aabox.hpp"
#include "../../physics/geometry/vertices.hpp"

namespace GYDM {
    class __lambda__ IShapelet : public GYDM::ICanvaslet {
    public:
        IShapelet(const GYDM::RGBA& color = transparent, const GYDM::RGBA& border_color = transparent);
        virtual ~IShapelet() noexcept {}

    public:
        void draw_on_canvas(GYDM::dc_t* dc, float Width, float Height) override;

    protected:
        virtual void draw_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;
        virtual void fill_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;
    };

    /*********************************************************************************************/
    class __lambda__ Linelet : public GYDM::IShapelet {
    public:
	    Linelet(float ex, float ey, const GYDM::RGBA& color);

	public:
        GYDM::Box get_bounding_box() override;
        
    protected:
        void on_resize(float new_width, float new_height, float old_width, float old_height) override;
        void draw_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override {}
        void fill_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

    private:
        float epx;
        float epy;
    };

    class __lambda__ HLinelet : public GYDM::Linelet {
    public:
        HLinelet(float width, const GYDM::RGBA& color) : Linelet(width, 0.0F, color) {}
    };

    class __lambda__ VLinelet : public GYDM::Linelet {
    public:
        VLinelet(float height, const GYDM::RGBA& color) : Linelet(0.0F, height, color) {}
    };

    /*********************************************************************************************/
    class __lambda__ Rectanglet : public GYDM::IShapelet {
    public:
	    Rectanglet(float edge_size, const GYDM::RGBA& color, const GYDM::RGBA& border_color = transparent);
	    Rectanglet(float width, float height, const GYDM::RGBA& color, const GYDM::RGBA& border_color = transparent);
	    
    public:
        GYDM::Box get_bounding_box() override;

    protected:
        void on_resize(float new_width, float new_height, float old_width, float old_height) override;
        void draw_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;
        void fill_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

	private:
	    float width;
	    float height;
    };

    class __lambda__ Squarelet : public GYDM::Rectanglet {
    public:
        Squarelet(float edge_size, const GYDM::RGBA& color, const GYDM::RGBA& border_color = transparent)
            : Rectanglet(edge_size, color, border_color) {}
    };

    class __lambda__ RoundedRectanglet : public GYDM::IShapelet {
    public:
	    RoundedRectanglet(float edge_size, float radius, const GYDM::RGBA& color, const GYDM::RGBA& border_color = transparent);
	    RoundedRectanglet(float width, float height, float radius, const GYDM::RGBA& color, const GYDM::RGBA& border_color = transparent);
	    
	public:
	    GYDM::Box get_bounding_box() override;
        
    protected:
        void on_resize(float new_width, float new_height, float old_width, float old_height) override;
        void draw_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;
        void fill_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

	private:
	    float width;
	    float height;
        float radius;
    };

    class __lambda__ RoundedSquarelet : public GYDM::RoundedRectanglet {
    public:
        RoundedSquarelet(float edge_size, float radius, const GYDM::RGBA& color, const GYDM::RGBA& border_color = transparent)
            : RoundedRectanglet(edge_size, edge_size, radius, color, border_color) {}
    };

    class __lambda__ Ellipselet : public GYDM::IShapelet {
    public:
	    Ellipselet(float radius, const GYDM::RGBA& color, const GYDM::RGBA& border_color = transparent);
	    Ellipselet(float aradius, float bradius, const GYDM::RGBA& color, const GYDM::RGBA& border_color = transparent);

	public:
	    GYDM::Box get_bounding_box() override;
        
    protected:
        void on_resize(float new_width, float new_height, float old_width, float old_height) override;
        void draw_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;
        void fill_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

	private:
	    float aradius;
	    float bradius;
    };
    
    class __lambda__ Circlet : public GYDM::Ellipselet {
    public:
	    Circlet(float radius, const GYDM::RGBA& color, const GYDM::RGBA& border_color = transparent)
            : Ellipselet(radius, radius, color, border_color) {}
    };
    
    /*********************************************************************************************/
    class __lambda__ Polygonlet : public GYDM::IShapelet {
    public:
	    Polygonlet(const GYDM::polygon_vertices& vertices, const GYDM::RGBA& color, const GYDM::RGBA& border_color = transparent);
	    virtual ~Polygonlet();

	public:
	    GYDM::Box get_bounding_box() override;
        
    public:
        size_t side_count() { return this->n; }
        float leftmost_x() { return this->lx; }
        float topmost_y() { return this->ty; }
	    
    protected:
        void on_resize(float new_width, float new_height, float old_width, float old_height) override;
        void draw_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;
        void fill_shape(GYDM::dc_t* dc, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

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

    class __lambda__ RegularPolygonlet : public GYDM::Polygonlet {
    public:
	    RegularPolygonlet(size_t n, float radius, const GYDM::RGBA& color, const GYDM::RGBA& border_color = transparent);
	    RegularPolygonlet(size_t n, float radius, float rotation, const GYDM::RGBA& color, const GYDM::RGBA& border_color = transparent);

    public:
        float radius() { return this->_radius; }

    private:
        float _radius;
	};

    class __lambda__ Trianglet : public GYDM::RegularPolygonlet {
    public:
	    Trianglet(float side_length, const GYDM::RGBA& color, const GYDM::RGBA& border_color = transparent);
	    Trianglet(float side_length, float rotation, const GYDM::RGBA& color, const GYDM::RGBA& border_color = transparent);
    };
}
