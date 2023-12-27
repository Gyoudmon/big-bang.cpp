#pragma once

#include <string>
#include <vector>
#include <cstdarg>

namespace GYDM {
#define VSNPRINT(retval, fmt) \
    const int DEFAULT_POOL_SIZE = 1024; \
    char chpool[DEFAULT_POOL_SIZE]; \
    int bigSize = DEFAULT_POOL_SIZE - 1; \
    char* pool; \
    va_list argl; \
    do { \
	pool = (bigSize < DEFAULT_POOL_SIZE) ? chpool : (new char[bigSize + 1]); \
    	va_start(argl, fmt); \
    	int status = vsnprintf(pool, bigSize + 1, fmt, argl); \
	va_end(argl); \
        if ((status == -1) || (status > bigSize)) { \
	    bigSize = ((status > 0) ? status : (bigSize * 2)) + 1; \
	    if (pool != chpool) delete[] pool; \
	    pool = nullptr; \
	} \
    } while (pool == nullptr); \
    std::string retval(pool); \
    if (pool != chpool) delete[] pool;

    /************************************************************************************************/
    __lambda__ std::string substring(const std::string& src, int start, int endplus1 = -1);
    __lambda__ std::string flstring(double flonum, int precision);
    __lambda__ std::string fxstring(long long fixnum, int width);
    __lambda__ std::string sstring(unsigned long long bytes, int precision);

    __lambda__ long long string_to_fixnum(const std::string& string);

    __lambda__ std::string make_nstring(const char* fmt, ...);
    __lambda__ std::string string_first_line(const std::string& src);
    __lambda__ std::vector<std::string> string_lines(const std::string& src, bool skip_empty_line = false);

    /************************************************************************************************/
    __lambda__ std::string binumber(unsigned long long n, size_t bitsize = 0);
    __lambda__ std::string hexnumber(unsigned long long n, size_t bytecount = 0);

    __lambda__ unsigned long long scan_natural(const char* src, size_t* pos, size_t end, bool skip_trailing_space = true);
    __lambda__ long long scan_integer(const char* src, size_t* pos, size_t end, bool skip_trailing_space = true);
    __lambda__ double scan_flonum(const char* src, size_t* pos, size_t end, bool skip_trailing_space = true);
    __lambda__ std::string scan_string(const char* src, size_t* pos, size_t end, char delimiter = ' ', bool skip_trailing_space = true);
    __lambda__ void scan_bytes(const char* src, size_t* pos, size_t end, char* bs, size_t bs_start, size_t bs_end, bool terminating = true);
    
    __lambda__ size_t scan_skip_token(const char* src, size_t* pos, size_t end, bool skip_trailing_space = true);
    __lambda__ size_t scan_skip_space(const char* src, size_t* pos, size_t end);
    __lambda__ size_t scan_skip_delimiter(const char* src, size_t* pos, size_t end, char delim = ',', bool skip_trailing_space = true);
    __lambda__ size_t scan_skip_newline(const char* src, size_t* pos, size_t end);
    __lambda__ size_t scan_skip_this_line(const char* src, size_t* pos, size_t end);

    template<typename B, size_t N>
    void scan_bytes(const char* src, size_t* pos, size_t end, B (&bs)[N], size_t start = 0U, bool terminating = true) {
        scan_bytes(src, pos, end, reinterpret_cast<char*>(bs), 0, N, terminating);
    }

    /************************************************************************************************/
    __lambda__ size_t string_utf8_length(const char* src, int max = -1);
    __lambda__ size_t string_utf8_length(const std::string& src);
    __lambda__ int string_utf8_index(const char* src, int idx, int max = -1);
    __lambda__ int string_utf8_index(const std::string& src, int idx);
    __lambda__ uint32_t string_utf8_ref(const char* src, int char_idx, int max = -1);
    __lambda__ uint32_t string_utf8_ref(const std::string& src, int char_idx);
    __lambda__ size_t string_character_size(const char* src, int byte_idx);
    __lambda__ size_t string_character_size(const std::string& src, int byte_idx);
    __lambda__ bool string_popback_utf8_char(std::string& src);
    __lambda__ std::string string_add_between(const char* s, char ch = '\n');

    /************************************************************************************************/
    __lambda__ bool string_equal(const char* s1, const char* s2);
    __lambda__ bool string_equal(const std::string& s1, const char* s2);
    __lambda__ bool string_equal(const std::string& s1, const std::string& s2);
    __lambda__ bool string_prefix(const char* src, const char* sub, int max = -1);
    __lambda__ bool string_prefix(const std::string& src, const char* sub);
    __lambda__ bool string_prefix(const std::string& src, const std::string& sub);
    __lambda__ bool string_suffix(const char* src, const char* sub, int max = -1);
    __lambda__ bool string_suffix(const std::string& src, const char* sub);
    __lambda__ bool string_suffix(const std::string& src, const std::string& sub);
    
    __lambda__ bool string_ci_equal(const char* s1, const char* s2);
    __lambda__ bool string_ci_equal(const std::string& s1, const char* s2);
    __lambda__ bool string_ci_equal(const std::string& s1, const std::string& s2);
    __lambda__ bool string_ci_prefix(const char* src, const char* sub, int max = -1);
    __lambda__ bool string_ci_prefix(const std::string& src, const char* sub);
    __lambda__ bool string_ci_prefix(const std::string& src, const std::string& sub);
    __lambda__ bool string_ci_suffix(const char* src, const char* sub, int max = -1);
    __lambda__ bool string_ci_suffix(const std::string& src, const char* sub);
    __lambda__ bool string_ci_suffix(const std::string& src, const std::string& sub);
}
