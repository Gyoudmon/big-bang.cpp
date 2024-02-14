#pragma once

#include "../datum/box.hpp"
#include "../datum/flonum.hpp"

#include <type_traits>

namespace GYDM {
    template<typename T> struct SuperType { using type = long long; };
    template<> struct SuperType<double> { using type = double; };
    template<> struct SuperType<float> { using type = double; };
    
    template<typename Fl> struct LengthType { using type = float; };
    template<> struct LengthType<double> { using type = double; };

    template<typename T>
    using SuperDatum = typename SuperType<T>::type;

    template<typename T>
    using RealDatum = typename std::enable_if<std::is_arithmetic<T>::value>::type;

    template<typename T>
    using FixnumDatum = typename std::enable_if<std::is_integral<T>::value>::type;

    template<typename T>
    using FlonumDatum = typename std::enable_if<std::is_floating_point<T>::value>::type;

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
    inline void orthogonal_decompose(Fl magnitude, Fl direction, Fl* x, Fl* y, bool is_radian = true) {
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
    inline void vector_rotate(Fl x, Fl y, Fl theta, Fl* rx, Fl* ry, Fl ox, Fl oy, bool is_radian = true) {
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
    inline void circle_point(Fl radius, Fl angle, Fl* x, Fl* y, bool is_radian = false) {
        Fl rad = is_radian ? angle : degrees_to_radians(angle);

	    SET_BOX(x, radius * flcos(rad));
	    SET_BOX(y, radius * flsin(rad));
    }

    template<typename Fl>
    inline void ellipse_point(Fl radiusX, Fl radiusY, Fl angle, Fl* x, Fl* y, bool is_radian = false) {
        Fl rad = is_radian ? angle : degrees_to_radians(angle);

	    SET_BOX(x, radiusX * flcos(rad));
	    SET_BOX(y, radiusY * flsin(rad));
    }

    template<typename Fl>
    inline Fl point_distance(Fl x1, Fl y1, Fl x2, Fl y2) {
        return flsqrt(flsqr(x2 - x1) + flsqr(y2 - y1));
    }

    template<typename Fl>
    inline Fl point_distance_squared(Fl x1, Fl y1, Fl x2, Fl y2) {
        return flsqr(x2 - x1) + flsqr(y2 - y1);
    }

    template<typename Fl>
    inline void line_point(Fl x0, Fl y0, Fl x1, Fl y1, double t, Fl* x, Fl* y) {
	    Fl flt = Fl(t);

	    SET_BOX(x, (x0 - x1) * flt + x1);
	    SET_BOX(y, (y0 - y1) * flt + y1);
    }

    template<typename Fl>
    inline Fl line_slope(Fl x0, Fl y0, Fl x1, Fl y1) {
	    return (x1 == x0) ? Fl(flnan) : (y1 - y0) / (x1 - x0);
    }

    /*********************************************************************************************/
    /**
     * for Error-Free Transformations
     * 
     *      FMA(x, y, z) = x * y + z
     * where the FMA is short for `fused multiply add`
     * 
     * The `error` might be quite considerable
     */

    template<typename Fl>
    inline Fl two_product(Fl a, Fl b) {
        Fl result = a * b;
        Fl error = flfma(a, b, -result);

        return result + error;
    }

    template<typename Fl>
    inline Fl sum_of_products(Fl a, Fl b, Fl c, Fl d) {
        Fl cd = c * d;
        Fl result = flfma(a, b, cd);
        Fl error = flfma(c, d, -cd);
        
        return result + error;    
    }

    template<typename Fl>
    inline Fl difference_of_products(Fl a, Fl b, Fl c, Fl d) {
        Fl cd = c * d;
        Fl result = flfma(a, b, -cd);
        Fl error = flfma(-c, d, cd);

        return result + error;
    }

    template<typename Super, typename Fl1, typename Fl2 = Fl1>
    inline Super safe_two_product(Fl1 a, Fl2 b) {
        return two_product(Super(a), Super(b));
    }

    template<typename Super, typename Fl1, typename Fl2 = Fl1, typename Fl3 = Fl1, typename Fl4 = Fl1>
    inline Super safe_sum_of_products(Fl1 a, Fl2 b, Fl3 c, Fl4 d) {
        return sum_of_products(Super(a), Super(b), Super(c), Super(d));
    }

    template<typename Super, typename Fl1, typename Fl2 = Fl1, typename Fl3 = Fl1, typename Fl4 = Fl1>
    inline Super safe_difference_of_products(Fl1 a, Fl2 b, Fl3 c, Fl4 d) {
        return difference_of_products(Super(a), Super(b), Super(c), Super(d));
    }
    
    template<typename Fl, typename Super>
    Fl matrix_determinant(const Fl (&self)[1][1]) {
        return self[0][0];
    }

    template<typename Fl, typename Super>
    inline Super matrix_determinant(const Fl (&self)[2][2]) {
        return safe_difference_of_products<Super, Fl>(self[0][0], self[1][1], self[0][1], self[1][0]);
    }

    template<typename Fl, typename Super>
    Super matrix_determinant(const Fl (&self)[3][3]) {
        Super minor12 = safe_difference_of_products<Super, Fl>(self[1][1], self[2][2], self[1][2], self[2][1]);
        Super minor02 = safe_difference_of_products<Super, Fl>(self[1][0], self[2][2], self[1][2], self[2][0]);
        Super minor01 = safe_difference_of_products<Super, Fl>(self[1][0], self[2][1], self[1][1], self[2][0]);
    
        return flfma(Super(self[0][2]), minor01,
                difference_of_products(Super(self[0][0]), minor12, Super(self[0][1]), minor02));
    }

    template<typename Fl, typename Super>
    Super matrix_determinant(const Fl (&self)[4][4]) {
        Super s0 = safe_difference_of_products<Super, Fl>(self[0][0], self[1][1], self[1][0], self[0][1]);
        Super s1 = safe_difference_of_products<Super, Fl>(self[0][0], self[1][2], self[1][0], self[0][2]);
        Super s2 = safe_difference_of_products<Super, Fl>(self[0][0], self[1][3], self[1][0], self[0][3]);

        Super s3 = safe_difference_of_products<Super, Fl>(self[0][1], self[1][2], self[1][1], self[0][2]);
        Super s4 = safe_difference_of_products<Super, Fl>(self[0][1], self[1][3], self[1][1], self[0][3]);
        Super s5 = safe_difference_of_products<Super, Fl>(self[0][2], self[1][3], self[1][2], self[0][3]);

        Super c0 = safe_difference_of_products<Super, Fl>(self[2][0], self[3][1], self[3][0], self[2][1]);
        Super c1 = safe_difference_of_products<Super, Fl>(self[2][0], self[3][2], self[3][0], self[2][2]);
        Super c2 = safe_difference_of_products<Super, Fl>(self[2][0], self[3][3], self[3][0], self[2][3]);

        Super c3 = safe_difference_of_products<Super, Fl>(self[2][1], self[3][2], self[3][1], self[2][2]);
        Super c4 = safe_difference_of_products<Super, Fl>(self[2][1], self[3][3], self[3][1], self[2][3]);
        Super c5 = safe_difference_of_products<Super, Fl>(self[2][2], self[3][3], self[3][2], self[2][3]);

        return difference_of_products(s0, c5, s1, c4)
            + difference_of_products(s2, c3, -s3, c2)
            + difference_of_products(s5, c0, s4, c1);
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
