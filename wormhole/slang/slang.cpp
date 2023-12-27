#include "slang.hpp"

#include "../../datum/bytes.hpp"
#include "../../datum/time.hpp"
#include "../../datum/box.hpp"

#include "../checksum/ipv4.hpp"

using namespace GYDM;

/*************************************************************************************************/
/**
 * slang message structure:
 *  / magic number:   2 bytes, for UDP and TCP handshaking, it's constant '#~; otherwise, it's the size of the message 
 *  | version number: 1 byte, default 0
 *  | payload type:   1 byte, hint for decoding the payload
 *  | checksum:       2 bytes, checksum of entire message [algorithm is the same as that used in IP/UDP/TCP]
 *  | [additional fields based on version]
 *  \ [payload that encoded as ASN.1 DER, usually it is an ASN.1 Sequence]
 *
 * for version 1 message, there are two extra fields:
 *  | transaction id: 2 bytes, initialized by client and recopied by server
 *  | response port:  2 bytes, initialized by client for server to respond
 *
 * NOTE
 * The size of header (plus additional fields) should be even
 *  or the checksum algorithm may fail to verify the sum since
 *  it would insert a virtual nil byte before summing the payload
 */

static constexpr size_t slang_version_idx = 2;
static constexpr size_t slang_type_idx = 3; 
static constexpr size_t slang_checksum_idx = 4;
static constexpr uint16_t slang_message_magic = 0x237E; // '#~'

/*
static inline Platform::Array<uint8>^ slang_bytes(IASNSequence* payload) {
	size_t payload_span = payload->payload_span();
	auto basn = ref new Platform::Array<uint8>((unsigned int)(asn_span(payload_span)));

	payload->into_octets((uint8*)basn->Data, 0);

	return basn;
}

static void dispatch_message(const uint8_t* message, unsigned int total, double unboxing_ts, Platform::String^ peer, uint16_t port) {
	uint8_t version = network_uint8_ref(message, 2);
	uint8_t type = network_uint8_ref(message, 3);
	size_t cursor = slang_checksum_idx + slang_checksum_size;
	uint16_t transaction = 0;
	uint16_t response_port = 0;

	switch (version) {
	case 1: {
		transaction = network_uint16_ref(message, cursor);
		cursor += 2;
		response_port = network_uint16_ref(message, cursor);
		cursor += 2;
	}; break;
	}

	{ // extract payload
		const uint8_t* payload = message + cursor;
			
		asn_octets_unbox(message, &cursor);

		if (cursor == total) { // is this necessary?
			double applying_ms = current_inexact_milliseconds();
			long long now_ms = current_milliseconds();

			this->master->notify_message_unboxed(total, applying_ms - unboxing_ts);
			this->logger->log_message(Log::Debug, L"<recieved %u-byte slang message(%s, %u, %u) from %s:%u>",
				total, this->master->message_typename(type)->Data(), transaction, response_port, peer->Data(), port);

			switch (version) {
			case 1: this->master->on_message(now_ms, peer, response_port, transaction, response_port, type, payload); break;
			default: this->master->on_message(now_ms, peer, response_port, type, payload);
		}

			this->master->notify_message_applied(total, current_inexact_milliseconds() - applying_ms);
		} else {
			task_discard(this->logger, L"%s:%d: discard truncated slang message", peer->Data(), port);
		}
	}
}
*/

/*************************************************************************************************/
bool GYDM::is_slang_message(const uint8_t* message, size_t idx) {
	return network_uint16_ref(message, idx) == slang_message_magic;
}

bool GYDM::slang_message_validate(const uint8_t* message, size_t size, size_t idx) {
	uint16_t checksum = network_uint16_ref(message, idx + slang_checksum_idx);

	return ((checksum == 0x0000U) /* checksum is disabled */
			|| (checksum_ipv4(message, idx, idx + size) == 0));
}

size_t GYDM::slang_metainfo_unbox(const uint8_t* message, uint8_t* version, uint8_t* type, size_t idx) {
	SET_BOX(version, network_uint8_ref(message, idx + slang_version_idx));
	SET_BOX(type, network_uint8_ref(message, idx + slang_type_idx));

	return idx + slang_checksum_idx + 2;
}

/*************************************************************************************************/
/*
void slang_write_message(uint8_t* metainfo, const IASNSequence* payload, uint8_t type, uint16_t response_port, uint16_t transaction, bool checksumed) {
	uint8_t version = ((response_port == 0) ? 0 : 1);
	double sending_ms = current_inexact_milliseconds();
	size_t header_size = slang_checksum_idx + slang_checksum_size;

	{ // initialize header
		network_uint16_set(metainfo, 0, slang_message_magic);
		network_uint8_set(metainfo, 2, version);
		network_uint8_set(metainfo, 3, type);
		network_uint16_set(metainfo, slang_checksum_idx, 0); // clear checksum

		switch (version) { // write additional fields
		case 1: {
			network_uint16_set(metainfo, header_size, transaction);
			header_size += 2;
			network_uint16_set(metainfo, header_size, response_port);
			header_size += 2;
		}; break;
		}

		if (checksumed) { // calculate checksum
			unsigned short checksum = 0;

			checksum_ipv4(&checksum, metainfo, 0, header_size);
			// checksum_ipv4(&checksum, payload->Data, 0, payload->Length);

			if (checksum == 0) {
				network_uint16_set(metainfo, slang_checksum_idx, 0xFFFFU);
			} else {
				network_uint16_set(metainfo, slang_checksum_idx, checksum);
			}
		}
	}
}
*/
