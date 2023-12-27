#include "base.hpp"

#include "../../../datum/box.hpp"
#include "../../../datum/fixnum.hpp"
#include "../../../datum/flonum.hpp"

using namespace GYDM;

/*************************************************************************************************/
static const uint8_t default_flonum_base = 2; // TODO: implememnt base 10 representation

static inline size_t fill_ufixnum_octets(uint8_t* pool, uint64_t n, size_t capacity, size_t payload) {
    do {
        payload ++;
        pool[capacity - payload] = _U8(n & 0xFFU);
        n >>= 8U;
    } while (n > 0U);

    return payload;
}

static inline size_t fill_nfixnum_octets(uint8_t* pool, int64_t n, size_t capacity, size_t payload) {
    do {
        payload ++;
        pool[capacity - payload] = _U8(n & 0xFFU);
        n >>= 8U;
    } while (n < -1);

    return payload;
}

static size_t fill_fixnum_octets(uint8_t* pool, int64_t n, size_t capacity, size_t payload) {
    if (n >= 0) {
        payload = fill_ufixnum_octets(pool, n, capacity, payload);

        if (pool[capacity - payload] >= 0b10000000) {
            pool[capacity - (++payload)] = 0;
        }
    } else {
        payload = fill_nfixnum_octets(pool, n, capacity, payload);
 
        if (pool[capacity - payload] < 0b10000000) {
            pool[capacity - (++payload)] = 0xFFU;
        }
    }

    return payload;
}

static size_t fill_length_octets(uint8_t* pool, size_t length, size_t capacity) {
    size_t payload = 1U;
    
    if (length <= 127) {
        pool[capacity - payload] = _U8(length);
    } else {
        payload = fill_ufixnum_octets(pool, length, capacity, payload - 1);
        payload += 1;
        pool[capacity - payload] = _U8(0b10000000 | (payload - 1));
    }

    return payload;
}

static inline void substitude_trailing_zero(int64_t* E, int64_t* N, uint8_t delta, uint64_t mask, uint8_t rshift) {
    while (((*N) & mask) == 0) {
        (*E) += delta;
        (*N) >>= rshift;
    }
}

static void fill_flonum_binary(double real, double base, int64_t* E, int64_t* N) {
    double r = real;

    (*E) = 0;

    // TODO: implement Racket `integer?`
    while (flfloor(r) != r) {
        r *= base;
        (*E) -= 1U;
    }

    (*N) = _S64(flfloor(r));

    if (base == 16.0) {
        substitude_trailing_zero(E, N, 2, 0xFF, -8);
    } else if (base == 2.0) {
        substitude_trailing_zero(E, N, 8, 0xFF, -8);
    } else {
        substitude_trailing_zero(E, N, 1, 0b111, -3);
    }
}

template<typename N>
static inline void fill_integer_from_bytes(N* n, const uint8_t* pool, size_t start, size_t end, bool check_sign = false) {
    if (check_sign) {
        (*n) = ((pool[start] >= 0b10000000) ? -1 : 0);
    } else {
        (*n) = 0;
    }

    for (size_t idx = start; idx < end; idx++) {
        (*n) = ((*n) << 8U) | pool[idx];
    }
}


static uint8_t make_flonum_infoctet(double real, uint8_t base, size_t E_size, size_t binary_scaling_factor) {
    uint8_t infoctet = ((real > 0.0) ? 0b10000000 : 0b11000000);
    
    switch (base) {
    case 8:  infoctet ^= 0b00010000U; break;
    case 16: infoctet ^= 0b00100000U; break;
    }

    switch (binary_scaling_factor) {
    case 0:  infoctet ^= 0b00000000; break;
    case 1:  infoctet ^= 0b00000100; break;
    case 2:  infoctet ^= 0b00001000; break;
    default: infoctet ^= 0b00001100;
    }

    switch (E_size) {
    case 2:  infoctet ^= 0b00000001U; break;
    case 3:  infoctet ^= 0b00000010U; break;
    }

    if (E_size >= 4) {
        infoctet ^= 0b00000011U;
    }

    return infoctet;
}

/*************************************************************************************************/
size_t GYDM::asn_length_span(size_t length) {
    size_t span = 1;

    if (length > 127) {
        do {
            span++;
            length >>= 8U;
        } while (length > 0U);
    }

    return span;
}

octets GYDM::asn_length_to_octets(size_t length) {
    size_t span = asn_length_span(length);
    octets asn(span, '\0');

    asn_length_into_octets(length, const_cast<uint8_t*>(asn.c_str()));

    return asn;
}

size_t GYDM::asn_length_into_octets(size_t length, uint8_t* octets, size_t offset) {
    size_t span = asn_length_span(length);

    fill_length_octets(octets + offset, length, span);

    return offset + span;
}

size_t GYDM::asn_octets_to_length(const uint8_t* blength, size_t* offset) {
    size_t idx = ((offset == nullptr) ? 0 : (*offset));
    size_t length = blength[idx];

    if (length > 0b10000000) {
        size_t size = length & 0b01111111;

        fill_integer_from_bytes(&length, blength, idx + 1, idx + size + 1, false);

        if (offset != nullptr) {
            (*offset) += (size + 1);
        }
    } else if (offset != nullptr) {
        (*offset) += 1;
    }

    return length;
}

/*************************************************************************************************/
bool GYDM::asn_primitive_predicate(ASNPrimitive type, const uint8_t* content, size_t offset) {
    return (asn_primitive_identifier_octet(type) == content[offset]);
}

bool GYDM::asn_primitive_predicate(ASNPrimitive type, const octets& content, size_t offset) {
    return (asn_primitive_identifier_octet(type) == content[offset]);
}

bool GYDM::asn_constructed_predicate(ASNConstructed type, const uint8_t* content, size_t offset) {
    return (asn_constructed_identifier_octet(type) == content[offset]);
}

bool GYDM::asn_constructed_predicate(ASNConstructed type, const octets& content, size_t offset) {
    return (asn_constructed_identifier_octet(type) == content[offset]);
}

octets GYDM::asn_octets_box(uint8_t tag, const octets& content, size_t size) {
    uint8_t pool[10];
    size_t capacity = sizeof(pool) / sizeof(uint8_t);
    size_t payload = fill_length_octets(pool, size, capacity);

    pool[capacity - (++payload)] = tag;

    return octets(content, 0, size).insert(0, pool + (capacity - payload), payload);
}

size_t GYDM::asn_octets_unbox(const uint8_t* basn, size_t* offset) {
    size_t content_idx = ((offset == nullptr) ? 0 : (*offset)) + 1U;
    size_t size = asn_octets_to_length(basn, &content_idx);

    SET_BOX(offset, (content_idx + size));

    return size;
}

/*************************************************************************************************/
octets GYDM::asn_boolean_to_octets(bool b) {
    octets bbool(3, '\0');

    asn_boolean_into_octets(b, const_cast<uint8_t*>(bbool.c_str()), 0);

    return bbool;
}

size_t GYDM::asn_boolean_into_octets(bool b, uint8_t* octets, size_t offset) {
    size_t span = asn_boolean_span(b);

    octets[offset++] = asn_primitive_identifier_octet(ASNPrimitive::Boolean);
    octets[offset++] = _U8(span);
    octets[offset++] = (b ? 0xFF : 0x00);

    return offset;
}

bool GYDM::asn_octets_to_boolean(const uint8_t* bnat, size_t* offset0) {
    size_t offset = ((offset0 == nullptr) ? 0 : (*offset0));
    size_t size = asn_octets_unbox(bnat, &offset);

    SET_BOX(offset0, offset);

    return (bnat[offset - size] > 0x00);
}

octets GYDM::asn_null_to_octets(std::nullptr_t placeholder) {
    uint8_t pool[2];

    asn_null_into_octets(placeholder, pool, 0);

    return octets(pool, sizeof(pool) / sizeof(uint8_t));
}

size_t GYDM::asn_null_into_octets(std::nullptr_t placeholder, uint8_t* octets, size_t offset) {
    octets[offset++] = asn_primitive_identifier_octet(ASNPrimitive::Null);
    octets[offset++] = 0x00;

    return offset;
}

std::nullptr_t GYDM::asn_octets_to_null(const uint8_t* bnull, size_t* offset0) {
    size_t offset = ((offset0 == nullptr) ? 0 : (*offset0));
    
    asn_octets_unbox(bnull, &offset);
    SET_BOX(offset0, offset);

    return nullptr;
}

size_t GYDM::asn_fixnum_span(int64_t fixnum) {
    size_t span = 0;

    /**
     * TODO: To be proven
     *   ASN.1 requires that the leading 9 bits must be neither all 0s nor all 1s.
     * That is, the sign bit is important and should be counted on for both
     * positive and negative integers. Thus, it should be okay to do bitwise
     * flipping on negative integers and treat them like positive ones, but
     * no need for their exact counterparts.
     */

    if (fixnum < 0) {
        fixnum = ~fixnum;
    }

    while (fixnum > 0xFF) {
        span++;
        fixnum >>= 8U;
    }

    span += ((fixnum >= 0b10000000) ? 2 : 1);

    return span;
}

octets GYDM::asn_int64_to_octets(int64_t fixnum, ASNPrimitive id) {
    size_t span = asn_span(asn_fixnum_span(fixnum));
    octets asn(span, '\0');
    
    asn_int64_into_octets(fixnum, const_cast<uint8_t*>(asn.c_str()), 0, id);

    return asn;
}

size_t GYDM::asn_int64_into_octets(int64_t fixnum, uint8_t* octets, size_t offset, ASNPrimitive id) {
    size_t span = asn_fixnum_span(fixnum);
    
    octets[offset++] = asn_primitive_identifier_octet(id);
    octets[offset++] = _U8(span);

    fill_fixnum_octets(octets + offset, fixnum, span, 0U);

    return offset + span;
}

int64_t GYDM::asn_octets_to_fixnum(const uint8_t* bfixnum, size_t* offset0) {
    size_t offset = ((offset0 == nullptr) ? 0 : (*offset0));
    size_t size = asn_octets_unbox(bfixnum, &offset);
    int64_t integer = 0;

    fill_integer_from_bytes(&integer, bfixnum, offset - size, offset, true);

    SET_BOX(offset0, offset);

    return integer;
}

size_t GYDM::asn_natural_span(Natural& nat) {
    size_t span = nat.length();

    if (nat[0] >= 0b10000000) {
        span += 1;
    }

    return span;
}

octets GYDM::asn_natural_to_octets(Natural& nat) {
    octets payload = nat.to_bytes();
    size_t size = nat.length();
    
    if (payload[0] >= 0b10000000) {
        payload.insert(0, 1, '\x00');
        size += 1;
    }

    return asn_octets_box(asn_primitive_identifier_octet(ASNPrimitive::Integer), payload, size);
}

size_t GYDM::asn_natural_into_octets(Natural& nat, uint8_t* octets, size_t offset) {
    size_t size = nat.length();
    size_t span = asn_natural_span(nat);

    octets[offset + 0] = asn_primitive_identifier_octet(ASNPrimitive::Integer);
    offset = asn_length_into_octets(span, octets, offset + 1);

    if (span > size) {
        octets[offset++] = '\x00';
    }

    memcpy(octets + offset, &nat[0], size);

    return offset + size;
}

Natural GYDM::asn_octets_to_natural(const uint8_t* bnat, size_t* offset0) {
    size_t offset = ((offset0 == nullptr) ? 0 : (*offset0));
    size_t size = asn_octets_unbox(bnat, &offset);
    Natural nat(bnat, offset - size, offset);

    SET_BOX(offset0, offset);

    return nat;
}

size_t GYDM::asn_flonum_span(double real) {
    size_t span = 1;
    uint8_t base = 2;

    if (flisfinite(real)) {
        // WARNING: 0.0 is +0.0, hence 1.0
        if (real == 0.0) {
            if (flsign(real) == 1.0) {
                span = 0;
            }
        } else {
            int64_t E, N;
            size_t E_size;

            fill_flonum_binary(flabs(real), double(base), &E, &N);
            E_size = asn_fixnum_span(E);
            
            span = 1 + E_size + asn_fixnum_span(N);
            
            if (E_size >= 4) {
                span += 1;
            }
        }
    }

    return span;
}

octets GYDM::asn_flonum_to_octets(double real) {
    size_t span = asn_span(asn_flonum_span(real));
    octets asn(span, '\0');
    
    asn_flonum_into_octets(real, const_cast<uint8_t*>(asn.c_str()), 0);

    return asn;
}

size_t GYDM::asn_flonum_into_octets(double real, uint8_t* octets, size_t offset) {
    octets[offset++] = asn_primitive_identifier_octet(ASNPrimitive::Real);

    if (!flisfinite(real)) {
        octets[offset++] = 1;

        if (real > 0.0) {
            octets[offset++] = 0x40;
        } else if (real < 0.0) {
            octets[offset++] = 0x41;
        } else {
            octets[offset++] = 0x42;
        }
    } else if (real == 0.0) {
        // WARNING: 0.0 is +0.0, hence 1.0
        if (flsign(real) == -1.0) {
            octets[offset++] = 1U;
            octets[offset++] = 0x43;
        } else {
            octets[offset++] = 0U;
        }
    } else {
        size_t E_size, N_size;
        int64_t E, N;

        fill_flonum_binary(flabs(real), double(default_flonum_base), &E, &N);
        E_size = asn_fixnum_span(E);
        N_size = asn_fixnum_span(N);

        offset = asn_length_into_octets(1 + ((E_size >= 4) ? 1 : 0) + E_size + N_size, octets, offset);
        octets[offset++] = make_flonum_infoctet(real, default_flonum_base, E_size, 0);

        if (E_size >= 4) {
            octets[offset++] = _U8(E_size);
        }

        offset += E_size;
        fill_fixnum_octets(octets, E, offset, 0);
        offset += N_size;
        fill_fixnum_octets(octets, N, offset, 0);
    }

    return offset;
}

double GYDM::asn_octets_to_flonum(const uint8_t* breal, size_t* offset0) {
    size_t offset = ((offset0 == nullptr) ? 0 : (*offset0));
    size_t size = asn_octets_unbox(breal, &offset);
    double real = flnan;

    if (size > 0) {
        uint8_t infoctet = breal[offset - size];

        if ((infoctet & (0b1 << 7)) > 0) {
            double sign = ((infoctet & (0b1 << 6)) > 0) ? -1.0 : 1.0;
            double base = flnan;
            uint8_t N_lshift = ((infoctet & 0b1100) >> 2);
            size_t E_start = offset - size + 1;
            size_t E_end = offset;

            switch ((infoctet & 0b110000) >> 4) {
            case 0b00: base = 2.0; break;
            case 0b01: base = 8.0; break;
            case 0b10: base = 16.0; break;
            }

            switch (infoctet & 0b11) {
            case 0b00: E_end = E_start + 1; break;
            case 0b01: E_end = E_start + 2; break;
            case 0b11: E_end = E_start + 3; break;
            default: E_start ++; E_end = E_start + breal[E_start - 1];
            }

            if (E_end < offset) {
                int64_t E, N;

                fill_integer_from_bytes(&E, breal, E_start, E_end, true);
                fill_integer_from_bytes(&N, breal, E_end, offset, true);
                real = sign * double(N << N_lshift) * flexpt(base, double(E));
            } else if (E_end == offset) {
                real = 0.0;
            }
        } else {
            switch (infoctet) {
            case 0b01000000: real = +infinity; break;
            case 0b01000001: real = -infinity; break;
            case 0b01000010: real = flnan; break;
            case 0b01000011: real = -0.0; break;
            }
        }
    } else if (size == 0) {
        real = 0.0;
    }

    SET_BOX(offset0, offset);

    return real;
}

size_t GYDM::asn_ia5_span(const std::string& str) {
    return str.length();
}

octets GYDM::asn_ia5_to_octets(const std::string& str) {
    size_t payload = asn_ia5_span(str);
    octets asn(asn_span(payload), '\0');

    asn_ia5_into_octets(str, const_cast<uint8_t*>(asn.c_str()), 0U);

    return asn;
}

size_t GYDM::asn_ia5_into_octets(const std::string& str, uint8_t* octets, size_t offset) {
    size_t size = str.length();

    octets[offset++] = asn_primitive_identifier_octet(ASNPrimitive::IA5_String);
    offset = asn_length_into_octets(size, octets, offset);
    memcpy(octets + offset, str.c_str(), size);

    return offset + size;
}

std::string GYDM::asn_octets_to_ia5(const uint8_t* bia5, size_t* offset0) {
    size_t offset = ((offset0 == nullptr) ? 0 : (*offset0));
    size_t size = asn_octets_unbox(bia5, &offset);

    SET_BOX(offset0, offset);

    return std::string(reinterpret_cast<const char*>(bia5 + (offset - size)), size);
}

size_t GYDM::asn_utf8_span(const std::string& str) {
    return str.size();
}

octets GYDM::asn_utf8_to_octets(const std::string& str) {
    size_t payload = asn_utf8_span(str);
    octets asn(asn_span(payload), '\0');

    asn_utf8_into_octets(str, const_cast<uint8_t*>(asn.c_str()), 0U);

    return asn;
}

size_t GYDM::asn_utf8_into_octets(const std::string& str, uint8_t* octets, size_t offset) {
    size_t size = asn_utf8_span(str);
    
    octets[offset++] = asn_primitive_identifier_octet(ASNPrimitive::UTF8_String);
    offset = asn_length_into_octets(size, octets, offset);
    memcpy(octets + offset, str.c_str(), size);

    return offset + size;
}

std::string GYDM::asn_octets_to_utf8(const uint8_t* butf8, size_t* offset0) {
    size_t offset = ((offset0 == nullptr) ? 0 : (*offset0));
    size_t size = asn_octets_unbox(butf8, &offset);
    
    SET_BOX(offset0, offset);

    return std::string(reinterpret_cast<const char*>(butf8 + (offset - size)), size);
}
