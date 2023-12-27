#pragma once

#include "tuple.hpp"

namespace WarGrey::STEM {
    template<typename T>
    class __lambda__ Point : public WarGrey::STEM::Tuple<WarGrey::STEM::Point, T> {
    public:
        Point() = default;
        Point(T x, T y) : Tuple<WarGrey::STEM::Point, T>(x, y) {}

        template<typename U>
        explicit Point(const WarGrey::STEM::Point<U>& v)
            : Tuple<WarGrey::STEM::Point, U>(T(v.x), T(v.y)) {}

        ~Point() noexcept {}
    };

    typedef Point<float> Dot; 
}
