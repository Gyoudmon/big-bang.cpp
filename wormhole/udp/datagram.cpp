#include "datagram.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::UserDatagram::UserDatagram(size_t size) {
    this->self = SDLNet_AllocPacket(int(size));
}

WarGrey::STEM::UserDatagram::~UserDatagram() noexcept {
    if (this->self != nullptr) {
        SDLNet_FreePacket(this->self);
        this->self = nullptr;
    }
}

size_t WarGrey::STEM::UserDatagram::capacity() {
    return static_cast<size_t>(this->self->maxlen);
}

size_t WarGrey::STEM::UserDatagram::resize(size_t new_size) {
    return static_cast<size_t>(SDLNet_ResizePacket(this->self, int(new_size)));
}

const char* WarGrey::STEM::UserDatagram::hostname() {
    return SDLNet_ResolveIP(&this->self->address);
}

uint16_t WarGrey::STEM::UserDatagram::port() {
    return this->self->address.port;
}

std::basic_string<unsigned char> WarGrey::STEM::UserDatagram::message() {
    return std::basic_string<unsigned char>(this->self->data, static_cast<size_t>(this->self->len));
}

int WarGrey::STEM::UserDatagram::recv(UDPsocket udp) {
    int rsize = -1;

    if (SDLNet_UDP_Recv(udp, this->self) == 1) {
        rsize = this->self->len;
    }

    return rsize;
}
