#pragma once

#include <cstdint>
#include <cstddef>

namespace WarGrey::STEM {
	uint16_t checksum_ipv4(const uint8_t* message, size_t start, size_t end);
	uint16_t checksum_ipv4(uint16_t accumulated_crc, const uint8_t* message, size_t start, size_t end);
	uint16_t checksum_ipv4(uint16_t* accumulated_crc, const uint8_t* message, size_t start, size_t end);

	template<typename B, size_t N>
	uint16_t checksum_ipv4(const B(&message)[N], size_t start = 0) {
		return checksum_ipv4(reinterpret_cast<const uint8_t*>(message), start, N - 1);
	}

	template<typename B, size_t N>
	uint16_t checksum_ipv4(uint16_t accumulated_crc, const B(&message)[N], size_t start = 0) {
		return checksum_ipv4(accumulated_crc, reinterpret_cast<const uint8_t*>(message), start, N - 1);
	}

	template<typename B, size_t N>
	uint16_t checksum_ipv4(uint16_t* accumulated_crc, const B(&message)[N], size_t start = 0) {
		return checksum_ipv4(accumulated_crc, reinterpret_cast<const uint8_t*>(message), start, N - 1);
	}
}
