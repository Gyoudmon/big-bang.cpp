#pragma once

#include <SDL2/SDL_net.h>

#include <thread>
#include <map>

#include "slang/udp.hpp"

/**************************************************************************************************/
namespace WarGrey::STEM {
    class SocketDaemon {
    public:
        SocketDaemon(int maxsockets = 16);
        virtual ~SocketDaemon() noexcept;

    public:
        template<typename E>
        bool udp_listen(WarGrey::STEM::UDPLocalPeer<E>* peer, uint16_t service, int packet_capacity = 512) {
            return this->udp_listen(new WarGrey::STEM::UDPDaemon<E>(peer, service, packet_capacity));
        }

    public:
        void start_wait_read_process_loop(int timeout_ms);

    private:
        void wait_read_process_loop(int timeout_ms);

    private:
        bool udp_listen(WarGrey::STEM::IUDPDaemon* daemon);

    private:
        SDLNet_SocketSet master = nullptr;
        std::map<uint16_t, shared_udp_daemon_t> udp_deamons;

    private:
        std::thread* wrpl = nullptr;
        int fallback_timeout = 1;
    };
}
