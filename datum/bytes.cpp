#include "bytes.hpp"
#include "flonum.hpp"

using namespace WarGrey::STEM;

char WarGrey::STEM::byte_to_hexadecimal(char ch, char fallback_value) {
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

char WarGrey::STEM::byte_to_decimal(char ch, char fallback_value) {
    if ((ch >= '0') && (ch <= '9')) {
        ch = ch - '0';
    } else {
        ch = fallback_value;
    }

    return ch;
}

char WarGrey::STEM::byte_to_octal(char ch, char fallback_value) {
    if ((ch >= '0') && (ch <= '7')) {
        ch = ch - '0';
    } else {
        ch = fallback_value;
    }

    return ch;
}

char WarGrey::STEM::hexadecimal_to_byte(char ch) {
    return ((ch >= 10) ? (ch - 10 + 'A') : (ch + '0'));
}

char WarGrey::STEM::decimal_to_byte(char ch) {
    return (ch + '0');
}

char WarGrey::STEM::octal_to_byte(char ch) {
    return (ch + '0');
}

char WarGrey::STEM::hexadecimal_ref(const char* src, size_t idx, char fallback_value) {
    return byte_to_hexadecimal(src[idx], fallback_value);
}

void WarGrey::STEM::hexadecimal_set(char* dest, size_t idx, char ch) {
    dest[idx] = hexadecimal_to_byte(ch);
}

char WarGrey::STEM::decimal_ref(const char* src, size_t idx, char fallback_value) {
    return byte_to_decimal(src[idx], fallback_value);
}

void WarGrey::STEM::decimal_set(char* dest, size_t idx, char ch) {
    dest[idx] = decimal_to_byte(ch);
}

