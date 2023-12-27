#include "ipv4.hpp"

//// https://tools.ietf.org/html/rfc1071

using namespace GYDM;

/*************************************************************************************************/
static uint16_t update_sum(uint16_t sum, const uint8_t* message, size_t start, size_t end) {
    size_t count = end - start;
    uint32_t HL = sum;
    
    if ((count & 0x01) == 1) {
        HL += (message[--end] << 8U);
    }

    for (size_t idx = start; idx < end; idx += 2) {
        HL += ((message[idx] << 8U) ^ message[idx + 1]);
    }

    while (HL > 0xFFFFU) {
        HL = (HL & 0xFFFFU) + (HL >> 16U);
    }

    return ~(static_cast<uint16_t>(HL));
}

/*************************************************************************************************/
uint16_t GYDM::checksum_ipv4(const uint8_t* message, size_t start, size_t end) {
    return update_sum(0xFFFFL, message, start, end);
}

uint16_t GYDM::checksum_ipv4(uint16_t acc_crc, const uint8_t* message, size_t start, size_t end) {
    return update_sum(~acc_crc, message, start, end);
}

uint16_t GYDM::checksum_ipv4(uint16_t* acc_crc, const uint8_t* message, size_t start, size_t end) {
    uint16_t sum = 0UL;

    if (acc_crc == nullptr) {
        sum = checksum_ipv4(message, start, end);
    } else {
        sum = update_sum(~(*acc_crc), message, start, end);
        (*acc_crc) = sum;
    }

    return sum;
}
