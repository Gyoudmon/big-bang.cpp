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

