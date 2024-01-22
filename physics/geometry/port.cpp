#include "port.hpp"

using namespace GYDM;

/*************************************************************************************************/
GYDM::Port::Port(const MatterPort& name) {
    this->reset(name);
}

void GYDM::Port::reset(const MatterPort& name) {
    switch (name) {
    case MatterPort::LT: case MatterPort::NW: this->fx = 0.0F; this->fy = 0.0F; break;
    case MatterPort::LC: case MatterPort::W:  this->fx = 0.0F; this->fy = 0.5F; break;
    case MatterPort::LB: case MatterPort::SW: this->fx = 0.0F; this->fy = 1.0F; break;
    case MatterPort::CT: case MatterPort::N:  this->fx = 0.5F; this->fy = 0.0F; break;
    case MatterPort::CC: case MatterPort::C:  this->fx = 0.5F; this->fy = 0.5F; break;
    case MatterPort::CB: case MatterPort::S:  this->fx = 0.5F; this->fy = 1.0F; break;
    case MatterPort::RT: case MatterPort::NE: this->fx = 1.0F; this->fy = 0.0F; break;
    case MatterPort::RC: case MatterPort::E:  this->fx = 1.0F; this->fy = 0.5F; break;
    case MatterPort::RB: case MatterPort::SE: this->fx = 1.0F; this->fy = 1.0F; break;
    }
}
