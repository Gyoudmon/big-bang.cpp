#pragma once

#include <cmath>
#include <limits>
#include <cstdlib>

// TODO
// check: http://www.plunk.org/~hatch/rightway.html
// check: https://www-pequan.lip6.fr/~graillat/papers/posterRNC7.pdf
// check: https://www-pequan.lip6.fr/~graillat/papers/nolta07.pdf

namespace GYDM {
    // for non-flonums
    template<typename T> bool inline flisnan(T fx) { return false; }
    template<typename T> bool inline flisinfinity(T fx) { return false; }
    template<typename T> bool inline flisfinite(T fx) { return true; }
    template<typename T> bool inline flisinteger(T fx) { return true; }
    template<typename T> T inline flabs(T fx) { return (fx >= 0) ? fx : -fx; }
    template<typename T> T inline flfloor(T fx) { return fx; }
    template<typename T> T inline fltruncate(T fx) { return fx; }
    template<typename T> T inline flfma(T x, T y, T z) { return x * y + z; }

    template<typename T> T inline flsafe(T v, T fallback) { return v; }
    template<typename T> T inline flsafe(T v, T min, T max) {
        if (v < min) {
            v = min;
        } else if (v > max) {
            v = max;
        } else if (flisnan(v)) {
            v = max;
        }

        return v;
    }

    template<typename V1, typename V2, typename E> bool inline flequal(V1 v1, V2 v2, E epsilon) {
        E diff = v2 - v1;

        return (-epsilon <= diff) && (diff <= epsilon);
    }
    
/*************************************************************************************************/    
#define flin(open, v, close) ((open <= v) && (v <= close))
#define flout(open, v, close) ((v < open) || (v > close))

#define flnull static_cast<double*>(nullptr)
#define flnull_f static_cast<float*>(nullptr)
#define flnull_t static_cast<long double*>(nullptr)

    static const float flnan_f = std::nanf("");
    static const double flnan = std::nan("");
    static const long double flnan_t = std::nanl("");

    static const float pi_f       = 3.1415927f;
    static const double pi        = 3.141592653589793;
    static const long double pi_t = 3.1415926535897932385L;

    static const float d_pi_f       = pi_f * 2.0F;
    static const double d_pi        = pi * 2.0;
    static const long double d_pi_t = pi_t * 2.0L;

    static const float h_pi_f       = pi_f * 0.5F;
    static const double h_pi        = pi * 0.5;
    static const long double h_pi_t = pi_t * 0.5L;

    static const float q_pi_f       = pi_f * 0.25F;
    static const double q_pi        = pi * 0.25;
    static const long double q_pi_t = pi_t * 0.25L;

    static const float infinity_f        = std::numeric_limits<float>::infinity();
    static const double infinity         = std::numeric_limits<double>::infinity();
    static const long double infinity_t  = std::numeric_limits<long double>::infinity();

    bool inline flisnan(float f) { return std::isnan(f); }
    bool inline flisnan(double fl) { return std::isnan(fl); }
    bool inline flisnan(long double fl) { return std::isnan(fl); }

    bool inline flisfinite(float f) { return std::isfinite(f); }
    bool inline flisfinite(double fl) { return std::isfinite(fl); }
    bool inline flisfinite(long double fl) { return std::isfinite(fl); }

    bool inline flisinfinity(float f) { return std::isinf(f); }
    bool inline flisinfinity(double fl) { return std::isinf(fl); }
    bool inline flisinfinity(long double fl) { return std::isinf(fl); }

    bool inline flisinteger(float f) { return (std::truncf(f) == f) && flisfinite(f); }
    bool inline flisinteger(double fl) { return (std::trunc(fl) == fl) && flisfinite(fl); }
    bool inline flisinteger(long double fl) { return (std::truncl(fl) == fl) && flisfinite(fl); }

    float inline flsafe(float v, float fallback = 0.0F) { return (flisnan(v) ? fallback : v); }
    double inline flsafe(double v, double fallback = 0.0) { return (flisnan(v) ? fallback : v); }
    long double inline flsafe(long double v, long double fallback = 0.0L) { return (flisnan(v) ? fallback : v); }

    float inline flabs(float f) { return std::abs(f); }
    double inline flabs(double fl) { return std::abs(fl); }
    long double inline flabs(long double fl) { return std::abs(fl); }

    float inline flmod(float a, float b) { return std::fmodf(a, b); }
    double inline flmod(double a, double b) { return std::fmod(a, b); }
    long double inline flmod(long double a, long double b) { return std::fmodl(a, b); }

    // WARNING: 0.0 is +0.0, hence, flsign(0.0) => 1.0
    float inline flsign(float f) { return std::copysignf(1.0f, f); }
    double inline flsign(double fl) { return std::copysign(1.0, fl); }
    long double inline flsign(long double fl) { return std::copysignl(1.0l, fl); }

    float inline fllog(float f) { return std::logf(f); }
    double inline fllog(double fl) { return std::log(fl); }
    long double inline fllog(long double fl) { return std::logl(fl); }

    float inline fllog(float f, float b) { return std::logf(f) / std::logf(b); }
    double inline fllog(double fl, double b) { return std::log(fl) / std::log(b); }
    long double inline fllog(long double fl, long double b) { return std::logl(fl) / std::logl(b); }

    float inline flsqr(float f) { return f * f; }
    double inline flsqr(double fl) { return fl * fl; }
    long double inline flsqr(long double fl) { return fl * fl; }

    float inline flsqrt(float f) { return std::sqrtf(f); }
    double inline flsqrt(double fl) { return std::sqrt(fl); }
    long double inline flsqrt(long double fl) { return std::sqrtl(fl); }

    float inline flexpt(float fb, float fe) { return std::powf(fb, fe); }
    double inline flexpt(double flb, double fle) { return std::pow(flb, fle); }
    long double inline flexpt(long double flb, long double fle) { return std::powl(flb, fle); }

    float inline flmin(float f1, float f2) { return std::fminf(f1, f2); }
    float inline flmin(float f1, float f2, float f3) { return std::fminf(std::fminf(f1, f2), f3); }
    double inline flmin(double fl1, double fl2) { return std::fmin(fl1, fl2); }
    double inline flmin(double fl1, double fl2, double fl3) { return std::fmin(std::fmin(fl1, fl2), fl3); }
    long double inline flmin(long double fl1, long double fl2) { return std::fminl(fl1, fl2); }
    long double inline flmin(long double fl1, long double fl2, long double fl3) { return std::fminl(std::fminl(fl1, fl2), fl3); }

    float inline flmax(float f1, float f2) { return std::fmaxf(f1, f2); }
    float inline flmax(float f1, float f2, float f3) { return std::fmaxf(std::fmaxf(f1, f2), f3); }
    double inline flmax(double fl1, double fl2) { return std::fmax(fl1, fl2); }
    double inline flmax(double fl1, double fl2, double fl3) { return std::fmax(std::fmax(fl1, fl2), fl3); }
    long double inline flmax(long double f1, long double f2) { return std::fmaxl(f1, f2); }
    long double inline flmax(long double f1, long double f2, long double f3) { return std::fmaxl(std::fmaxl(f1, f2), f3); }

    float inline flround(float f) { return std::roundf(f); }
    double inline flround(double fl) { return std::round(fl); }
    long double inline flround(long double fl) { return std::roundl(fl); }

    float inline flfloor(float f) { return std::floorf(f); }
    double inline flfloor(double fl) { return std::floor(fl); }
    long double inline flfloor(long double fl) { return std::floorl(fl); }

    float inline flceiling(float f) { return std::ceilf(f); }
    double inline flceiling(double fl) { return std::ceil(fl); }
    long double inline flceiling(long double fl) { return std::ceill(fl); }

    float inline fltruncate(float f) { return std::truncf(f); }
    double inline fltruncate(double fl) { return std::trunc(fl); }
    long double inline fltruncate(long double fl) { return std::truncl(fl); }

    float inline flsin(float f) { return std::sinf(f); }
    double inline flsin(double fl) { return std::sin(fl); }
    long double inline flsin(long double fl) { return std::sinl(fl); }

    float inline flasin(float f) { return std::asinf(f); }
    double inline flasin(double fl) { return std::asin(fl); }
    long double inline flasin(long double fl) { return std::asinl(fl); }

    float inline flcos(float f) { return std::cosf(f); }
    double inline flcos(double fl) { return std::cos(fl); }
    long double inline flcos(long double fl) { return std::cosl(fl); }

    float inline flacos(float f) { return std::acosf(f); }
    double inline flacos(double fl) { return std::acos(fl); }
    long double inline flacos(long double fl) { return std::acosl(fl); }

    float inline fltan(float f) { return std::tanf(f); }
    double inline fltan(double fl) { return std::tan(fl); }
    long double inline fltan(long double fl) { return std::tanl(fl); }

    float inline flatan(float fy, float fx) { return std::atan2f(fy, fx); }
    double inline flatan(double fly, double flx) { return std::atan2(fly, flx); }
    long double inline flatan(long double fly, long double flx) { return std::atan2l(fly, flx); }

    float inline flfma(float x, float y, float z) { return std::fmaf(x, y, z); }
    double inline flfma(double x, double y, double z) { return std::fma(x, y, z); }
    long double inline flfma(long double x, long double y, long double z) { return std::fmal(x, y, z); }

    template<typename T> T inline fl2fx(float f) { return T(std::roundf(f)); }
    template<typename T> T inline fl2fx(double fl) { return T(std::round(fl)); }
    template<typename T> T inline fl2fx(long double fl) { return T(std::roundl(fl)); }

    int inline fl2fxi(float f) { return GYDM::fl2fx<int>(f); }
    int inline fl2fxi(double fl) { return GYDM::fl2fx<int>(fl); }
    int inline fl2fxi(long double fl) { return GYDM::fl2fx<int>(fl); }

/*************************************************************************************************/
    /**
     * for Error-Free Transformations
     * 
     *      FMA(x, y, z) = x * y + z
     * where the FMA is short for `fused multiply add`
     * 
     * The `error` might be quite considerable
     */

    template<typename Fl>
    inline Fl fl_add(Fl a, Fl b) {
        Fl sum = a + b;
        Fl delta = sum - a;
        Fl error = (a - (sum - delta)) + (b - delta);
    
        return sum + error;
    }

    template<typename Fl>
    inline Fl fl_multiply(Fl a, Fl b) {
        Fl product = a * b;
        Fl error = flfma(a, b, -product);

        return product + error;
    }

    template<typename Fl>
    inline Fl fl_add_products(Fl a, Fl b, Fl c, Fl d) {
        Fl cd = c * d;
        Fl sum = flfma(a, b, cd);
        Fl error = flfma(c, d, -cd);
        
        return sum + error;    
    }

    template<typename Fl>
    inline Fl fl_subtract_products(Fl a, Fl b, Fl c, Fl d) {
        Fl cd = c * d;
        Fl diff = flfma(a, b, -cd);
        Fl error = flfma(-c, d, cd);

        return diff + error;
    }

    template<typename Super, typename Fl1, typename Fl2 = Fl1>
    inline Super fl_safe_add(Fl1 a, Fl2 b) {
        return fl_add(Super(a), Super(b));
    }

    template<typename Super, typename Fl1, typename Fl2 = Fl1>
    inline Super fl_safe_multiply(Fl1 a, Fl2 b) {
        return fl_multiply(Super(a), Super(b));
    }

    template<typename Super, typename Fl1, typename Fl2 = Fl1, typename Fl3 = Fl1, typename Fl4 = Fl1>
    inline Super fl_safe_add_products(Fl1 a, Fl2 b, Fl3 c, Fl4 d) {
        return fl_add_products(Super(a), Super(b), Super(c), Super(d));
    }

    template<typename Super, typename Fl1, typename Fl2 = Fl1, typename Fl3 = Fl1, typename Fl4 = Fl1>
    inline Super fl_safe_subtract_products(Fl1 a, Fl2 b, Fl3 c, Fl4 d) {
        return fl_subtract_products(Super(a), Super(b), Super(c), Super(d));
    }
}
