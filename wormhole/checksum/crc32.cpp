#include "crc32.hpp"

using namespace Plteen;

/*************************************************************************************************/
static uint32_t crc_table[256];
static int crc_table_computed = 0;   

static void make_crc_table() {
    for (uint32_t n = 0; n < 256; n++) {
        uint32_t c = n;
    
        for (size_t k = 0; k < 8; k++) {
            if (c & 1) {
                c = 0xEDB88320L ^ (c >> 1);
            } else {
                c = c >> 1;
            }
        }
        
        crc_table[n] = c;
    }

    crc_table_computed = 1;
}

static uint32_t update_crc(uint32_t crc, const uint8_t* message, size_t start, size_t end) {
    uint32_t c = crc;

    /**
     * Update a running CRC with the bytes message[start...end-1]
     *
     * The CRC should be initialized to all 1's, and the transmitted value
     *   is the 1's complement of the final running CRC.
     */

    if (!crc_table_computed) {
        make_crc_table();
    }

    for (size_t idx = start; idx < end; idx++) {
        c = crc_table[(c ^ message[idx]) & 0xFFU] ^ (c >> 8);
    }

    return c;
}

/*************************************************************************************************/
uint32_t Plteen::checksum_crc32(const uint8_t* message, size_t start, size_t end) {
    return update_crc(0xFFFFFFFFL, message, start, end) ^ 0xFFFFFFFFL;
}

uint32_t Plteen::checksum_crc32(uint32_t acc_crc, const uint8_t* message, size_t start, size_t end) {
    return update_crc(acc_crc ^ 0xFFFFFFFF, message, start, end) ^ 0xFFFFFFFFL;
}

uint32_t Plteen::checksum_crc32(uint32_t* acc_crc, const uint8_t* message, size_t start, size_t end) {
    uint32_t crc = 0UL;

    if (acc_crc == nullptr) {
        crc = checksum_crc32(message, start, end);
    } else {
        crc = update_crc((*acc_crc) ^ 0xFFFFFFFF, message, start, end) ^ 0xFFFFFFFFL;
        (*acc_crc) = crc;
    }

    return crc;
}
