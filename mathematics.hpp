#pragma once

#include "datum/flonum.hpp"

namespace WarGrey::STEM {
    float radians_to_degrees(float degrees);
    float degrees_to_radians(float degrees);
	
    bool rectangle_inside(float tlx1, float tly1, float brx1, float bry1, float tlx2, float tly2, float brx2, float bry2);
    bool rectangle_overlay(float tlx1, float tly1, float brx1, float bry1, float tlx2, float tly2, float brx2, float bry2);
    bool rectangle_contain(float tlx, float tly, float brx, float bry, float x, float y);
}

