#include "mathematics.hpp"

#include "datum/box.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
inline static float quick_degrees_to_radians(float degrees) {
    return (degrees * pi_f) / 180.0f;
}

/*************************************************************************************************/
float WarGrey::STEM::radians_to_degrees(float radians) {
    return (radians / pi_f) * 180.0f;
}

float WarGrey::STEM::degrees_to_radians(float degrees) {
    return quick_degrees_to_radians(degrees);
}

bool WarGrey::STEM::point_inside(float px, float py, float x1, float y1, float x2, float y2) {
    return (x1 <= x2 ? flin(x1, px, x2) : flin(x2, px, x1))
        && (y1 <= y2 ? flin(y1, py, y2) : flin(y2, py, y2));
}

bool WarGrey::STEM::rectangle_inside(float tlx1, float tly1, float brx1, float bry1, float tlx2, float tly2, float brx2, float bry2) {
    return flin(tlx2, tlx1, brx2) && flin(tlx2, brx1, brx2) && (flin(tly2, tly1, bry2) && flin(tly2, bry1, bry2));
}

bool WarGrey::STEM::rectangle_overlay(float tlx1, float tly1, float brx1, float bry1, float tlx2, float tly2, float brx2, float bry2) {
    return !((brx1 < tlx2) || (tlx1 > brx2) || (bry1 < tly2) || (tly1 > bry2));
}

bool WarGrey::STEM::rectangle_contain(float tlx, float tly, float brx, float bry, float x, float y) {
    return flin(tlx, x, brx) && flin(tly, y, bry);
}

/*************************************************************************************************/
bool WarGrey::STEM::lines_intersection(float x11, float y11, float x12, float y12, float x21, float y21, float x22, float y22,
        float* px, float* py, float* t1, float* t2) {
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

    float denominator = ((x11 - x12) * (y21 - y22) - (y11 - y12) * (x21 - x22));
    // WARNING: client applications should check the flonum relevant errors when two lines are almost parallel
    bool intersected = (denominator != 0.0);

    if (intersected) {
	float T1 = +((x11 - x21) * (y21 - y22) - (y11 - y21) * (x21 - x22)) / denominator;
	float T2 = -((x11 - x12) * (y11 - y21) - (y11 - y12) * (x11 - x21)) / denominator;

	SET_VALUES(t1, T1, t2, T2);
	SET_BOX(px, x21 + T2 * (x22 - x21));
	SET_BOX(py, y21 + T2 * (y22 - y21));
    }

    return intersected;
}

