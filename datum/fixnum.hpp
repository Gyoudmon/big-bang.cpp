#pragma once

#include <cmath>
#include <cstdlib>

namespace GYDM {
#define fxin(open, v, close) ((open <= v) && (v <= close))
#define fxout(open, v, close) ((v < open) || (v > close))

#define _SZ(v) static_cast<size_t>(v)

#define _U8(v) static_cast<uint8_t>(v)
#define _U16(v) static_cast<uint16_t>(v)
#define _U32(v) static_cast<uint32_t>(v)
#define _U64(v) static_cast<uint64_t>(v)

#define _S8(v) static_cast<int8_t>(v)
#define _S16(v) static_cast<int16_t>(v)
#define _S32(v) static_cast<int32_t>(v)
#define _S64(v) static_cast<int64_t>(v)

    int inline fxabs(int fx) { return std::abs(fx); }
    long long inline fxabs(long long fx) { return std::abs(fx); }

    template<typename I> I inline fxmax(I fx1, I fx2) { return ((fx1 < fx2) ? fx2 : fx1); }
    template<typename I> I inline fxmin(I fx1, I fx2) { return ((fx1 < fx2) ? fx1 : fx2); }
    template<typename I> I inline fxmax(I fx1, I fx2, I fx3) { return fxmax(fx1, fxmax(fx2, fx3)); }
    template<typename I> I inline fxmin(I fx1, I fx2, I fx3) { return fxmin(fx1, fxmin(fx2, fx3)); }

    long long inline fxround(long long fx, float precision) { return (long long)(std::roundf(float(fx) * precision)); }
    long long inline fxround(long long fx, double precision) { return (long long)(std::round(double(fx) * precision)); }

    long long inline fxfloor(long long fx, float precision) { return (long long)(std::floor(float(fx) * precision)); }
    long long inline fxfloor(long long fx, double precision) { return (long long)(std::floor(double(fx) * precision)); }

    long long inline fxceiling(long long fx, float precision) { return (long long)(std::ceil(float(fx) * precision)); }
    long long inline fxceiling(long long fx, double precision) { return (long long)(std::ceil(double(fx) * precision)); }

    static inline size_t integer_length(unsigned long long n) { size_t s = 0;  while (n) { n >>= 1; s++; } return s; }

    template<typename I>
    I inline safe_index(I idx, I size) {
        if (idx >= size) {
            idx = idx % size;
        } else if (idx < 0) {
            idx = size - (-idx % size);            
        }

        return idx;
    }

    template<typename I>
    I inline fxmod(I a, I b) {
        I r = a - (a/b) * b;

        return (I)(r < 0 ? r + b : r);
    }

}
