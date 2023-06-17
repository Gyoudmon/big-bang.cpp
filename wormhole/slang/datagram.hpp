#pragma once

#include <SDL2/SDL_net.h>
#include <string>
#include <memory>

namespace WarGrey::STEM {
    struct Datagram {
        int64_t timestamp;
        const char* remote_host;
        uint16_t respond_port;
        uint16_t transaction;
        const unsigned char* payload;
        size_t payload_size;
    };

    typedef std::shared_ptr<Datagram> shared_datagram_t;

    /*********************************************************************************************/
    class UserDatagramPacket {
    public:
        UserDatagramPacket(size_t size = 512);
        virtual ~UserDatagramPacket() noexcept;

    public:
        int recv(UDPsocket udp);

    public:
        const unsigned char* unbox(uint8_t* type, uint16_t* transaction, uint16_t* response_port, size_t* size);

    public:
        size_t capacity();
        size_t resize(size_t new_size);
        const char* hostname();
        uint16_t port();

    private:
        UDPpacket* self;
    };
}
