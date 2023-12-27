#pragma once

#include <cstddef>
#include <cinttypes>

namespace GYDM {
	__lambda__ bool is_slang_message(const uint8_t* message, size_t idx = 0);

	__lambda__ bool slang_message_validate(const uint8_t* message, size_t size, size_t idx = 0);
	__lambda__ size_t slang_metainfo_unbox(const uint8_t* message, uint8_t* version, uint8_t* type, size_t idx = 0);
}
