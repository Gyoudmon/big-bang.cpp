#pragma once

#include <cstdint>
#include <cstddef>

namespace WarGrey::STEM {
	uint32_t checksum_crc32(const uint8_t* message, size_t start, size_t end);
	uint32_t checksum_crc32(uint32_t accumulated_crc, const uint8_t* message, size_t start, size_t end);
	uint32_t checksum_crc32(uint32_t* accumulated_crc, const uint8_t* message, size_t start, size_t end);

	template<typename B, size_t N>
	uint32_t checksum_crc32(const B(&message)[N], size_t start = 0) {
		return checksum_crc32(reinterpret_cast<const uint8_t*>(message), start, N - 1);
	}

	template<typename B, size_t N>
	uint32_t checksum_crc32(uint32_t accumulated_crc, const B(&message)[N], size_t start = 0) {
		return checksum_crc32(accumulated_crc, reinterpret_cast<const uint8_t*>(message), start, N - 1);
	}

	template<typename B, size_t N>
	uint32_t checksum_crc32(uint32_t* accumulated_crc, const B(&message)[N], size_t start = 0) {
		return checksum_crc32(accumulated_crc, reinterpret_cast<const uint8_t*>(message), start, N - 1);
	}
}
