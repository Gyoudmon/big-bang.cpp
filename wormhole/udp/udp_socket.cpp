#include "udp_socket.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::UDPDaemon::UDPDaemon(uint16_t port) {
    this->self = SDLNet_UDP_Open(port);
        
    this->addrv4.host = INADDR_ANY;
    this->addrv4.port = port;

    if (this->self == nullptr) {
        fprintf(stderr, "Error in creating UDP Socket: %s!\n", SDLNet_GetError());
    } else {
        this->channel = SDLNet_UDP_Bind(this->self, -1, &addrv4);

        if (this->channel == -1) {
            fprintf(stderr, "Error in binding UDP Address: %s!\n", SDLNet_GetError());
        }
    }
}

WarGrey::STEM::UDPDaemon::~UDPDaemon() noexcept {
    if (this->self != nullptr) {
        SDLNet_UDP_Close(this->self);
        this->self = nullptr;
    }
}

bool WarGrey::STEM::UDPDaemon::okay() {
    return (this->self != nullptr);
}

const char* WarGrey::STEM::UDPDaemon::hostname() {
    return SDLNet_ResolveIP(&this->addrv4);
}

uint16_t WarGrey::STEM::UDPDaemon::port() {
    return this->addrv4.port;
}

bool WarGrey::STEM::UDPDaemon::register_to(SDLNet_SocketSet master) {
    bool okay = false;

    if (this->okay()) {
        okay = (SDLNet_UDP_AddSocket(master, this->self) > 0);
    }

    return okay;
}

void WarGrey::STEM::UDPDaemon::unregister_from(SDLNet_SocketSet master) {
    if (this->okay()) {
        SDLNet_UDP_DelSocket(master, this->self);
    }
}

bool WarGrey::STEM::UDPDaemon::ready() {
    return this->okay() && SDLNet_SocketReady(this->self);
}

int WarGrey::STEM::UDPDaemon::recv_into(WarGrey::STEM::UserDatagram* datagram) {
    if (this->okay()) {
        return datagram->recv(this->self);
    } else {
        return -1;
    }
}
