#include "datagram.hpp"
#include "slang.hpp"

#include "../checksum/ipv4.hpp"

#include "../../datum/box.hpp"
#include "../../datum/bytes.hpp"

using namespace GYDM;

/*************************************************************************************************/
GYDM::UserDatagramPacket::UserDatagramPacket(size_t size) {
    this->self = SDLNet_AllocPacket(int(size));
}

GYDM::UserDatagramPacket::~UserDatagramPacket() noexcept {
    if (this->self != nullptr) {
        SDLNet_FreePacket(this->self);
        this->self = nullptr;
    }
}

size_t GYDM::UserDatagramPacket::capacity() {
    return static_cast<size_t>(this->self->maxlen);
}

size_t GYDM::UserDatagramPacket::resize(size_t new_size) {
    return static_cast<size_t>(SDLNet_ResizePacket(this->self, int(new_size)));
}

const char* GYDM::UserDatagramPacket::hostname() {
    return SDLNet_ResolveIP(&this->self->address);
}

uint16_t GYDM::UserDatagramPacket::port() {
    return this->self->address.port;
}

const unsigned char* GYDM::UserDatagramPacket::unbox(uint8_t* type, uint16_t* transaction, uint16_t* response_port, size_t* size) {
    unsigned char* payload = nullptr;
    
    if (is_slang_message(this->self->data)) {
        if (slang_message_validate(this->self->data, this->self->len)) {
            uint8_t version = 0U;
            size_t cursor = slang_metainfo_unbox(this->self->data, &version, type);

            switch (version) {
	        case 1: {
		        SET_BOX(transaction, network_uint16_ref(this->self->data, cursor));
		        cursor += 2;
		        SET_BOX(response_port, network_uint16_ref(this->self->data, cursor));
		        cursor += 2;
	        }; break;
            default: SET_BOXES(transaction, response_port, 0U);
	        }

            payload = this->self->data + cursor;
            SET_BOX(size, static_cast<size_t>(this->self->len) - cursor);
        } else {
        	fprintf(stderr, "%s:%d: slang message has been modified. [CHECKSUM: %x]\n",
                    this->hostname(), this->port(), checksum_ipv4(this->self->data, 0, this->self->len));
		}
    } else {
        fprintf(stderr, "%s:%d: not a slang message, ignored.\n", this->hostname(), this->port());
    }

    if (payload == nullptr) {
        SET_BOX(size, 0U);
    }

    return payload;
}

int GYDM::UserDatagramPacket::recv(UDPsocket udp) {
    int rsize = -1;

    if (SDLNet_UDP_Recv(udp, this->self) == 1) {
        rsize = this->self->len;
    }

    return rsize;
}
