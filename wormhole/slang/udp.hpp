#pragma once

#include <SDL2/SDL_net.h>
#include <memory>

#include "datagram.hpp"

namespace WarGrey::STEM {
    class IUDPLocalPeer {
    public:
        virtual bool absent() { return false; }

    public:
        virtual void on_user_datagram(uint16_t service, int type, const shared_datagram_t& datagram) = 0;
    };

    /*********************************************************************************************/
    class IUDPDaemon {
    public:
        IUDPDaemon(IUDPLocalPeer* peer, uint16_t service, int packet_size);
        virtual ~IUDPDaemon() noexcept;

    public:
        bool okay();
        const char* hostname();
        uint16_t service();

    public:
        bool register_to(SDLNet_SocketSet master);
        void unregister_from(SDLNet_SocketSet master);

    public:
        bool ready();
        size_t packet_capacity();
        size_t packet_resize(size_t new_size);
        bool recv_packet();
        void dispatch_packet();

    private:
        UDPsocket self;
        IPaddress addrv4;
        WarGrey::STEM::UserDatagramPacket* packet = nullptr;
        WarGrey::STEM::IUDPLocalPeer* peer;
    };

    typedef std::shared_ptr<IUDPDaemon> shared_udp_daemon_t;

    class IUDPClient {
    public:
        IUDPClient(int packet_size, uint16_t response_port);
        virtual ~IUDPClient() noexcept;

    public:
        bool okay();
        size_t packet_capacity();
        size_t packet_resize(size_t new_size);

    private:
        UDPsocket self;
        IPaddress addrv4;
        WarGrey::STEM::UserDatagramPacket* packet = nullptr;
    };

    /*********************************************************************************************/
    template<typename E>
    class UDPLocalPeer : public WarGrey::STEM::IUDPLocalPeer {
    public:
        void on_user_datagram(uint16_t service, int type, const shared_datagram_t& datagram) override {
            this->on_user_datagram(service, static_cast<E>(type), datagram);
        }

    protected:
        virtual void on_user_datagram(uint16_t service, E type, const shared_datagram_t& datagram) = 0;
    };

    template<typename E>
    class UDPDaemon : public WarGrey::STEM::IUDPDaemon {
    public:
        UDPDaemon(UDPLocalPeer<E>* peer, uint16_t service, int packet_size)
            : IUDPDaemon(peer, service, packet_size) {}
        virtual ~UDPDaemon() noexcept { /* do nothing */ }
    };
}
