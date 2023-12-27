#include "anchor.hpp"

using namespace GYDM;

/*************************************************************************************************/
GYDM::Anchor::Anchor(const MatterAnchor& name) {
    this->reset(name);
}

void GYDM::Anchor::reset(const MatterAnchor& name) {
    switch (name) {
    case MatterAnchor::LT: this->fx = 0.0F; this->fy = 0.0F; break;
    case MatterAnchor::LC: this->fx = 0.0F; this->fy = 0.5F; break;
    case MatterAnchor::LB: this->fx = 0.0F; this->fy = 1.0F; break;
    case MatterAnchor::CT: this->fx = 0.5F; this->fy = 0.0F; break;
    case MatterAnchor::CC: this->fx = 0.5F; this->fy = 0.5F; break;
    case MatterAnchor::CB: this->fx = 0.5F; this->fy = 1.0F; break;
    case MatterAnchor::RT: this->fx = 1.0F; this->fy = 0.0F; break;
    case MatterAnchor::RC: this->fx = 1.0F; this->fy = 0.5F; break;
    case MatterAnchor::RB: this->fx = 1.0F; this->fy = 1.0F; break;
    }
}
