#pragma once

#include <vector>

namespace WarGrey::STEM {
    typedef std::vector<std::pair<float, float>> polygon_vertices;

    __lambda__ WarGrey::STEM::polygon_vertices regular_polygon_vertices(size_t n, float radius, float rotation);
}
