#pragma once

#include "base.hpp"
#include "identifier.hpp"

namespace Plteen {
#define define_asn_enum(e, E, ...) \
enum class E { __VA_ARGS__ }; \
inline size_t asn_##e##_span(E e) { return asn_enum_span(e); } \
inline Plteen::octets asn_##e##_to_octets(E e) { return asn_enum_to_octets(e); } \
inline size_t asn_##e##_into_octets(E e, uint8_t* octets, size_t offset = 0) { return asn_enum_into_octets(e, octets, offset); } \
inline E asn_octets_to_##e(const uint8_t* benum, size_t* offset = nullptr) { return asn_octets_to_enum<E>(benum, offset); } \
inline E asn_octets_to_##e(const Plteen::octets& benum, size_t* offset = nullptr) { return asn_octets_to_enum<E>(benum.c_str(), offset); }

    template<typename E>
    inline size_t asn_enum_span(E enumerated) {
        return asn_fixnum_span(static_cast<int64_t>(enumerated));
    }

    template<typename E>
    inline Plteen::octets asn_enum_to_octets(E enumerated) {
        return asn_int64_to_octets(static_cast<int64_t>(enumerated), ASNPrimitive::Enumerated);
    }

    template<typename E>
    inline size_t asn_enum_into_octets(E enumerated, uint8_t* octets, size_t offset = 0) {
        return asn_int64_into_octets(static_cast<int64_t>(enumerated), octets, offset, ASNPrimitive::Enumerated);
    }

    template<typename E>
    inline E asn_octets_to_enum(const uint8_t* benum, size_t* offset = nullptr) {
        return static_cast<E>(asn_octets_to_fixnum(benum, offset));
    }
}
