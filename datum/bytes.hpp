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
}
