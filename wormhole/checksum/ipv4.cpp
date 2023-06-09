#include "ipv4.hpp"

//// https://tools.ietf.org/html/rfc1071

using namespace WarGrey::STEM;

/*************************************************************************************************/
static uint16_t update_sum(uint16_t sum, const uint8_t* message, size_t start, size_t end) {
    size_t count = end - start;
    uint16_t H = (sum >> 8U);
    uint16_t L = (sum & 0xFFU);

    if ((count & 0x01) == 1) {
        H += message[--end];
    }

    for (size_t idx = start; idx < end; idx += 2) {
        H += message[idx];
        L += message[idx + 1];
    }

    while ((H > 0xFF) || (L > 0xFF)) {
        uint16_t Hcarry = H >> 8U;
        uint16_t Lcarry = L >> 8U;

        H = (H & 0xFF) + Lcarry;
        L = (L & 0xFF) + Hcarry;
    }

    return ~((H << 8U) ^ L);
}

/*************************************************************************************************/
uint16_t WarGrey::STEM::checksum_ipv4(const uint8_t* message, size_t start, size_t end) {
    return update_sum(0xFFFFL, message, start, end);
}

uint16_t WarGrey::STEM::checksum_ipv4(uint16_t acc_crc, const uint8_t* message, size_t start, size_t end) {
    return update_sum(~acc_crc, message, start, end);
}

uint16_t WarGrey::STEM::checksum_ipv4(uint16_t* acc_crc, const uint8_t* message, size_t start, size_t end) {
    uint16_t sum = 0UL;

    if (acc_crc == nullptr) {
        sum = checksum_ipv4(message, start, end);
    } else {
        sum = update_sum(~(*acc_crc), message, start, end);
        (*acc_crc) = sum;
    }

    return sum;
}
