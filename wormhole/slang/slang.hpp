#pragma once

#include <cstddef>
#include <cinttypes>

namespace WarGrey::STEM {
	bool is_slang_message(const uint8_t* message, size_t idx = 0);

	bool slang_message_validate(const uint8_t* message, size_t size, size_t idx = 0);
	size_t slang_metainfo_unbox(const uint8_t* message, uint8_t* version, uint8_t* type, size_t idx = 0);
}
