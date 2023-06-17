#pragma once

#include <string>

namespace WarGrey::STEM {
    /*********************************************************************************************/
#define _B(lit_string) reinterpret_cast<const unsigned char*>(lit_string)
#define _b(lit_string) reinterpret_cast<unsigned char*>(lit_string)

    typedef std::basic_string<unsigned char> bytes;

    /*********************************************************************************************/
    char byte_to_hexadecimal(char ch, char fallback_value);
    char hexadecimal_to_byte(char ch, char ten = 'A');
    char byte_to_decimal(char ch, char fallback_value);
    char decimal_to_byte(char ch);
    char byte_to_octal(char ch, char fallback_value);
    char octal_to_byte(char ch);

    char hexadecimal_ref(const char* src, size_t idx, char fallback_value);
    void hexadecimal_set(char* src, size_t idx, char hex);
    char decimal_ref(const char* src, size_t idx, char fallback_value);
    void decimal_set(char* src, size_t idx, char dec);

    /*********************************************************************************************/
    uint8_t network_uint8_ref(const uint8_t* src, size_t idx);
	void network_uint8_set(uint8_t* dest, size_t idx, uint8_t x);

	uint16_t network_uint16_ref(const uint8_t* src, size_t idx);
	void network_uint16_set(uint8_t* dest, size_t idx, uint16_t x);

	uint32_t network_uint32_ref(const uint8_t* src, size_t idx);
	void network_uint32_set(uint8_t* dest, size_t idx, uint32_t x);

	uint64_t network_uint64_ref(const uint8_t* src, size_t idx);
	void network_uint64_set(uint8_t* dest, size_t idx, uint64_t x);

	float network_float_ref(const uint8_t* src, size_t idx);
	void network_float_set(uint8_t* dest, size_t idx, float x);

	float network_flword_ref(const uint8_t* src, size_t idx, float scale = 1.0F);
	void network_flword_set(uint8_t* dest, size_t idx, float x, float scale = 1.0F);

	void read_network_floats(uint8_t* src, size_t address, size_t quantity, float* dest);
}
