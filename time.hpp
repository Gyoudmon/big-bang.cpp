#pragma once

#include <string>

namespace WarGrey::STEM {
    long long current_seconds();
    long long current_milliseconds();

    void sleep(long long ms);
    void sleep_us(long long us);
    
    std::string make_timestamp_utc(long long utc_s, bool locale);
    std::string make_datestamp_utc(long long utc_s, bool locale);
    std::string make_daytimestamp_utc(long long utc_s, bool locale);
}

