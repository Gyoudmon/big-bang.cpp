#include "sequence.hpp"
#include "../../../datum/box.hpp"

using namespace GYDM;

/*************************************************************************************************/
GYDM::IASNSequence::IASNSequence(size_t count) : count(count) {}

size_t GYDM::IASNSequence::span() {
    size_t payload = 0;

    for (size_t idx = 0; idx < this->count; idx++) {        
        payload += asn_span(this->field_payload_span(idx));
    }

    return payload;
}

octets GYDM::IASNSequence::to_octets() {
    size_t ospan = asn_span(this->span());
    octets basn(ospan, '\0');

    this->into_octets(const_cast<uint8_t*>(basn.c_str()), 0);

    return basn;
}

size_t GYDM::IASNSequence::into_octets(uint8_t* octets, size_t offset) {
    size_t pay_span = this->span();

    octets[offset++] = asn_constructed_identifier_octet(ASNConstructed::Sequence);
    offset = asn_length_into_octets(pay_span, octets, offset);

    for (size_t idx = 0; idx < this->count; idx++) {
        offset = this->fill_field(idx, octets, offset);
    }

    return offset;
}

void GYDM::IASNSequence::from_octets(const uint8_t* basn, size_t* offset0) {
    size_t offset = ((offset0 == nullptr) ? 0 : (*offset0));
    size_t size = asn_octets_unbox(basn, &offset);
    size_t position = offset - size;

    SET_BOX(offset0, offset);

    for (size_t idx = 0; idx < this->count; idx++) {
        this->extract_field(idx, basn, &position);
    }
}
