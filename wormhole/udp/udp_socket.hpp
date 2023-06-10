#pragma once

#include <SDL2/SDL_net.h>
#include <memory>

#include "datagram.hpp"

namespace WarGrey::STEM {
    class UDPDaemon {
    public:
        UDPDaemon(uint16_t port);
        virtual ~UDPDaemon() noexcept;

    public:
        bool okay();
        const char* hostname();
        uint16_t port();

    public:
        bool register_to(SDLNet_SocketSet master);
        void unregister_from(SDLNet_SocketSet master);

    public:
        bool ready();
        int recv_into(WarGrey::STEM::UserDatagram* datagram);

    private:
        UDPsocket self;
        IPaddress addrv4;
        int channel;
    };

    typedef std::shared_ptr<UDPDaemon> shared_udp_daemon_t;
}
