#pragma once

#include <SDL2/SDL_net.h>

#include <thread>
#include <map>

/**************************************************************************************************/
namespace WarGrey::STEM {
    class SocketDaemon {
    public:
        SocketDaemon(int maxsockets = 16);
        virtual ~SocketDaemon() noexcept;

    public:
        bool udp_listen(uint16_t port, int packet_max_size);

    public:
        void start_wait_read_process_loop(int timeout_ms);

    private:
        void wait_read_process_loop(int timeout_ms);

    private:
        SDLNet_SocketSet master = nullptr;
        std::map<uint16_t, UDPsocket> udp_deamons;

    private:
        std::thread* wrpl = nullptr;
        int fallback_timeout = 1;
        int udp_packet_size = 1;
    };
}
