#include <ctime>
#include <chrono>
#include <thread>

#include "time.hpp"

using namespace WarGrey::STEM;
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
long long WarGrey::STEM::current_seconds() {
    return time(nullptr);
}

long long WarGrey::STEM::current_milliseconds() {
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

/**************************************************************************************************/
void WarGrey::STEM::sleep(long long ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void WarGrey::STEM::sleep_us(long long us) {
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

/*************************************************************************************************/
std::string WarGrey::STEM::make_timestamp_utc(long long utc_s, bool locale) {
    char timestamp[32];

    ntime(timestamp, utc_s, "%FT%T", locale);

    return std::string(timestamp);
}

std::string WarGrey::STEM::make_datestamp_utc(long long utc_s, bool locale) {
    char timestamp[32];

    ntime(timestamp, utc_s, "%F", locale);

    return std::string(timestamp);
}

std::string WarGrey::STEM::make_daytimestamp_utc(long long utc_s, bool locale) {
    char timestamp[32];

    ntime(timestamp, utc_s, "%T", locale);
        
    return std::string(timestamp);
}

