#include "bytes.hpp"
#include "flonum.hpp"

using namespace GYDM;

/*************************************************************************************************/
#define GET_INT16_FROM_INT8(tab_int8, index) ((tab_int8[(index)] << 8) ^ tab_int8[(index) + 1])
#define SET_INT16_TO_INT8(tab_int8, index, value) \
    do { \
        tab_int8[(index)] = (value) >> 8;  \
        tab_int8[(index) + 1] = (value) & 0xFF; \
    } while (0)

/*************************************************************************************************/
char GYDM::byte_to_hexadecimal(char ch, char fallback_value) {
    if ((ch >= '0') && (ch <= '9')) {
        ch = ch - '0';
    } else if ((ch >= 'a') && (ch <= 'f')) {
        ch = ch - 'a' + 10;
    } else if ((ch >= 'A') && (ch <= 'F')) {
        ch = ch - 'A' + 10;
    } else {
        ch = fallback_value;
    }

    return ch;
}

char GYDM::byte_to_decimal(char ch, char fallback_value) {
    if ((ch >= '0') && (ch <= '9')) {
        ch = ch - '0';
    } else {
        ch = fallback_value;
    }

    return ch;
}

char GYDM::byte_to_octal(char ch, char fallback_value) {
    if ((ch >= '0') && (ch <= '7')) {
        ch = ch - '0';
    } else {
        ch = fallback_value;
    }

    return ch;
}

char GYDM::hexadecimal_to_byte(char ch, char ten) {
    return ((ch >= 10) ? (ch - 10 + ten) : (ch + '0'));
}

char GYDM::decimal_to_byte(char ch) {
    return (ch + '0');
}

char GYDM::octal_to_byte(char ch) {
    return (ch + '0');
}

char GYDM::hexadecimal_ref(const char* src, size_t idx, char fallback_value) {
    return byte_to_hexadecimal(src[idx], fallback_value);
}

void GYDM::hexadecimal_set(char* dest, size_t idx, char ch) {
    dest[idx] = hexadecimal_to_byte(ch);
}

char GYDM::decimal_ref(const char* src, size_t idx, char fallback_value) {
    return byte_to_decimal(src[idx], fallback_value);
}

void GYDM::decimal_set(char* dest, size_t idx, char ch) {
    dest[idx] = decimal_to_byte(ch);
}

/*************************************************************************************************/
uint8_t GYDM::network_uint8_ref(const uint8_t* src, size_t idx) {
	return src[idx];
}

void GYDM::network_uint8_set(uint8_t* dest, size_t idx, uint8_t x) {
	dest[idx] = x;
}

uint16_t GYDM::network_uint16_ref(const uint8_t* src, size_t idx) {
	return GET_INT16_FROM_INT8(src, idx);
}

void GYDM::network_uint16_set(uint8_t* dest, size_t idx, uint16_t x) {
	SET_INT16_TO_INT8(dest, idx, x);
}

uint32_t GYDM::network_uint32_ref(const uint8_t* src, size_t idx) {
	uint32_t msb16 = GET_INT16_FROM_INT8(src, idx);
	uint32_t lsb16 = GET_INT16_FROM_INT8(src, idx + 2);

	return (msb16 << 16) + lsb16;
}

void GYDM::network_uint32_set(uint8_t* dest, size_t idx, uint32_t x) {
	uint16_t msb16 = x >> 16;
	uint16_t lsb16 = x & 0xFFFF;

	SET_INT16_TO_INT8(dest, idx + 0, msb16);
	SET_INT16_TO_INT8(dest, idx + 2, lsb16);
}

uint64_t GYDM::network_uint64_ref(const uint8_t* src, size_t idx) {
	uint64_t msb32 = network_uint32_ref(src, idx);
	uint64_t lsb32 = network_uint32_ref(src, idx + 4);

	return (msb32 << 32) + lsb32;
}

void GYDM::network_uint64_set(uint8_t* dest, size_t idx, uint64_t x) {
	uint32_t msb32 = x >> 32;
	uint32_t lsb32 = x & 0xFFFFFFFF;

	network_uint32_set(dest, idx, msb32);
	network_uint32_set(dest, idx + 4, lsb32);
}

float GYDM::network_float_ref(const uint8_t* src, size_t idx) {
	uint8_t flbytes[4];
	float dest = 0.0F;

	flbytes[0] = src[idx + 3];
	flbytes[1] = src[idx + 2];
	flbytes[2] = src[idx + 1];
	flbytes[3] = src[idx + 0];

	memcpy((void*)&dest, (void*)flbytes, 4);

	return dest;
}

void GYDM::network_float_set(uint8_t* dest, size_t idx, float src) {
	uint8_t flbytes[4];

	memcpy((void*)flbytes, (void*)&src, 4);

	dest[idx + 3] = flbytes[0];
	dest[idx + 2] = flbytes[1];
	dest[idx + 1] = flbytes[2];
	dest[idx + 0] = flbytes[3];
}

float GYDM::network_flword_ref(const uint8_t* src, size_t idx, float scale) {
	uint16_t u16 = network_uint16_ref(src, idx);

	return float(u16) / scale;
}

void GYDM::network_flword_set(uint8_t* dest, size_t idx, float x, float scale) {
	uint16_t u16 = static_cast<uint16_t>(flround(x * scale));

	network_uint16_set(dest, idx, u16);
}

void GYDM::read_network_floats(uint8_t* src, size_t address, size_t quantity, float* dest) {
	for (size_t i = 0; i < quantity; i++) {
		dest[i] = network_float_ref(src, address + i * sizeof(float));
	}
}
