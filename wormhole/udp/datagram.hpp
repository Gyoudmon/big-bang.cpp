#pragma once

#include <SDL2/SDL_net.h>
#include <string>

namespace WarGrey::STEM {
    class UserDatagram {
    public:
        UserDatagram(size_t size = 512);
        virtual ~UserDatagram() noexcept;

    public:
        std::basic_string<unsigned char> message();
        int recv(UDPsocket udp);

    public:
        size_t capacity();
        size_t resize(size_t new_size);
        const char* hostname();
        uint16_t port();

    private:
        UDPpacket* self;
    };
}
