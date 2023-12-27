#pragma once

#include "tuple.hpp"

namespace GYDM {
    template<typename T>
    class __lambda__ Point : public GYDM::Tuple<GYDM::Point, T> {
    public:
        Point() = default;
        Point(T x, T y) : Tuple<GYDM::Point, T>(x, y) {}

        template<typename U>
        explicit Point(const GYDM::Point<U>& v)
            : Tuple<GYDM::Point, U>(T(v.x), T(v.y)) {}

        ~Point() noexcept {}
    };

    typedef Point<float> Dot; 
}
