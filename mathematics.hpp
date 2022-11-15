#pragma once

#include "datum/flonum.hpp"

namespace WarGrey::STEM {
    float radians_to_degrees(float degrees);
    float degrees_to_radians(float degrees);
	
    bool point_inside(float px, float py, float x1, float y1, float x2, float y2);
    bool rectangle_inside(float tlx1, float tly1, float brx1, float bry1, float tlx2, float tly2, float brx2, float bry2);
    bool rectangle_overlay(float tlx1, float tly1, float brx1, float bry1, float tlx2, float tly2, float brx2, float bry2);
    bool rectangle_contain(float tlx, float tly, float brx, float bry, float x, float y);

    bool lines_intersection(float x11, float y11, float x12, float y12, float x21, float y21, float x22, float y22,
        float* px, float* py, float* t1 = nullptr, float* t2 = nullptr);
}

