#pragma once

#include <cmath>
#include <limits>
#include <cstdlib>

namespace WarGrey::STEM {
#define flin(open, v, close) ((open <= v) && (v <= close))
#define flout(open, v, close) ((v < open) || (v > close))

#define flnull static_cast<double*>(nullptr)
#define flnull_f static_cast<float*>(nullptr)

    static const float flnan_f = std::nanf("");
    static const double flnan = std::nan("");
    static const long double flnan_t = std::nanl("");

    static const float pi_f       = 3.1415927f;
    static const double pi        = 3.141592653589793;
    static const long double pi_t = 3.1415926535897932385L;

    static const float d_pi_f       = pi_f * 2.0F;
    static const double d_pi        = pi * 2.0;
    static const long double d_pi_t = pi_t * 2.0L;

    static const float h_pi_f       = pi_f / 2.0F;
    static const double h_pi        = pi / 2.0;
    static const long double h_pi_t = pi_t / 2.0L;

    static const float q_pi_f       = pi_f / 4.0F;
    static const double q_pi        = pi / 4.0;
    static const long double q_pi_t = pi_t / 4.0L;

    static const float infinity_f        = std::numeric_limits<float>::infinity();
    static const double infinity         = std::numeric_limits<double>::infinity();
    static const long double infinity_t  = std::numeric_limits<long double>::infinity();

    bool inline flisnan(float f) { return std::isnan(f); }
    bool inline flisnan(double fl) { return std::isnan(fl); }
    bool inline flisnan(long double fl) { return std::isnan(fl); }

    bool inline flisfinite(float f) { return std::isfinite(f); }
    bool inline flisfinite(double fl) { return std::isfinite(fl); }
    bool inline flisfinite(long double fl) { return std::isfinite(fl); }

    float inline flsafe(float v, float fallback = 0.0F) { return (flisnan(v) ? fallback : v); }
    double inline flsafe(double v, double fallback = 0.0) { return (flisnan(v) ? fallback : v); }
    long double inline flsafe(long double v, long double fallback = 0.0L) { return (flisnan(v) ? fallback : v); }

    // for non-flonums
    template<typename T> T inline flsafe(T v, T fallback) { return v; }
    template<typename T> bool inline flisnan(T fl) { return false; }

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

    float inline flabs(float f) { return std::abs(f); }
    double inline flabs(double fl) { return std::abs(fl); }
    long double inline flabs(long double fl) { return std::abs(fl); }

    // WARNING: 0.0 is +0.0, hence, flsign(0.0) => 1.0
    float inline flsign(float f) { return std::copysignf(1.0f, f); }
    double inline flsign(double fl) { return std::copysign(1.0, fl); }
    long double inline flsign(long double fl) { return std::copysignl(1.0l, fl); }

    float inline fllog(float f) { return std::log(f); }
    double inline fllog(double fl) { return std::log(fl); }

    float inline fllog(float f, float b) { return std::log(f) / std::log(b); }
    double inline fllog(double fl, double b) { return std::log(fl) / std::log(b); }

    float inline flsqr(float f) { return f * f; }
    double inline flsqr(double fl) { return fl * fl; }

    float inline flsqrt(float f) { return std::sqrt(f); }
    double inline flsqrt(double fl) { return std::sqrt(fl); }

    float inline flexpt(float fb, float fe) { return std::pow(fb, fe); }
    double inline flexpt(double flb, double fle) { return std::pow(flb, fle); }

    float inline flmin(float f1, float f2) { return std::fmin(f1, f2); }
    float inline flmin(float f1, float f2, float f3) { return std::fmin(std::fmin(f1, f2), f3); }
    double inline flmin(double fl1, double fl2) { return std::fmin(fl1, fl2); }
    double inline flmin(double fl1, double fl2, double fl3) { return std::fmin(std::fmin(fl1, fl2), fl3); }

    float inline flmax(float f1, float f2) { return std::fmax(f1, f2); }
    float inline flmax(float f1, float f2, float f3) { return std::fmax(std::fmax(f1, f2), f3); }
    double inline flmax(double fl1, double fl2) { return std::fmax(fl1, fl2); }
    double inline flmax(double fl1, double fl2, double fl3) { return std::fmax(std::fmax(fl1, fl2), fl3); }

    float inline flround(float f) { return std::round(f); }
    double inline flround(double fl) { return std::round(fl); }

    float inline flfloor(float f) { return std::floor(f); }
    double inline flfloor(double fl) { return std::floor(fl); }

    float inline flceiling(float f) { return std::ceil(f); }
    double inline flceiling(double fl) { return std::ceil(fl); }

    float inline flsin(float f) { return std::sin(f); }
    double inline flsin(double fl) { return std::sin(fl); }

    float inline flasin(float f) { return std::asin(f); }
    double inline flasin(double fl) { return std::asin(fl); }

    float inline flcos(float f) { return std::cos(f); }
    double inline flcos(double fl) { return std::cos(fl); }

    float inline flacos(float f) { return std::acos(f); }
    double inline flacos(double fl) { return std::acos(fl); }

    float inline fltan(float f) { return std::tan(f); }
    double inline fltan(double fl) { return std::tan(fl); }

    float inline flatan(float fy, float fx) { return std::atan2(fy, fx); }
    double inline flatan(double fly, double flx) { return std::atan2(fly, flx); }

    template<typename T> T inline fl2fx(float f) { return T(std::round(f)); }
    template<typename T> T inline fl2fx(double fl) { return T(std::round(fl)); }

    int inline fl2fxi(float f) { return WarGrey::STEM::fl2fx<int>(f); }
    int inline fl2fxi(double fl) { return WarGrey::STEM::fl2fx<int>(fl); }
}
