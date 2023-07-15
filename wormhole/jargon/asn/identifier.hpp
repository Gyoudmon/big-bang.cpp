#pragma once

#include <cinttypes>

namespace WarGrey::STEM {
    enum class ASNPrimitive {
        Boolean, Integer, /* BitString, OctetString, */ Null,
        /* ObjectIdentifier, ObjectDescriptor, External, */
        Real, Enumerated,
        UTF8_String, IA5_String
    };

    enum class ASNConstructed {
        Sequence
    };

    enum class ASN1TagClass { Universal, Application, ContextSpecific, Private };

    __lambda__ uint8_t asn_identifier_octet(uint8_t tag, bool constructed = false, WarGrey::STEM::ASN1TagClass type = ASN1TagClass::Universal);

    __lambda__ uint8_t asn_identifier_tag(uint8_t octet);
    __lambda__ WarGrey::STEM::ASN1TagClass asn_identifier_class(uint8_t octet);
    __lambda__ bool asn_identifier_constructed(uint8_t octet);

    __lambda__ uint8_t asn_primitive_identifier_octet(WarGrey::STEM::ASNPrimitive type);
    __lambda__ uint8_t asn_constructed_identifier_octet(WarGrey::STEM::ASNConstructed type);
}
