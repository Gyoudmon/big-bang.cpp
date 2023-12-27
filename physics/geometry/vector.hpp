#pragma once

#include "tuple.hpp"

namespace GYDM {
    template<typename T>
    class __lambda__ EuclideanVector : public GYDM::Tuple<GYDM::EuclideanVector, T> {
    public:
        EuclideanVector() = default;
        EuclideanVector(T x, T y) : Tuple<GYDM::EuclideanVector, T>(x, y) {}

        template <typename U>
        explicit EuclideanVector(const EuclideanVector<U>& v)
            : Tuple<GYDM::EuclideanVector, U>(T(v.x), T(v.y)) {}

        ~EuclideanVector() noexcept {}
    };

    typedef EuclideanVector<float> Vector;
}
