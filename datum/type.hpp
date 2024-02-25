#pragma once

#include <type_traits>

namespace GYDM {
    template<typename T> struct SuperType { using type = long long; };
    template<> struct SuperType<double> { using type = double; };
    template<> struct SuperType<float> { using type = double; };
    
    template<typename Fl> struct LengthType { using type = float; };
    template<> struct LengthType<double> { using type = double; };

    template<typename T>
    using SuperDatum = typename SuperType<T>::type;

    template<typename T>
    using RealDatum = typename std::enable_if<std::is_arithmetic<T>::value>::type;

    template<typename T>
    using FixnumDatum = typename std::enable_if<std::is_integral<T>::value>::type;

    template<typename T>
    using FlonumDatum = typename std::enable_if<std::is_floating_point<T>::value>::type;

    template<typename T, typename V = T>
    using IfRealDatum = typename std::enable_if<std::is_arithmetic<T>::value, V>::type;

    template<typename T, typename V = T>
    using IfFixnumDatum = typename std::enable_if<std::is_integral<T>::value, V>::type;

    template<typename T, typename V = T>
    using IfFlonumDatum = typename std::enable_if<std::is_floating_point<T>::value, V>::type;
}
