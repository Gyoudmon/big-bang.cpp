#include "vertices.hpp"

#include "../mathematics.hpp"

using namespace GYDM;

/*************************************************************************************************/
GYDM::polygon_vertices GYDM::regular_polygon_vertices(size_t n, float radius, float rotation) {
    // for inscribed regular polygon, the radius should be `Rcos(pi/n)`
    float start = degrees_to_radians(rotation);
    float delta = 2.0F * pi_f / float(n);
    polygon_vertices pvs;
    
    for (size_t idx = 0; idx < n; idx++) {
        float theta = start + delta * float(idx);
        float px = radius * flcos(theta);
        float py = radius * flsin(theta);
        
        pvs.push_back(Dot(px, py));
    }

    return pvs;
}
