#pragma once

#include <string>
#include <vector>
#include <cstdarg>

namespace WarGrey::STEM {
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
    std::string substring(std::string& src, int start, int endplus1 = -1);
    std::string flstring(double flonum, int precision);
    std::string fxstring(long long fixnum, int width);
    std::string sstring(unsigned long long bytes, int precision);
    std::string gpstring(double position, char suffix, int precision = 3);

    long long string_to_fixnum(std::string& string);

    std::string make_nstring(const char* fmt, ...);
    std::string string_first_line(std::string& src);
    std::vector<std::string> string_lines(std::string& src, bool skip_empty_line = false);

    /************************************************************************************************/
    std::string binumber(unsigned long long n, size_t bitsize = 0);
    std::string hexnumber(unsigned long long n, size_t bytecount = 0);

    unsigned long long scan_natural(const char* src, size_t* pos, size_t end, bool skip_trailing_space = true);
    long long scan_integer(const char* src, size_t* pos, size_t end, bool skip_trailing_space = true);
    double scan_flonum(const char* src, size_t* pos, size_t end, bool skip_trailing_space = true);
    void scan_bytes(const char* src, size_t* pos, size_t end, char* bs, size_t bs_start, size_t bs_end, bool terminating = true);
    
    size_t scan_skip_token(const char* src, size_t* pos, size_t end, bool skip_trailing_space = true);
    size_t scan_skip_space(const char* src, size_t* pos, size_t end);
    size_t scan_skip_newline(const char* src, size_t* pos, size_t end);
    size_t scan_skip_this_line(const char* src, size_t* pos, size_t end);

    template<typename B, size_t N>
    void scan_bytes(const char* src, size_t* pos, size_t end, B (&bs)[N], size_t start = 0U, bool terminating = true) {
        scan_bytes(src, pos, end, reinterpret_cast<char*>(bs), 0, N, terminating);
    }

    /************************************************************************************************/
    bool string_popback_utf8_char(std::string& src);
}

