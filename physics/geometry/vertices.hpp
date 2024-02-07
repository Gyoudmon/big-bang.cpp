#pragma once

#include <vector>

#include "../algebra/point.hpp"

namespace GYDM {
    typedef std::vector<GYDM::Dot> polygon_vertices;

    __lambda__ GYDM::polygon_vertices regular_polygon_vertices(size_t n, float radius, float rotation);
}
