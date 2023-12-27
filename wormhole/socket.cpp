#include "socket.hpp"

#include "slang/datagram.hpp"
#include "slang/network.hpp"

using namespace GYDM;

/*************************************************************************************************/
GYDM::SocketDaemon::SocketDaemon(int maxsockets) {
    int msckt = (maxsockets <= 0) ? 1 : maxsockets;

    network_initialize();
    this->master = SDLNet_AllocSocketSet(msckt);
    this->fallback_timeout = msckt;
}

GYDM::SocketDaemon::~SocketDaemon() noexcept {
    SDLNet_SocketSet shadow = this->master;

    if (this->wrpl != nullptr) {
        this->master = nullptr;
        this->wrpl->join();
        delete this->wrpl;
    }

    if (shadow != nullptr) {
        for (auto it : this->udp_deamons) {
            it.second->unregister_from(shadow);
        }

        SDLNet_FreeSocketSet(shadow);
    }

    this->udp_deamons.clear();
}

/*************************************************************************************************/
bool GYDM::SocketDaemon::udp_listen(IUDPDaemon* daemon) {
    uint16_t service = daemon->service();

    if (this->udp_deamons.find(service) == this->udp_deamons.end()) {
        shared_udp_daemon_t udp(daemon);
        
        if (udp->okay()) {
            if (udp->register_to(this->master)) {
                this->udp_deamons[service] = udp;
            } else {
                fprintf(stderr, "Error in watching UDP socket: %s!\n", SDLNet_GetError());
            }
        } else {
            fprintf(stderr, "Error in creating UDP Socket: %s!\n", SDLNet_GetError());
        }
    }

    return (this->udp_deamons.find(service) != this->udp_deamons.end());
}

/*************************************************************************************************/
void GYDM::SocketDaemon::start_wait_read_process_loop(int timeout_ms) {
    if (this->wrpl == nullptr) {
        this->wrpl = new std::thread(&SocketDaemon::wait_read_process_loop, this, timeout_ms);
    }
}

void GYDM::SocketDaemon::wait_read_process_loop(int timeout_ms) {
    int timeout = (timeout_ms <= 0) ? this->fallback_timeout : timeout_ms;
    int ready = 0;
    
    while (this->master != nullptr) {
        // it's efficient than sleeping thread
        ready = SDLNet_CheckSockets(this->master, timeout);

        if (ready > 0) {
            for (auto it : this->udp_deamons) {
                if (it.second->ready()) {
                    if (it.second->recv_packet()) {
                        it.second->dispatch_packet();
                    }
                }
            }
        } else if (ready < 0) {
            perror("WaitReadProcessLoop");
        }
    }
}
