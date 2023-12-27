#include "string.hpp"
#include "flonum.hpp"
#include "fixnum.hpp"
#include "bytes.hpp"
#include "char.hpp"

#include <cstdlib>

using namespace GYDM;

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

static bool string_equal_cs(const char* s1, const char* s2, size_t sz1, size_t sz2) {
    bool yes = true;
    
    if (sz1 == sz2) {
        for (size_t i = 0; i < sz1; i ++) {
            if (s1[i] != s2[i]) {
                yes = false;
                break;
            }
        }
    } else {
        yes = false;
    }

    return yes;
}

static bool string_equal_ci(const char* s1, const char* s2, size_t sz1, size_t sz2) {
    bool yes = true;
    
    if (sz1 == sz2) {
        for (size_t i = 0; i < sz1; i ++) {
            if (!char_ci_eq(s1[i], s2[i])) {
                yes = false;
                break;
            }
        }
    } else {
        yes = false;
    }

    return yes;
}

/*************************************************************************************************/
std::string GYDM::flstring(double value, int precision) {
    return ((precision >= 0)
        ? make_nstring(make_nstring("%%.%dlf", precision).c_str(), value)
        : make_nstring("%lf", value));
}

std::string GYDM::fxstring(long long value, int width) {
    return ((width > 0)
        ? make_nstring(make_nstring("%%0%dld", width).c_str(), value)
        : make_nstring("%lld", value));
}

std::string GYDM::sstring(unsigned long long bytes, int precision) {
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
std::string GYDM::substring(const std::string& src, int start, int endplus1) {
    std::string substr;
    size_t max_size = src.size();
    size_t subsize = ((endplus1 > 0) ? fxmin(static_cast<size_t>(endplus1), max_size) : max_size) - start;

    if (subsize > 0) {
        substr = src.substr(start, subsize);
    }

    return substr;
}

std::string GYDM::make_nstring(const char* fmt, ...) {
    VSNPRINT(s, fmt);

    return s;
}

std::string GYDM::binumber(unsigned long long n, size_t bitsize) {
    size_t size = ((bitsize < 1) ? ((n == 0) ? 1 : integer_length(n)) : bitsize);
    std::string bs(size, '0');

    for (size_t idx = size; idx > 0; idx--) {
        bs[idx - 1] = (((n >> (size - idx)) & 0b1) ? '1' : '0');
    }

    return bs;
}

std::string GYDM::hexnumber(unsigned long long n, size_t bytecount) {
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
long long GYDM::string_to_fixnum(const std::string& string) {
    return std::atoll(string.c_str());
}

/**************************************************************************************************/
std::string GYDM::string_first_line(const std::string& src) {
    const char* raw_src = src.c_str();
    size_t total = src.size();
    size_t line_size = newline_position(raw_src, 0, total, &total);
    
    return std::string(raw_src, line_size);
}

std::vector<std::string> GYDM::string_lines(const std::string& src, bool skip_empty_line) {
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
unsigned long long GYDM::scan_natural(const char* src, size_t* pos, size_t end, bool skip_trailing_space) {
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

long long GYDM::scan_integer(const char* src, size_t* pos, size_t end, bool skip_trailing_space) {
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

double GYDM::scan_flonum(const char* src, size_t* pos, size_t end, bool skip_trailing_space) {
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

std::string GYDM::scan_string(const char* src, size_t* pos, size_t end, char delim, bool skip_trailing_space) {
    size_t idx = (*pos);
    size_t size = 0;
    std::string s;

    while ((*pos) < end) {
        char c = src[(*pos)];

        if (c == delim) {
            break;
        }

        (*pos) += 1;

        if (c != space) {
            size = (*pos) - idx;
        }
    }

    if (size > 0) {
        s = std::string(src).substr(idx, size);
    }

    if (skip_trailing_space) {
        scan_skip_space(src, pos, end);
    }

    return s;
}

void GYDM::scan_bytes(const char* src, size_t* pos, size_t end, char* bs, size_t bs_start, size_t bs_end, bool terminating) {
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

size_t GYDM::scan_skip_token(const char* src, size_t* pos, size_t end, bool skip_trailing_space) {
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

size_t GYDM::scan_skip_space(const char* src, size_t* pos, size_t end) {
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

size_t GYDM::scan_skip_delimiter(const char* src, size_t* pos, size_t end, char delim, bool skip_trailing_space) {
    size_t idx = (*pos);

    if (idx < end) {
        if (src[idx] == delim) {
            (*pos) += 1;

            if (skip_trailing_space) {
                scan_skip_space(src, pos, end);
            }
        }
    }

    return (*pos) - idx;
}

size_t GYDM::scan_skip_newline(const char* src, size_t* pos, size_t end) {
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

size_t GYDM::scan_skip_this_line(const char* src, size_t* pos, size_t end) {
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
/**
 * UTF-8 encodes characters in 1 to 4 bytes, and their binary forms are:
 *   0xxx xxxx
 *   110x xxxx  10xx xxxx
 *   1110 xxxx  10xx xxxx  10xx xxxx
 *   1111 0xxx  10xx xxxx  10xx xxxx  10xx xxxx
 */

size_t GYDM::string_utf8_length(const char* src, int max0) {
    size_t max = (max0 >= 0) ? max0 : strlen(src);
    size_t idx = 0;
    size_t n = 0;

    while ((idx < max) && (src[idx] != '\0')) {
        unsigned char c = static_cast<unsigned char>(src[idx]);

        n++;

        if (c < 0b10000000U) {
            idx ++;
        } else if (c >= 0b11110000U) {
            idx += 4;
        } else if (c >= 0b11100000U) {
            idx += 3;
        } else {
            idx += 2;
        }
    }
    
    return n;
}

size_t GYDM::string_utf8_length(const std::string& src) {
    return string_utf8_length(src.c_str(), int(src.size()));
}

int GYDM::string_utf8_index(const char* src, int idx, int max0) {
    size_t max = (max0 >= 0) ? max0 : strlen(src);
    size_t it = 0;
    int cidx = -1;
    int n = 0;

    while ((it < max) && (src[it] != '\0')) {
        if (n < idx) {
            unsigned char c = static_cast<unsigned char>(src[it]);

            if (c < 0b10000000U) {
                it ++;
            } else if (c >= 0b11110000U) {
                it += 4;
            } else if (c >= 0b11100000U) {
                it += 3;
            } else {
                it += 2;
            }

            n++;
        } else {
            cidx = int(it);
            break;
        }
    }
    
    return cidx;
}

int GYDM::string_utf8_index(const std::string& src, int char_idx) {
    return string_utf8_index(src.c_str(), char_idx, int(src.size()));
}

uint32_t GYDM::string_utf8_ref(const char* src, int idx, int max0) {
    size_t max = (max0 >= 0) ? max0 : strlen(src);
    size_t codepoint = 0;
    int code_idx = string_utf8_index(src, idx, max0);

    if (code_idx >= 0) {
        unsigned char c = static_cast<unsigned char>(src[code_idx]);

        if (c < 0b10000000U) {
            codepoint = c;
        } else if (c >= 0b11110000U) {
            if (code_idx + 3 < max) {
                size_t b1 = c & 0b00000111U;
                size_t b2 = static_cast<unsigned char>(src[code_idx + 1]) & 0b00111111U;
                size_t b3 = static_cast<unsigned char>(src[code_idx + 2]) & 0b00111111U;
                size_t b4 = static_cast<unsigned char>(src[code_idx + 4]) & 0b00111111U;

                codepoint = (b1 << 18) | (b2 << 12) | (b3 << 6) | b4;
            }
        } else if (c >= 0b11100000U) {
            if (code_idx + 2 < max) {
                size_t b1 = c & 0b00001111U;
                size_t b2 = static_cast<unsigned char>(src[code_idx + 1]) & 0b00111111U;
                size_t b3 = static_cast<unsigned char>(src[code_idx + 2]) & 0b00111111U;
 
                codepoint = (b1 << 12) | (b2 << 6) | b3;
            }
        } else {
            if (code_idx + 1 < max) {
                size_t b1 = c & 0b00011111U;
                size_t b2 = static_cast<unsigned char>(src[code_idx + 1]) & 0b00111111U; 
                
                codepoint = (b1 << 5) | b2;
            }
        }
    }

    return static_cast<uint32_t>(codepoint);
}

uint32_t GYDM::string_utf8_ref(const std::string& src, int idx) {
    return string_utf8_ref(src.c_str(), idx, int(src.size()));
}

size_t GYDM::string_character_size(const char* src, int idx) {
    unsigned char c = static_cast<unsigned char>(src[idx]);
    size_t size = 1;

    if (c >= 0b11110000U) {
        size = 4;
    } else if (c >= 0b11100000U) {
        size = 3;
    } else if (c >= 0b11000000U) {
        size = 2;
    }

    return size;
}

size_t GYDM::string_character_size(const std::string& src, int idx) {
    return string_character_size(src.c_str(), idx);
}

bool GYDM::string_popback_utf8_char(std::string& src) {
    size_t size = src.size();
    bool okay = false;

    if (size > 0) {
        const unsigned char* text = reinterpret_cast<const unsigned char*>(src.c_str());
        
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

std::string GYDM::string_add_between(const char* s, char ch) {
    std::string vstr;
    int idx = 0;

    while (s[idx] != '\0') {
        unsigned char c = static_cast<unsigned char>(s[idx]);

        if (idx > 0) {
            vstr.push_back(ch);
        }

        if (c >= 0b11110000U) {
            vstr.append(s, idx, 4);
            idx += 4;
        } else if (c >= 0b11100000U) {
            vstr.append(s, idx, 3);
            idx += 3;
        } else if (c >= 0b11000000U) {
            vstr.append(s, idx, 2);
            idx += 2;
        } else {
            vstr.push_back(s[idx]);
            idx ++;
        }
    }

    return vstr;
}

/************************************************************************************************/
bool GYDM::string_equal(const char* s1, const char* s2) {
    return string_equal_cs(s1, s2, strlen(s1), strlen(s2));
}

bool GYDM::string_equal(const std::string& s1, const char* s2) {
    return string_equal_cs(s1.c_str(), s2, s1.size(), strlen(s2));
}

bool GYDM::string_equal(const std::string& s1, const std::string& s2) {
    return string_equal_cs(s1.c_str(), s2.c_str(), s1.size(), s2.size());
}

bool GYDM::string_prefix(const char* src, const char* sub, int max0) {
    bool yes = true;
    size_t max = (max0 >= 0) ? max0 : strlen(src);
    size_t n = strnlen(sub, max + 1);

    if (max >= n) {
        for (size_t i = 0; i < n; i ++) {
            if (src[i] != sub[i]) {
                yes = false;
                break;
            }
        }
    } else {
        yes = false;
    }

    return yes;
}

bool GYDM::string_prefix(const std::string& src, const char* sub) {
    return string_prefix(src.c_str(), sub, int(src.size()));
}

bool GYDM::string_prefix(const std::string& src, const std::string& sub) {
    return string_prefix(src.c_str(), sub.c_str(), int(src.size()));
}

bool GYDM::string_suffix(const char* src, const char* sub, int max0) {
    bool yes = true;
    size_t max = (max0 >= 0) ? max0 : strlen(src);
    size_t n = strnlen(sub, max + 1);

    if (max >= n) {
        for (size_t i = n; i > 0; i --) {
            if (src[--max] != sub[i - 1]) {
                yes = false;
                break;
            }
        }
    } else {
        yes = false;
    }

    return yes;
}

bool GYDM::string_suffix(const std::string& src, const char* sub) {
    return string_suffix(src.c_str(), sub, int(src.size()));
}

bool GYDM::string_suffix(const std::string& src, const std::string& sub) {
    return string_suffix(src.c_str(), sub.c_str(), int(src.size()));
}

/************************************************************************************************/
bool GYDM::string_ci_equal(const char* s1, const char* s2) {
    return string_equal_ci(s1, s2, strlen(s1), strlen(s2));
}

bool GYDM::string_ci_equal(const std::string& s1, const char* s2) {
    return string_equal_ci(s1.c_str(), s2, s1.size(), strlen(s2));
}

bool GYDM::string_ci_equal(const std::string& s1, const std::string& s2) {
    return string_equal_ci(s1.c_str(), s2.c_str(), s1.size(), s2.size());
}

bool GYDM::string_ci_prefix(const char* src, const char* sub, int max0) {
    bool yes = true;
    size_t max = (max0 >= 0) ? max0 : strlen(src);
    size_t n = strnlen(sub, max + 1);

    if (max >= n) {
        for (size_t i = 0; i < n; i ++) {
            if (!char_ci_eq(src[i], sub[i])) {
                yes = false;
                break;
            }
        }
    } else {
        yes = false;
    }

    return yes;
}

bool GYDM::string_ci_prefix(const std::string& src, const char* sub) {
    return string_ci_prefix(src.c_str(), sub, int(src.size()));
}

bool GYDM::string_ci_prefix(const std::string& src, const std::string& sub) {
    return string_ci_prefix(src.c_str(), sub.c_str(), int(src.size()));
}

bool GYDM::string_ci_suffix(const char* src, const char* sub, int max0) {
    bool yes = true;
    size_t max = (max0 >= 0) ? max0 : strlen(src);
    size_t n = strnlen(sub, max + 1);

    if (max >= n) {
        for (size_t i = n; i > 0; i --) {
            if (!char_ci_eq(src[--max], sub[i - 1])) {
                yes = false;
                break;
            }
        }
    } else {
        yes = false;
    }

    return yes;
}

bool GYDM::string_ci_suffix(const std::string& src, const char* sub) {
    return string_suffix(src.c_str(), sub, int(src.size()));
}

bool GYDM::string_ci_suffix(const std::string& src, const std::string& sub) {
    return string_suffix(src.c_str(), sub.c_str(), int(src.size()));
}
