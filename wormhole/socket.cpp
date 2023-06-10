#include "socket.hpp"

#include "udp/datagram.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
WarGrey::STEM::SocketDaemon::SocketDaemon(int maxsockets) {
    int msckt = (maxsockets <= 0) ? 1 : maxsockets;

    this->master = SDLNet_AllocSocketSet(msckt);
    this->fallback_timeout = msckt;
}

WarGrey::STEM::SocketDaemon::~SocketDaemon() noexcept {
    if (this->master != nullptr) {
        SDLNet_FreeSocketSet(this->master);
        this->master = nullptr;
    }

    if (this->wrpl != nullptr) {
        this->wrpl->join();
        delete this->wrpl;
    }
}

/*************************************************************************************************/
bool WarGrey::STEM::SocketDaemon::udp_listen(uint16_t port, int packet_max_size) {
    if (this->udp_deamons.find(port) == this->udp_deamons.end()) {
        UDPsocket udp = SDLNet_UDP_Open(port);
        IPaddress addrv4;

        addrv4.host = INADDR_ANY;
        addrv4.port = port;

        if (udp == nullptr) {
            fprintf(stderr, "Error in creating UDP Socket: %s!\n", SDLNet_GetError());
        } else {
            int useless = SDLNet_UDP_Bind(udp, -1, &addrv4);

            if (useless == -1) {
                fprintf(stderr, "Error in binding UDP Address: %s!\n", SDLNet_GetError());
            }
            
            if (SDLNet_UDP_AddSocket(this->master, udp) == -1) {                
                fprintf(stderr, "Error in watching UDP socket: %s!\n", SDLNet_GetError());
                SDLNet_UDP_Close(udp);
            } else {
                this->udp_deamons[port] = udp;
            }
        }
    }

    this->udp_packet_size = std::max(this->udp_packet_size, packet_max_size);

    return (this->udp_deamons.find(port) != this->udp_deamons.end());
}

/*************************************************************************************************/
void WarGrey::STEM::SocketDaemon::start_wait_read_process_loop(int timeout_ms) {
    if (this->wrpl == nullptr) {
        this->wrpl = new std::thread(&SocketDaemon::wait_read_process_loop, this, timeout_ms);
    }
}

void WarGrey::STEM::SocketDaemon::wait_read_process_loop(int timeout_ms) {
    UserDatagram packet(this->udp_packet_size);
    int timeout = (timeout_ms <= 0) ? this->fallback_timeout : timeout_ms;
    int ready = 0;
    
    while (this->master != nullptr) {
        // it's efficient than thread sleeping
        ready = SDLNet_CheckSockets(this->master, timeout);

        if (ready > 0) {
            if (packet.capacity() < this->udp_packet_size) {
                packet.resize(this->udp_packet_size);
            }
            
            for (auto it = this->udp_deamons.begin(); it != this->udp_deamons.end(); ++ it) {
                if (SDLNet_SocketReady(it->second)) {
                    if (packet.recv(it->second)) {
                        printf("[%s:%d] says: %s\n",
                            packet.hostname(), packet.port(),
                            packet.message().c_str());
                    }

                    ready -= 1;
                    if (ready == 0) {
                        break;
                    }
                }
            }
        } else if (ready < 0) {
            fprintf(stderr, "Error in polling socket activities: %s!\n", SDLNet_GetError());
            perror("WaitReadProcessLoop");
        }
    }
}
