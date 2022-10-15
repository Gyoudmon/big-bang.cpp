#include "flonum.hpp"
#include "mathematics.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
inline static float quick_degrees_to_radians(float degrees) {
	return degrees * pi / 180.0f;
}

/*************************************************************************************************/
float WarGrey::STEM::radians_to_degrees(float radians) {
	return (radians / pi) * 180.0f;
}

float WarGrey::STEM::degrees_to_radians(float degrees) {
	return quick_degrees_to_radians(degrees);
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

