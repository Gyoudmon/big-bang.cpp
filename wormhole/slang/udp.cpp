#include "udp.hpp"
#include "network.hpp"

#include "../../datum/time.hpp"

using namespace Plteen;

/*************************************************************************************************/
Plteen::IUDPDaemon::IUDPDaemon(IUDPLocalPeer* peer, uint16_t port, int packet_size) {
    network_initialize();

    this->peer = peer;
    this->self = SDLNet_UDP_Open(port);
        
    this->addrv4.host = INADDR_ANY;
    this->addrv4.port = port;

    if (this->self == nullptr) {
        fprintf(stderr, "Error in creating UDP daemon: %s!\n", SDLNet_GetError());
    } else {
        int channel = SDLNet_UDP_Bind(this->self, -1, &addrv4);

        if (channel == -1) {
            fprintf(stderr, "Error in binding UDP Address: %s!\n", SDLNet_GetError());
        }

        this->packet = new UserDatagramPacket(packet_size);
    }
}

Plteen::IUDPDaemon::~IUDPDaemon() noexcept {
    if (this->self != nullptr) {
        SDLNet_UDP_Close(this->self);
        this->self = nullptr;
    }

    if (this->packet != nullptr) {
        delete this->packet;
    }

    /* `this->peer` is managed by itself */
}

bool Plteen::IUDPDaemon::okay() {
    return (this->self != nullptr);
}

const char* Plteen::IUDPDaemon::hostname() {
    return SDLNet_ResolveIP(&this->addrv4);
}

uint16_t Plteen::IUDPDaemon::service() {
    return this->addrv4.port;
}

bool Plteen::IUDPDaemon::register_to(SDLNet_SocketSet master) {
    bool okay = false;

    if (this->okay()) {
        okay = (SDLNet_UDP_AddSocket(master, this->self) > 0);
    }

    return okay;
}

void Plteen::IUDPDaemon::unregister_from(SDLNet_SocketSet master) {
    if (this->okay()) {
        SDLNet_UDP_DelSocket(master, this->self);
    }
}

/*************************************************************************************************/
size_t Plteen::IUDPDaemon::packet_capacity() {
    if (this->packet != nullptr) {
        return this->packet->capacity();
    } else {
        return 0;
    }
}

size_t Plteen::IUDPDaemon::packet_resize(size_t new_size) {
    if (this->packet != nullptr) {
        return this->packet->resize(new_size);
    } else {
        return 0;
    }
}

bool Plteen::IUDPDaemon::ready() {
    return this->okay() && SDLNet_SocketReady(this->self);
}

bool Plteen::IUDPDaemon::recv_packet() {
    if (this->okay() && (this->packet != nullptr)) {
        return this->packet->recv(this->self);
    } else {
        return false;
    }
}

/*************************************************************************************************/
void Plteen::IUDPDaemon::dispatch_packet() {
    if ((this->peer != nullptr) && (!this->peer->absent())) {    
        if (this->packet != nullptr) {
            shared_datagram_t datagram = std::make_shared<Datagram>();
            uint8_t type;
        
            datagram->timestamp = current_milliseconds();
            datagram->remote_host = this->packet->hostname();
            datagram->payload = this->packet->unbox(&type, &datagram->transaction, &datagram->respond_port, &datagram->payload_size);
    
            if (datagram->payload != nullptr) {
                this->peer->on_user_datagram(this->service(), type, datagram);
            }
        }
    }
}

/*************************************************************************************************/
Plteen::IUDPClient::IUDPClient(int packet_size, uint16_t port) {
    network_initialize();

    this->self = SDLNet_UDP_Open(port);

    if (this->self == nullptr) {
        fprintf(stderr, "Error in creating UDP client: %s!\n", SDLNet_GetError());
    } else {
        this->packet = new UserDatagramPacket(packet_size);
    }
}

Plteen::IUDPClient::~IUDPClient() noexcept {
    if (this->self != nullptr) {
        SDLNet_UDP_Close(this->self);
        this->self = nullptr;
    }

    if (this->packet != nullptr) {
        delete this->packet;
    }
}

bool Plteen::IUDPClient::okay() {
    return (this->self != nullptr);
}

size_t Plteen::IUDPClient::packet_capacity() {
    if (this->packet != nullptr) {
        return this->packet->capacity();
    } else {
        return 0;
    }
}

size_t Plteen::IUDPClient::packet_resize(size_t new_size) {
    if (this->packet != nullptr) {
        return this->packet->resize(new_size);
    } else {
        return 0;
    }
}
