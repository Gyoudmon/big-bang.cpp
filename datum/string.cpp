#include "string.hpp"
#include "flonum.hpp"
#include "fixnum.hpp"
#include "bytes.hpp"
#include "char.hpp"

#include <cstdlib>

using namespace WarGrey::STEM;

/*************************************************************************************************/
static size_t newline_position(const char* src, size_t idx0, size_t idxn, size_t* next_idx) {
    size_t line_size = 0;
    size_t eol_size = 0;

    for (size_t idx = idx0; idx < idxn; idx ++) {
        if (src[idx] == linefeed) {
            eol_size = (((idx + 1) < idxn) && (src[idx + 1] == carriage_return)) ? 2 : 1;
            break;
        } else if (src[idx] == carriage_return) {
            eol_size = (((idx + 1) < idxn) && (src[idx + 1] == linefeed)) ? 2 : 1;
            break;
        }

        line_size ++;
    }

    (*next_idx) = idx0 + line_size + eol_size;

    return line_size;
}

/*************************************************************************************************/
std::string WarGrey::STEM::flstring(double value, int precision) {
    return ((precision >= 0)
        ? make_nstring(make_nstring("%%.%dlf", precision).c_str(), value)
        : make_nstring("%lf", value));
}

std::string WarGrey::STEM::fxstring(long long value, int width) {
    return ((width > 0)
        ? make_nstring(make_nstring("%%0%dld", width).c_str(), value)
        : make_nstring("%lld", value));
}

std::string WarGrey::STEM::sstring(unsigned long long bytes, int precision) {
    static const char* units[] = { "KB", "MB", "GB", "TB" };
    static unsigned int max_idx = sizeof(units) / sizeof(char*) - 1;
    std::string size = make_nstring("%llu", bytes);

    if (bytes >= 1024) {
        double flsize = double(bytes) / 1024.0;
        unsigned idx = 0;

        while ((flsize >= 1024.0) && (idx < max_idx)) {
            flsize /= 1024.0;
            idx++;
        }

        size = flstring(flsize, precision) + units[idx];
    }

    return size;
}

/*************************************************************************************************/
std::string WarGrey::STEM::substring(std::string& src, int start, int endplus1) {
    std::string substr;
    size_t max_size = src.size();
    size_t subsize = ((endplus1 > 0) ? fxmin(static_cast<size_t>(endplus1), max_size) : max_size) - start;

    if (subsize > 0) {
        substr = src.substr(start, subsize);
    }

    return substr;
}

std::string WarGrey::STEM::make_nstring(const char* fmt, ...) {
    VSNPRINT(s, fmt);

    return s;
}

std::string WarGrey::STEM::binumber(unsigned long long n, size_t bitsize) {
    size_t size = ((bitsize < 1) ? ((n == 0) ? 1 : integer_length(n)) : bitsize);
    std::string bs(size, '0');

    for (size_t idx = size; idx > 0; idx--) {
        bs[idx - 1] = (((n >> (size - idx)) & 0b1) ? '1' : '0');
    }

    return bs;
}

std::string WarGrey::STEM::hexnumber(unsigned long long n, size_t bytecount) {
    size_t isize = integer_length(n);
    size_t size = ((bytecount < 1) ? ((n == 0) ? 1 : (isize / 8 + ((isize % 8 == 0) ? 0 : 1))) : bytecount) * 2;
    std::string bs(size, '0');

    for (size_t idx = size; idx > 0; idx --) {
        bs[idx - 1] = hexadecimal_to_byte(n & 0xFU);
        n >>= 4U;
    }

    return bs;
}

/**************************************************************************************************/
long long WarGrey::STEM::string_to_fixnum(std::string& string) {
    return atoll(string.c_str());
}

/**************************************************************************************************/
std::string WarGrey::STEM::string_first_line(std::string& src) {
    const char* raw_src = src.c_str();
    size_t total = src.size();
    size_t line_size = newline_position(raw_src, 0, total, &total);
    
    return std::string(raw_src, line_size);
}

std::vector<std::string> WarGrey::STEM::string_lines(std::string& src, bool skip_empty_line) {
    std::vector<std::string> lines;
    const char* raw_src = src.c_str();
    size_t total = src.size();
    size_t nidx = 0;

    while (total > 0) {
        size_t line_size = newline_position(raw_src, 0, total, &nidx);

        if ((line_size > 0) || (!skip_empty_line)) {
            lines.push_back(std::string(raw_src, line_size));
        }

        raw_src += nidx;
        total -= nidx;
    }

    return lines;
}

/************************************************************************************************/
unsigned long long WarGrey::STEM::scan_natural(const char* src, size_t* pos, size_t end, bool skip_trailing_space) {
    unsigned long long value = 0;

    while ((*pos) < end) {
        char c = src[(*pos)];

        if ((c < zero) || (c > nine)) {
            break;
        }

        value = value * 10 + (c - zero);
        (*pos) += 1;
    }

    if (skip_trailing_space) {
        scan_skip_space(src, pos, end);
    }

    return value;
}

long long WarGrey::STEM::scan_integer(const char* src, size_t* pos, size_t end, bool skip_trailing_space) {
    int sign = 1;
    long long value = 0;

    if ((*pos) < end) {
        if (src[(*pos)] == minus) {
            sign = -1;
            (*pos) += 1;
        } else if (src[(*pos)] == plus) {
            (*pos) += 1;
        }
    }

    while ((*pos) < end) {
        char c = src[(*pos)];

        if ((c < zero) || (c > nine)) {
            break;
        }

        value = value * 10 + (c - zero);
        (*pos) += 1;
    }

    if (skip_trailing_space) {
        scan_skip_space(src, pos, end);
    }

    return value * sign;
}

double WarGrey::STEM::scan_flonum(const char* src, size_t* pos, size_t end, bool skip_trailing_space) {
    double value = flnan;
    double i_acc = 10.0;
    double f_acc = 1.0;
    double sign = 1.0;

    if ((*pos) < end) {
        if (src[(*pos)] == minus) {
            sign = -1.0;
            (*pos) += 1;
        } else if (src[(*pos)] == plus) {
            (*pos) += 1;
        }
    }

    while ((*pos) < end) {
        char ch = src[(*pos)];

        (*pos) += 1;

        if ((ch < zero) || (ch > nine)) {
            // TODO: deal with scientific notation
            if ((ch == dot) && (f_acc == 1.0)) {
                i_acc = 1.0;
                f_acc = 0.1;
                continue;
            } else {
                (*pos) -= 1;
                break;
            }
        }

        if (std::isnan(value)) {
            value = 0.0;
        }

        value = value * i_acc + double(ch - zero) * f_acc;

        if (f_acc != 1.0) {
            f_acc *= 0.1;
        }
    }

    if (skip_trailing_space) {
        scan_skip_space(src, pos, end);
    }

    return value * sign;
}

void WarGrey::STEM::scan_bytes(const char* src, size_t* pos, size_t end, char* bs, size_t bs_start, size_t bs_end, bool terminating) {
    size_t bsize = bs_end - bs_start;
    size_t size = fxmin(end - (*pos), bsize);

    if (terminating) {
        if (size == bsize) {
            size--;
        }

        bs[bs_start + size] = '\0';
    }

    if (size > 0) {
        memcpy(bs, &src[(*pos)], size);
    }

    (*pos) += size;
}

size_t WarGrey::STEM::scan_skip_token(const char* src, size_t* pos, size_t end, bool skip_trailing_space) {
    size_t idx = (*pos);

    while ((*pos) < end) {
        char c = src[(*pos)];

        if (c == space) {
            break;
        }

        (*pos) += 1;
    }

    if (skip_trailing_space) {
        scan_skip_space(src, pos, end);
    }

    return (*pos) - idx;
}

size_t WarGrey::STEM::scan_skip_space(const char* src, size_t* pos, size_t end) {
    size_t idx = (*pos);

    while ((*pos) < end) {
        char c = src[(*pos)];

        if (c != space) {
            break;
        }

        (*pos) += 1;
    }

    return (*pos) - idx;
}

size_t WarGrey::STEM::scan_skip_newline(const char* src, size_t* pos, size_t end) {
    size_t idx = (*pos);

    while ((*pos) < end) {
        char c = src[(*pos)];

        if ((c != linefeed) && (c != carriage_return)) {
            break;
        }

        (*pos) += 1;
    }

    return (*pos) - idx;
}

size_t WarGrey::STEM::scan_skip_this_line(const char* src, size_t* pos, size_t end) {
    size_t idx = (*pos);

    while ((*pos) < end) {
        char c = src[(*pos)];

        if ((c == linefeed) || (c == carriage_return)) {
            scan_skip_newline(src, pos, end);
            break;
        }

        (*pos) += 1;
    }

    return (*pos) - idx;
}

/************************************************************************************************/
bool WarGrey::STEM::string_popback_utf8_char(std::string& src) {
    size_t size = src.size();
    bool okay = false;

    if (size > 0) {
        const unsigned char* text = reinterpret_cast<const unsigned char*>(src.c_str());
        
        /**
         * UTF-8 encodes characters in 1 to 4 bytes, and their binary forms are:
         *   0xxx xxxx
         *   110x xxxx  10xx xxxx
         *   1110 xxxx  10xx xxxx  10xx xxxx
         *   1111 xxxx  10xx xxxx  10xx xxxx  10xx xxxx
         */
        
        if (text[size - 1] < 0b10000000U) {
            src.pop_back();
        } else {
            size -= 2;
            while (text[size] < 0b11000000U) size--;
            src.erase(size);
        }

        okay = true;
    }

    return okay;
}

