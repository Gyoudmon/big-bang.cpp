#pragma once

#include "tuple.hpp"

namespace GYDM {
    template<typename T>
    class __lambda__ Point : public GYDM::Tuple<GYDM::Point, T> {
    public:
        Point() = default;
        Point(T x, T y) : GYDM::Tuple<GYDM::Point, T>(x, y) {}

        template<typename U>
        explicit Point(const GYDM::Point<U>& v)
            : GYDM::Tuple<GYDM::Point, T>(T(v.x), T(v.y)) {}

        template<typename U>
        explicit Point(const GYDM::Point<U>& v, T sx, T sy)
            : GYDM::Tuple<GYDM::Point, T>(T(v.x * sx), T(v.y * sy)) {}

        ~Point() noexcept {}
    };

    typedef GYDM::Point<float> Dot;
}
