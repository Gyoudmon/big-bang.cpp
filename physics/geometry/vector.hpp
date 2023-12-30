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
            : GYDM::Tuple<GYDM::EuclideanVector, U>(T(v.x), T(v.y)) {}

        template<typename U>
        explicit EuclideanVector(const GYDM::EuclideanVector<U>& v, T sx, T sy)
            : GYDM::Tuple<GYDM::EuclideanVector, T>(T(v.x * sx), T(v.y * sy)) {}

        ~EuclideanVector() noexcept {}
    };

    typedef GYDM::EuclideanVector<float> Vector;
}
