#pragma once

#include <string>

namespace GYDM {
    __lambda__ long long current_seconds();
    __lambda__ long long current_milliseconds();
    __lambda__ double current_inexact_milliseconds();
    
    __lambda__ void sleep(long long ms);
    __lambda__ void sleep_us(long long us);
    
    __lambda__ std::string make_now_timestamp_utc(bool locale, bool needs_ms = true);

    __lambda__ std::string make_timestamp_utc(long long utc_s, bool locale);
    __lambda__ std::string make_datestamp_utc(long long utc_s, bool locale);
    __lambda__ std::string make_daytimestamp_utc(long long utc_s, bool locale);
}
