#include "forward.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
void WarGrey::STEM::matter_anchor_fraction(MatterAnchor& a, float* ofx, float* ofy) {
    float fx = 0.0F;
    float fy = 0.0F;

    switch (a) {
        case MatterAnchor::LT:                       break;
        case MatterAnchor::LC:            fy = 0.5F; break;
        case MatterAnchor::LB:            fy = 1.0F; break;
        case MatterAnchor::CT: fx = 0.5F;            break;
        case MatterAnchor::CC: fx = 0.5F; fy = 0.5F; break;
        case MatterAnchor::CB: fx = 0.5F; fy = 1.0F; break;
        case MatterAnchor::RT: fx = 1.0F;            break;
        case MatterAnchor::RC: fx = 1.0F; fy = 0.5F; break;
        case MatterAnchor::RB: fx = 1.0F; fy = 1.0F; break;
    }

    (*ofx) = fx;
    (*ofy) = fy;
}
