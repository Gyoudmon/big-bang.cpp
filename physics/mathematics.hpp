#pragma once

#include "../datum/box.hpp"
#include "../datum/flonum.hpp"

namespace GYDM {
    template<typename Fl>
    struct Length { using type = float; };

    template<>
    struct Length<double> { using type = double; };

    /*********************************************************************************************/
    template<typename Fl>
    inline Fl clamp(Fl v, Fl min, Fl max) {
        if (v > max) {
            v = max;
        } else if (v < min) {
            v = min;
        }

        return v;
    }

    template<typename Fl>
    inline Fl radians_to_degrees(Fl radians) {
        return (radians / Fl(pi)) * Fl(180.0);
    }

    template<typename Fl>
    inline Fl degrees_to_radians(Fl degrees) {
        return (degrees * Fl(pi)) / Fl(180.0);
    }

    template<typename Fl>
    Fl degrees_normalize(Fl degrees, Fl degrees_start = Fl(0.0)) {
	    Fl degrees_end = degrees_start + Fl(360.0);

	    while (degrees < degrees_start) degrees += Fl(360.0);
	    while (degrees >= degrees_end) degrees -= Fl(360.0);

	    return degrees;
    }

    template<typename Fl>
    Fl radians_normalize(Fl radians, Fl degrees_start = Fl(0.0)) {
	    Fl radians_start = degrees_to_radians(degrees_start);
	    Fl radians_end = radians_start + Fl(d_pi);

	    while (radians < radians_start) radians += Fl(d_pi);
	    while (radians >= radians_end) radians -= Fl(d_pi);

	    return radians;
    }

    template<typename Fl>
    void orthogonal_decompose(Fl magnitude, Fl direction, Fl* x, Fl* y, bool is_radian = true) {
        Fl rad = is_radian ? direction : degrees_to_radians(direction);

        SET_BOX(x, magnitude * flcos(rad));
        SET_BOX(y, magnitude * flsin(rad));
    }

    template<typename Fl>
    inline Fl vector_magnitude(Fl x, Fl y) {
        return flsqrt(x * x + y * y);
    }

    template<typename Fl>
    inline Fl vector_direction(Fl x, Fl y, bool need_radian = true) {
        Fl rad = flatan(y, x);

        return need_radian ? rad : radians_to_degrees(rad);
    }

    template<typename Fl>
    void vector_rotate(Fl x, Fl y, Fl theta, Fl* rx, Fl* ry, Fl ox, Fl oy, bool is_radian = true) {
        Fl radians = is_radian ? theta : degrees_to_radians(theta);
	    Fl cosr = flcos(radians);
	    Fl sinr = flsin(radians);
	    Fl dx = x - ox;
	    Fl dy = y - oy;

	    SET_BOX(rx, dx * cosr - dy * sinr + ox);
	    SET_BOX(ry, dx * sinr + dy * cosr + oy);
    }

    template<typename Fl>
    inline Fl vector_clamp(Fl v, Fl ceil) {
        return clamp(v, -ceil, ceil);
    }

    /*********************************************************************************************/
    template<typename Fl>
    inline bool point_inside(Fl px, Fl py, Fl x1, Fl y1, Fl x2, Fl y2) {
        return (x1 <= x2 ? flin(x1, px, x2) : flin(x2, px, x1))
            && (y1 <= y2 ? flin(y1, py, y2) : flin(y2, py, y2));
    }

    template<typename Fl>
    inline bool rectangle_inside(Fl tlx1, Fl tly1, Fl brx1, Fl bry1, Fl tlx2, Fl tly2, Fl brx2, Fl bry2) {
        return flin(tlx2, tlx1, brx2) && flin(tlx2, brx1, brx2) && (flin(tly2, tly1, bry2) && flin(tly2, bry1, bry2));
    }

    template<typename Fl>
    inline bool rectangle_overlay(Fl tlx1, Fl tly1, Fl brx1, Fl bry1, Fl tlx2, Fl tly2, Fl brx2, Fl bry2) {
        return !((brx1 < tlx2) || (tlx1 > brx2) || (bry1 < tly2) || (tly1 > bry2));
    }

    template<typename Fl>
    inline bool rectangle_contain(Fl tlx, Fl tly, Fl brx, Fl bry, Fl x, Fl y) {
        return flin(tlx, x, brx) && flin(tly, y, bry);
    }

    /*********************************************************************************************/
    template<typename Fl>
    void circle_point(Fl radius, Fl angle, Fl* x, Fl* y, bool is_radian = false) {
        Fl rad = is_radian ? angle : degrees_to_radians(angle);

	    SET_BOX(x, radius * flcos(rad));
	    SET_BOX(y, radius * flsin(rad));
    }

    template<typename Fl>
    void ellipse_point(Fl radiusX, Fl radiusY, Fl angle, Fl* x, Fl* y, bool is_radian = false) {
        Fl rad = is_radian ? angle : degrees_to_radians(angle);

	    SET_BOX(x, radiusX * flcos(rad));
	    SET_BOX(y, radiusY * flsin(rad));
    }

    template<typename Fl>
    Fl point_distance(Fl x1, Fl y1, Fl x2, Fl y2) {
        return flsqrt(flsqr(x2 - x1) + flsqr(y2 - y1));
    }

    template<typename Fl>
    Fl point_distance_squared(Fl x1, Fl y1, Fl x2, Fl y2) {
        return flsqr(x2 - x1) + flsqr(y2 - y1);
    }

    template<typename Fl>
    void line_point(Fl x0, Fl y0, Fl x1, Fl y1, double t, Fl* x, Fl* y) {
	    Fl flt = Fl(t);

	    SET_BOX(x, (x0 - x1) * flt + x1);
	    SET_BOX(y, (y0 - y1) * flt + y1);
    }

    template<typename Fl>
    Fl line_slope(Fl x0, Fl y0, Fl x1, Fl y1) {
	    return (x1 == x0) ? Fl(flnan) : (y1 - y0) / (x1 - x0);
    }

    /*********************************************************************************************/
    template<typename Fl>
    bool lines_intersect(Fl x11, Fl y11, Fl x12, Fl y12, Fl x21, Fl y21, Fl x22, Fl y22,
            Fl* px = nullptr, Fl* py = nullptr, Fl* t1 = nullptr, Fl* t2 = nullptr) {
        // find the intersection point P(px, py) of L1((x11, y11), (x12, y12)) and L2((x21, y21), (x22, y22))

        /** Theorem
         * In Euclidean Vector Space, A line can be represented in vector form as L = v0 + tv,
         * the parameter `t` can be used to detect the interval of line. More precisely, for `t`:
         *   -inf < t < +inf, L is an infinitely long line.
         *   0 <= t <= 1, L is a line segment.
         *   0 <= t < +inf, L is a ray.
         *
         * a). L1 = (x11, y11) + t1(x12 - x11, y12 - y11)
         * b). L2 = (x21, y21) + t2(x22 - x21, y22 - x21)
         *  ==> t1 = + [(x11 - x21)(y21 - y22) - (y11 - y21)(x21 - x22)] / [(x11 - x12)(y21 - y22) - (y11 - y12)(x21 - x22)]
         *      t2 = - [(x11 - x12)(y11 - y21) - (y11 - y12)(x11 - x21)] / [(x11 - x12)(y21 - y22) - (y11 - y12)(x21 - x22)]
         *  ==> P(x11 + t1(x12 - x11), y11 + t1(y12 - y11)) or
         *      P(x21 + t2(x22 - x21), y21 + t2(y22 - y21))
         */

        Fl denominator = ((x11 - x12) * (y21 - y22) - (y11 - y12) * (x21 - x22));
        // WARNING: client applications should check the float relevant errors when two lines are almost parallel
        bool intersected = (denominator != Fl(0.0));

        if (intersected) {
	        Fl T1 = +((x11 - x21) * (y21 - y22) - (y11 - y21) * (x21 - x22)) / denominator;
	        Fl T2 = -((x11 - x12) * (y11 - y21) - (y11 - y12) * (x11 - x21)) / denominator;

	        SET_VALUES(t1, T1, t2, T2);
	        SET_BOX(px, x21 + T2 * (x22 - x21));
	        SET_BOX(py, y21 + T2 * (y22 - y21));
        } else {
            SET_VALUES(px, Fl(flnan), py, Fl(flnan));
            SET_VALUES(t1, Fl(flnan), t2, Fl(flnan));
        }

        return intersected;
    }
}
