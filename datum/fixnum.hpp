#pragma once

#include <cmath>
#include <cstdlib>

namespace WarGrey::STEM {
#define fxin(open, v, close) ((open <= v) && (v <= close))
#define fxout(open, v, close) ((v < open) || (v > close))

    int inline fxabs(int fx) { return std::abs(fx); }
    long long inline fxabs(long long fx) { return std::abs(fx); }

    template<typename I> I inline fxmax(I fx1, I fx2) { return ((fx1 < fx2) ? fx2 : fx1); }
    template<typename I> I inline fxmin(I fx1, I fx2) { return ((fx1 < fx2) ? fx1 : fx2); }

    long long inline fxround(long long fx, float precision) { return (long long)(std::roundf(float(fx) * precision)); }
    long long inline fxround(long long fx, double precision) { return (long long)(std::round(double(fx) * precision)); }

    long long inline fxfloor(long long fx, float precision) { return (long long)(std::floor(float(fx) * precision)); }
    long long inline fxfloor(long long fx, double precision) { return (long long)(std::floor(double(fx) * precision)); }

    long long inline fxceiling(long long fx, float precision) { return (long long)(std::ceil(float(fx) * precision)); }
    long long inline fxceiling(long long fx, double precision) { return (long long)(std::ceil(double(fx) * precision)); }

    static inline size_t integer_length(unsigned long long n) { size_t s = 0;  while (n) { n >>= 1; s++; } return s; }
}

