#include <ctime>
#include <chrono>
#include <thread>

#include "time.hpp"

using namespace Plteen;
using namespace std::chrono;

/**************************************************************************************************/
static inline void ntime(char* timestamp, const time_t utc_s, const char* tfmt, bool locale = false) {
    struct tm now_s;

#if defined(__windows__)
    if (locale) {
        localtime_s(&now_s, &utc_s);
    } else {
        gmtime_s(&now_s, &utc_s);
    }
#else
    if (locale) {
        localtime_r(&utc_s, &now_s);
    } else {
        gmtime_r(&utc_s, &now_s);
    }
#endif

    strftime(timestamp, 31, tfmt, &now_s);
}

/**************************************************************************************************/
long long Plteen::current_seconds() {
    return time(nullptr);
}

long long Plteen::current_milliseconds() {
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

double Plteen::current_inexact_milliseconds() {
    return double(duration_cast<microseconds>(system_clock::now().time_since_epoch()).count()) / 1000.0;
}

/**************************************************************************************************/
void Plteen::sleep(long long ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void Plteen::sleep_us(long long us) {
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

/*************************************************************************************************/
std::string Plteen::make_now_timestamp_utc(bool locale, bool needs_ms) {
    long long ms = current_milliseconds();
    long long s = ms / 1000;
    std::string ts = make_timestamp_utc(s, locale);

    if (needs_ms) {
        ts.append(".", 1);
        ts.append(std::to_string(ms % 1000));
    }

    return ts;
}

std::string Plteen::make_timestamp_utc(long long utc_s, bool locale) {
    char timestamp[32];

    ntime(timestamp, utc_s, "%FT%T", locale);

    return std::string(timestamp);
}

std::string Plteen::make_datestamp_utc(long long utc_s, bool locale) {
    char timestamp[32];

    ntime(timestamp, utc_s, "%F", locale);

    return std::string(timestamp);
}

std::string Plteen::make_daytimestamp_utc(long long utc_s, bool locale) {
    char timestamp[32];

    ntime(timestamp, utc_s, "%T", locale);
        
    return std::string(timestamp);
}
