#pragma once

#include "tuple.hpp"

namespace WarGrey::STEM {
    template<typename T>
    class __lambda__ EuclideanVector : public WarGrey::STEM::Tuple<WarGrey::STEM::EuclideanVector, T> {
    public:
        EuclideanVector() = default;
        EuclideanVector(T x, T y) : Tuple<WarGrey::STEM::EuclideanVector, T>(x, y) {}

        template <typename U>
        explicit EuclideanVector(const EuclideanVector<U>& v)
            : Tuple<WarGrey::STEM::EuclideanVector, U>(T(v.x), T(v.y)) {}

        ~EuclideanVector() noexcept {}
    };

    typedef EuclideanVector<float> Vector;
}
