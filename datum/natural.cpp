#include "natural.hpp"

#include "bytes.hpp"
#include "fixnum.hpp"

#include <memory>

using namespace GYDM;

/*************************************************************************************************/
template<typename BYTE>
static size_t natural_from_base16(uint8_t* natural, const BYTE n[], size_t nstart, size_t nend, size_t capacity) {
	size_t slot = capacity - 1;
	size_t payload = 0U;

	do {
		uint8_t lsb = ((nend > nstart) ? byte_to_hexadecimal(_U8(n[--nend]), 0U) : 0U);
		uint8_t msb = ((nend > nstart) ? byte_to_hexadecimal(_U8(n[--nend]), 0U) : 0U);

		natural[slot--] = (msb << 4U | lsb);

		if (natural[slot + 1] > 0) {
			payload = capacity - (slot + 1);
		}
	} while (nstart < nend);

	return payload;
}

template<typename BYTE>
static size_t natural_from_base(uintptr_t base, uint8_t* natural, const BYTE n[], int nstart, int nend, size_t capacity) {
	size_t cursor = capacity - 2;
	size_t payload = 0U;

	natural[capacity - 1] = 0;

	do {
		uint16_t decimal = byte_to_decimal(_U8(n[nstart++]), 0U);
		
		for (size_t idx = capacity - 1; idx > cursor; idx--) {
			uint16_t digit = static_cast<uint16_t>(natural[idx] * base) + decimal;

			natural[idx] = _U8(digit & 0xFFU);
			decimal = digit >> 8;
		}

		if (decimal > 0) {
			payload = capacity - cursor;
			natural[cursor--] = _U8(decimal);
		}
	} while (nstart < nend);

	return payload;
}

static inline size_t fixnum_length(size_t payload, size_t modulus) {
	return payload / modulus + ((payload % modulus > 0) ? 1 : 0);
}

template<typename UI>
static UI fixnum_ref(const uint8_t* natural, size_t payload, size_t capacity, int slot_idx, size_t offset, size_t size) {
	UI n = 0U;

	if (payload > 0U) {
		size_t start0 = capacity - payload;
		size_t start, end;

		if (slot_idx >= 0) {
			start = capacity - (fixnum_length(payload, size) - slot_idx) * size;
		} else {
			start = capacity + slot_idx * size;
		}

		start += offset;
		end = start + size;

		start = fxmax(start0, start);
		end = fxmin(end, capacity);

		for (size_t idx = start; idx < end; idx++) {
			n = (n << 8U) ^ natural[idx];
		}
	}

	return n;
}

template<typename Q, typename C>
static Q u_vq_hat(uint8_t u, uint8_t v, Q q_hat, C* carry, uint8_t* borrowed) {
	Q diff = v * q_hat + (*carry) + (*borrowed);

	if (diff <= 0xFFU) {
		(*carry) = 0U;
	} else {
		(*carry) = diff >> 8U;
		diff &= 0xFFU;
	}

	if (u < diff) {
		diff = 0x100U + u - diff;
		(*borrowed) = 1U;
	} else {
		diff = u - diff;
		(*borrowed) = 0U;
	}

	return diff;
}

template<typename Q>
static inline Q add(uint8_t lhs, uint8_t rhs, Q* carry) {
	Q digit = lhs + rhs + (*carry);

	if (digit > 0xFFU) {
		digit &= 0xFFU;
		(*carry) = 1U;
	} else {
		(*carry) = 0U;
	}

	return digit;
}

#define NATURAL_MODULAR_EXPT(self, me, b, n) { \
	self->quotient_remainder(n, self); \
\
	for (size_t bidx = 1; bidx < b.payload; bidx++) { \
		uint8_t bself = b.natural[b.capacity - bidx]; \
\
		for (uint16_t bmask = 0b1U; bmask < 0x100U; bmask <<= 1U) { \
			if ((bself & bmask) > 0U) { \
				me.operator*=(*self).quotient_remainder(n, &me); \
			} \
\
			self->operator*=(*self).quotient_remainder(n, self); \
		} \
	} \
    /* All leading zeros of the leading byte do not change the result but do make some useless computations. */ \
	natural_modular_expt(self, &me, b.natural[b.capacity - b.payload], n); \
}

template<typename N>
static void natural_modular_expt(Natural* self, Natural* me, uint64_t b, N n) {
	/** NOTE
	 * This function also serves the NATURAL_MODULAR_EXPT for the leading byte of b's payload
	 * Thus, invokers of this function should do all the preparations.
	 */

	do {
		if ((b & 0b1U) > 0U) {
			me->operator*=(*self).quotient_remainder(n, me);
		}

		self->operator*=(*self).quotient_remainder(n, self);
		b >>= 1U;
	} while (b > 0U);

	(*self) = (*me);
}

/*************************************************************************************************/
GYDM::Natural::~Natural() noexcept {
	if (this->natural != nullptr) {
		delete [] this->natural;
	}
}

GYDM::Natural::Natural() : Natural(0ULL) {}

GYDM::Natural::Natural(uint64_t n) : natural(nullptr), capacity(sizeof(uint64_t)), payload(0U) {
	this->natural = this->malloc(this->capacity);
	this->replaced_by_fixnum(n);
}

GYDM::Natural::Natural(const bytes& nstr, size_t nstart, size_t nend)
	: Natural(nstr.c_str(), nstart, ((nend <= nstart) ? nstr.size() : nend)) {}

GYDM::Natural::Natural(const std::string& nstr, size_t nstart, size_t nend)
	: Natural(reinterpret_cast<const uint8_t*>(nstr.c_str()), nstart, ((nend <= nstart) ? nstr.size() : nend)) {}

GYDM::Natural::Natural(const std::wstring& nstr, size_t nstart, size_t nend)
	: Natural(reinterpret_cast<const uint16_t*>(nstr.c_str()), nstart, ((nend <= nstart) ? nstr.size() : nend)) {}

GYDM::Natural::Natural(uintptr_t base, const bytes& nstr, size_t nstart, size_t nend)
	: Natural(base, nstr.c_str(), nstart, ((nend <= nstart) ? nstr.size() : nend)) {}

GYDM::Natural::Natural(uintptr_t base, const std::string& nstr, size_t nstart, size_t nend)
	: Natural(base, reinterpret_cast<const uint8_t*>(nstr.c_str()), nstart, ((nend <= nstart) ? nstr.size() : nend)) {}

GYDM::Natural::Natural(uintptr_t base, const std::wstring& nstr, size_t nstart, size_t nend)
	: Natural(base, reinterpret_cast<const uint16_t*>(nstr.c_str()), nstart, ((nend <= nstart) ? nstr.size() : nend)) {}

bool GYDM::Natural::is_zero() const {
	return ((this->payload == 0)
		|| ((this->payload == 1) // redundant checking
			&& (this->natural[this->capacity - 1] == 0)));
}

bool GYDM::Natural::is_one() const {
	return ((this->payload == 1)
		&& (this->natural[this->capacity - 1] == 1));
}

bool GYDM::Natural::is_fixnum() const {
	return (this->payload <= sizeof(uint64_t));
}

bool GYDM::Natural::is_odd() const {
	return ((this->payload > 0U) && ((this->natural[this->capacity - 1U] & 0x1U) == 0x1U));
}

bool GYDM::Natural::is_even() const {
	return ((this->payload == 0U) || ((this->natural[this->capacity - 1U] & 0x1U) == 0x0U));
}

size_t GYDM::Natural::length() const {
	return this->payload;
}

size_t GYDM::Natural::integer_length(uint8_t alignment) const {
	size_t s = 0;

	if (this->payload > 0) {
		s = (this->payload - 1) * 8;
		s += ::integer_length(this->natural[this->capacity - this->payload]);
	}

	if (alignment > 0) {
		if (s > 0) {
			size_t remainder = s % alignment;

			if (remainder > 0) {
				s += (alignment - remainder);
			}
		} else {
			s = alignment;
		}
	}

	return s;
}

bytes GYDM::Natural::to_bytes() const {
	return bytes(this->natural + (this->capacity - this->payload), this->payload);
}

bytes GYDM::Natural::to_hexstring(char ten) const {
	bytes hex(fxmax(_U32(this->payload), 1U) * 2, '0');
	size_t payload_idx = this->capacity - this->payload;
	size_t msb_idx = 0U;

	for (size_t idx = 0; idx < this->payload; idx++) {
		uint8_t ubyte = this->natural[idx + payload_idx];
		
		if (ubyte <= 0xF) {
			msb_idx++;
			hex[msb_idx++] = hexadecimal_to_byte(ubyte, ten);
		} else {
			hex[msb_idx++] = hexadecimal_to_byte(ubyte >> 4, ten);
			hex[msb_idx++] = hexadecimal_to_byte(ubyte & 0xF, ten);
		}
	}

	return hex;
}

bytes GYDM::Natural::to_binstring(uint8_t alignment) const {
	size_t bsize = this->integer_length(alignment);
	bytes bin(bsize, '0');
	size_t lsb_idx = bsize - 1U;

	for (size_t idx = 0; idx < this->payload; idx++) {
		uint8_t ubyte = this->natural[this->capacity - idx - 1];
		
		for (size_t bits = 0; bits < 8; bits++) {
			if ((ubyte & (0x1U << bits)) > 0) {
				bin[lsb_idx] = '1';
			}
			
			if (lsb_idx > 0) {
				lsb_idx--;
			} else {
				break;
			}
		};
	}

	return bin;
}

/*************************************************************************************************/
GYDM::Natural::Natural(const Natural& n) : natural(nullptr), capacity(fxmax(n.payload, sizeof(uint64_t))), payload(n.payload) { // copy constructor
	this->natural = this->malloc(this->capacity);

	if (this->payload > 0) {
		memcpy(this->natural + (this->capacity - this->payload),
			n.natural + (n.capacity - n.payload),
			this->payload);
	}
}

GYDM::Natural::Natural(Natural&& n) : natural(n.natural), capacity(n.capacity), payload(n.payload) { // move constructor
	n.on_moved();
}

Natural& GYDM::Natural::operator=(uint64_t n) {
	this->replaced_by_fixnum(n);

	return (*this);
}

Natural& GYDM::Natural::operator=(const Natural& n) { // copy assignment operator
	if (this != &n) {
		if (n.payload > this->capacity) {
			if (this->natural != nullptr) {
				delete[] this->natural;
			}

			this->capacity = n.payload;
			this->natural = this->malloc(this->capacity);
		}

		this->payload = n.payload;
		if (this->payload > 0) {
			size_t payload_idx = this->capacity - this->payload;
			size_t n_idx = n.capacity - this->payload;

			memcpy(this->natural + payload_idx, n.natural + n_idx, this->payload);
		}
	}

	return (*this);
}

Natural& GYDM::Natural::operator=(Natural&& n) noexcept { // move assignment operator
	if (this->natural != nullptr) {
		delete[] this->natural;
	}

	this->natural = n.natural;
	this->capacity = n.capacity;
	this->payload = n.payload;

	n.on_moved();

	return (*this);
}

void GYDM::Natural::on_moved() {
	this->capacity = 0U;
	this->payload = 0U;
	this->natural = nullptr;
}

/*************************************************************************************************/
int GYDM::Natural::compare(uint64_t rhs) const {
	int cmp = 0;

	if (this->is_fixnum()) {
		for (size_t slot = sizeof(uint64_t); slot > 0; slot--) {
			uint8_t ldigit = ((slot > this->payload) ? 0U : this->natural[this->capacity - slot]);
			uint8_t rdigit = (rhs >> ((slot - 1) * 8)) & 0xFFU;

			if (ldigit == rdigit) {
				continue;
			}

			cmp = ((ldigit < rdigit) ? -1 : 1);
			break;
		}
	} else {
		cmp = 1;
	}

	return cmp;
}

int GYDM::Natural::compare(const Natural& rhs) const {
	int cmp = ((this->payload < rhs.payload) ? -1 : +1);

	if (this->payload == rhs.payload) {
		cmp = memcmp(this->natural + (this->capacity - this->payload),
			rhs.natural + (rhs.capacity - rhs.payload),
			this->payload);
	}

	return cmp;
}

/*************************************************************************************************/
Natural& GYDM::Natural::operator++() {
	this->add_digit(1U);

	return (*this);
}

Natural& GYDM::Natural::operator--() {
	this->decrease_from_slot(1U);

	return (*this);
}

Natural& GYDM::Natural::operator+=(uint64_t rhs) {
	if (rhs > 0xFFU) {
		size_t digits = fxmax(this->payload, sizeof(uint64_t));
		size_t addend_idx = this->capacity - 1;
		
		if (this->capacity <= digits) {
			this->recalloc(digits + 1);
			addend_idx = this->capacity - 1;
		}

		do {
			uint16_t digit = this->natural[addend_idx] + (rhs & 0xFFU);

			if (digit > 0xFF) {
				this->natural[addend_idx] = _U8(digit & 0xFFU);
				rhs = (rhs >> 8U) + 1U;
			} else {
				this->natural[addend_idx] = _U8(digit);
				rhs >>= 8U;
			}

			addend_idx--;
		} while (rhs > 0U);

		this->payload = fxmax(this->payload, (this->capacity - addend_idx - 1));
	} else {
		this->add_digit(_U8(rhs));
	}

	return (*this);
}

Natural& GYDM::Natural::operator+=(const Natural& rhs) {
	// NOTE: the rhs may refer to (*this)

	if (rhs.payload > 1U) {
		size_t digits = fxmax(this->payload, rhs.payload);
		size_t cdigits = fxmin(this->payload, rhs.payload);
		size_t ddigits = digits - cdigits;
		size_t lcapacity = this->capacity;
		uint8_t* lsrc = this->natural;
		uint16_t carry = 0U;

		if (this->capacity <= digits) { // no `this->expand`ing to avoid redundant copy
			this->capacity = digits + 1;
			this->natural = this->malloc(this->capacity);
		}

		for (size_t idx = 1; idx <= cdigits; idx++) {
			this->natural[this->capacity - idx] = (uint8_t)add(lsrc[lcapacity - idx], rhs.natural[rhs.capacity - idx], &carry);
		}

		if (ddigits > 0) {
			if (this->payload < rhs.payload) {
				memcpy(this->natural + (this->capacity - digits), rhs.natural + (rhs.capacity - rhs.payload), ddigits);
			} else if (lsrc != this->natural) {
				memcpy(this->natural + (this->capacity - digits), lsrc + (lcapacity - this->payload), ddigits);
			}

			if (carry == 1) {
				for (size_t idx = cdigits + 1; idx <= digits; idx++) {
					size_t slot = this->capacity - idx;

					if (this->natural[slot] == 0xFFU) {
						this->natural[slot] = 0U;
					} else {
						this->natural[slot] += 1;
						carry = 0U;
						break;
					}
				}
			}
		}

		if (carry == 1) {
			this->payload = digits + carry;
			this->natural[this->capacity - this->payload] = 1;
		} else {
			this->payload = digits;
		}

		if (this->natural != lsrc) {
			delete[] lsrc;
		}
	} else if (rhs.payload == 1U) {
		this->add_digit(rhs.natural[rhs.capacity - 1U]);
	}

	return (*this);
}

Natural& GYDM::Natural::operator-=(uint64_t rhs) {
	if ((!this->is_zero()) && (rhs > 0U)) {
		size_t rhs_payloadp1 = 1U;
		uint8_t borrowed = 0U;

		while ((rhs_payloadp1 <= this->payload) && (rhs > 0U)) {
			uint16_t minuend = this->natural[this->capacity - rhs_payloadp1];
			uint16_t subtrachend = (rhs & 0xFFU) + borrowed;

			if (minuend >= subtrachend) {
				this->natural[this->capacity - rhs_payloadp1] = (uint8_t)(minuend - subtrachend);
				borrowed = 0U;
			} else {
				this->natural[this->capacity - rhs_payloadp1] = (uint8_t)(0x100U + minuend - subtrachend);
				borrowed = 1U;
			}

			rhs >>= 8U;
			rhs_payloadp1++;
		}

		if (rhs > 0U) {
			this->bzero();
		} else if (borrowed > 0U) {
			if (this->payload + 1U == rhs_payloadp1) {
				this->bzero();
			} else {
				this->decrease_from_slot(rhs_payloadp1);
				this->skip_leading_zeros(this->payload);
			}
		} else {
			this->skip_leading_zeros(this->payload);
		}
	}

	return (*this);
}

Natural& GYDM::Natural::operator-=(const Natural& rhs) {
	if (!rhs.is_zero()) {
		if (this->payload >= rhs.payload) {
			uint8_t borrowed = 0U;

			for (size_t idx = 1; idx <= rhs.payload; idx++) {
				uint16_t minuend = this->natural[this->capacity - idx];
				uint16_t subtrachend = rhs.natural[rhs.capacity - idx] + borrowed;

				if (minuend >= subtrachend) {
					this->natural[this->capacity - idx] = (uint8_t)(minuend - subtrachend);
					borrowed = 0U;
				} else {
					this->natural[this->capacity - idx] = (uint8_t)(0x100U + minuend - subtrachend);
					borrowed = 1U;
				}
			}

			if (borrowed > 0U) {
				if (this->payload == rhs.payload) {
					this->bzero();
				} else {
					this->decrease_from_slot(rhs.payload + 1U);
					this->skip_leading_zeros(this->payload);
				}
			} else if (this->payload == rhs.payload) {
				this->skip_leading_zeros(this->payload);
			}
		} else {
			this->bzero();
		}
	}

	return (*this);
}

Natural& GYDM::Natural::operator*=(uint64_t rhs) {
	if (!this->is_zero()) {
		if (rhs > 0xFFU) {
			size_t digits = this->payload + sizeof(uint64_t);
			uint8_t* product = this->natural;
			size_t j = 0U;

			if (this->capacity < digits + this->payload) {
				product = this->malloc(digits);
			}

			memset(product + (digits - this->payload), '\0', this->payload);

			do {
				uint16_t carry = 0U;
				uint8_t v = (uint8_t)(rhs & 0xFFU);

				if (v > 0) {
					for (size_t i = 1; i <= this->payload; i++) {
						size_t ij = digits - i - j;
						uint16_t digit = this->natural[this->capacity - i] * v + product[ij] + carry;

						product[ij] = (uint8_t)(digit & 0xFFU);
						carry = (digit >> 8);
					}
				}

				product[digits - this->payload - (++j)] = (uint8_t)carry;
				rhs >>= 8;
			} while (rhs > 0U);

			if (this->natural != product) {
				delete[] this->natural;
				this->natural = product;
				this->capacity = digits;
			} else {
				memmove(this->natural + (this->capacity - digits), this->natural, digits);
			}

			this->skip_leading_zeros(this->payload + j);
		} else {
			if (rhs == 0U) {
				this->bzero();
			} else {
				this->times_digit((uint8_t)rhs);
			}
		}
	}

	return (*this);
}

Natural& GYDM::Natural::operator*=(const Natural& rhs) {
	// NOTE: the rhs may refer to (*this)

	if (!this->is_zero()) {
		if (rhs.payload > 1U) {
			size_t digits = this->payload + rhs.payload;
			uint8_t* product = this->natural;

			if (this->capacity < this->payload + digits) {
				product = this->malloc(digits);
			}
			
			memset(product + (digits - this->payload), '\0', this->payload);

			for (size_t j = 1; j <= rhs.payload; j++) {
				uint16_t carry = 0U;
				
				if (rhs.natural[rhs.capacity - j] > 0) {
					for (size_t i = 1; i <= this->payload; i++) {
						size_t ij = digits - i - j + 1;
						uint16_t digit = this->natural[this->capacity - i] * rhs.natural[rhs.capacity - j] + product[ij] + carry;

						product[ij] = (uint8_t)(digit & 0xFFU);
						carry = (digit >> 8);
					}
				}

				product[digits - this->payload - j] = (uint8_t)carry;
			}

			if (this->natural != product) {
				delete[] this->natural;
				this->natural = product;
				this->capacity = digits;
			} else {
				memmove(this->natural + (this->capacity - digits), this->natural, digits);
			}

			this->skip_leading_zeros(digits);
		} else {
			if (rhs.is_zero()) {
				this->bzero();
			} else {
				this->times_digit(rhs.natural[rhs.capacity - 1U]);
			}
		}
	}

	return (*this);
}

Natural& GYDM::Natural::quotient_remainder(uint64_t rhs, Natural* oremainder) {
	// WARNING: `rhs` may refer to `(*this)`, `oremainder` may point to `this`.
	
	if (!this->is_zero()) {
		if (rhs > 0xFFU) {
			size_t payload_idx = this->capacity - this->payload;
			size_t divisor_payload = fixnum_length(::integer_length(rhs), 8U);
			size_t quotient_idx = payload_idx + divisor_payload - 1U;

			if (this->payload >= divisor_payload) {
				uint64_t remainder = 0U;
				size_t idx = payload_idx;

				while (idx < quotient_idx) {
					remainder = (remainder << 8U) ^ this->natural[idx++];
				}

				do {
					remainder = (remainder << 8U) ^ this->natural[idx];

					if (remainder < rhs) {
						this->natural[idx++] = 0U;
					} else {
						this->natural[idx++] = (uint8_t)(remainder / rhs);
						remainder = remainder % rhs;
					}
				} while (idx < this->capacity);

				this->skip_leading_zeros(this->capacity - quotient_idx);
				
				if (oremainder != nullptr) {
					(*oremainder) = remainder;
				}
			} else if (oremainder == nullptr) {
				this->bzero();
			} else if (oremainder != nullptr) {	
				if (this != oremainder) {
					oremainder->operator=(*this);
					this->bzero();
				}
			}
		} else if (rhs > 0U) {
			this->divide_digit((uint8_t)rhs, oremainder);
		}
	} else if (oremainder != nullptr) {
		oremainder->bzero();
	}

	return (*this);
}

Natural& GYDM::Natural::quotient_remainder(const Natural& rhs, Natural* oremainder) {
	// Algorithm: classic method that to estimate the pencil-and-paper long division.
	
	/** Theorem
	 * u = (u_n u_n-1 ... u_1 u_0)b
	 * v =     (v_n-1 ... v_1 v_0)b
	 * where u/v < b <==> u/b < v <==> floor(u/b) < v
	 * thus, q = floor(u/v), r = u - qv
	 *
	 * In order to eliminate the guesswork for q, let's find a
	 *     q^ = min(floor((u_n * b + u_n-1) / v_n-1), b - 1)
	 *   thus
	 *   A). q^ >= q;
	 *   B). v_n-1 >= floor(b/2) ==> q^-2 <= q <= q^
	 */

	// WARNING: `rhs` may refer to `(*this)`, `oremainder` may point to `this`.

	if (!this->is_zero()) {
		if (!rhs.is_fixnum()) {
			int cmp = this->compare(rhs);

			if (cmp > 0) { // `this` will be the remainder
				size_t quotient_size = (this->payload - rhs.payload) + 1U;
				uint8_t* quotient = ((this == oremainder) ? nullptr : this->natural);
				uint8_t vn_1 = rhs.natural[rhs.capacity - rhs.payload];
				uint8_t vn_2 = rhs.natural[rhs.capacity - rhs.payload + 1U];
				uint8_t shifts = 0U;

				{ // normalization
					if (vn_1 < 0x80U) { // 0x80 is `base/2`
						uint32_t virtual_shifted = (vn_1 << 16U) ^ (vn_2 << 8U) ^ rhs.natural[rhs.capacity - rhs.payload + 2U];

						while (virtual_shifted < 0x80000000U) {
							virtual_shifted <<= 1U;
							shifts += 1U;
						}

						vn_1 = (virtual_shifted >> 24U) & 0xFFU;
						vn_2 = (virtual_shifted >> 16U) & 0xFFU;
					}

					{ // m+n-bytes dividend / n-bytes divisor = m+1-bytes quotient ... n-bytes remainder
						if (quotient == nullptr) {
							if (this->capacity == this->payload) {
								this->expand(1U);
							}
						} else {
							if (this->capacity < this->payload + quotient_size + 1U) {
								this->expand(this->payload + quotient_size + 1U - this->capacity);
								quotient = this->natural;
							}
						}

						this->payload += 1U;
						this->natural[this->capacity - this->payload] = '\0';
					}
				}

				{ // long division
					size_t quotient_idx0 = this->capacity - quotient_size;

					for (size_t j_idx = 0; j_idx < quotient_size; j_idx++) {
						size_t j = quotient_idx0 + j_idx;
						size_t jn_idx = quotient_idx0 + j_idx - rhs.payload;
						uint64_t ujn_0 = this->natural[jn_idx + 0U];
						uint64_t ujn_1 = this->natural[jn_idx + 1U];
						uint64_t ujn_2 = this->natural[jn_idx + 2U];
						uint64_t ujnb_ujn_1 = (ujn_0 << 8U) ^ ujn_1;
						uint64_t q_hat = 0U;

						if (shifts > 0U) {
							uint64_t virtual_shifted = ((ujnb_ujn_1 << 16U) ^ (ujn_2 << 8U) ^ this->natural[jn_idx + 3U]) << shifts;

							ujnb_ujn_1 = (virtual_shifted >> 24U) & 0xFFFFU;
							ujn_2 = (virtual_shifted >> 16U) & 0xFFU;
						}

						{ // The q^ found here is guaranteed accurate (almost) 
							uint64_t r_hat = ujnb_ujn_1 % vn_1;

							q_hat = ujnb_ujn_1 / vn_1;

							if (q_hat > 0xFFU) {
								r_hat += (vn_1 * (q_hat - 0xFFU));
								q_hat = 0xFFU;
							}

							while ((q_hat * vn_2) > ((r_hat << 8U) ^ ujn_2)) {
								q_hat -= 1U;
								r_hat += vn_1;
							}
						}

						{ // in-place: r = u - v(q^)
							size_t divisor_diff = rhs.capacity - j - 1U;
							uint64_t carry = 0U;
							uint8_t borrowed = 0U;

							for (size_t idx = j; idx > jn_idx; idx--) {
								this->natural[idx] = (uint8_t)u_vq_hat(this->natural[idx], rhs.natural[idx + divisor_diff], q_hat, &carry, &borrowed);
							}
							
							this->natural[jn_idx] = (uint8_t)u_vq_hat(this->natural[jn_idx], 0U, q_hat, &carry, &borrowed);

							if (borrowed > 0U) { // the probability of this case is `2/base`.
								q_hat -= 1U;
								carry = 0U;

								for (size_t idx = j; idx > jn_idx; idx--) {
									this->natural[idx] = (uint8_t)add(this->natural[idx], rhs.natural[idx + divisor_diff], &carry);
								}

								this->natural[jn_idx] = (uint8_t)add(this->natural[jn_idx], 0U, &carry);
								// Ignore the carry here since it is triggered by the borrowing.
							}

							if (quotient != nullptr) {
								quotient[j_idx] = (uint8_t)q_hat;
							}
						}
					}
				}

				if (oremainder != nullptr) {
					this->skip_leading_zeros(rhs.payload);

					if (this != oremainder) {
						(*oremainder) = (*this);
					}
				}

				if (quotient != nullptr) { // <==> (this != oremainder)
					memmove(this->natural + (this->capacity - quotient_size), this->natural, quotient_size);
					this->skip_leading_zeros(quotient_size);
				}
			} else if (cmp == 0) {
				this->payload = 1U;
				this->natural[this->capacity - 1U] = 1U;

				if (oremainder != nullptr) {
					oremainder->bzero();
				}
			} else if (oremainder == nullptr) {
				this->bzero();
			} else if (oremainder != nullptr) {
				if (this != oremainder) {
					oremainder->operator=(*this);
					this->bzero();
				}
			}
		} else if (rhs.payload > 1U) {
			this->quotient_remainder(rhs.fixnum64_ref(0), oremainder);
		} else if (rhs.payload == 1U) {
			this->divide_digit(rhs.natural[rhs.capacity - 1U], oremainder);
		}
	} else if (oremainder != nullptr) {
		oremainder->bzero();
	}

	return (*this);
}

Natural& GYDM::Natural::expt(uint64_t n) {
	Natural Z = (*this);
	
	(*this) = 1U;

	while (n > 0U) {
		if ((n & 0x1U) > 0) {
			this->operator*=(Z);
		}

		n >>= 1U;
		Z *= Z;
	}

	return (*this);
}

Natural& GYDM::Natural::expt(const Natural& n) {
	// Algorithm: Right-to-Left binary method (also known as "Russian Peasant Method")
	Natural Z = (*this);
	Natural N = n;

	(*this) = 1U;

	while (N.payload > 0U) {
		if (N.is_odd()) {
			this->operator*=(Z);
		}

		N >>= 1U;
		Z *= Z;
	}

	return (*this);
}

Natural& GYDM::Natural::modular_expt(uint64_t b, uint64_t n) {
	if (b > 0U) {
		size_t product_size = sizeof(uint64_t) * 2U;
		Natural me = 1U;
		
		me.smart_prealloc(product_size);
		this->smart_prealloc(product_size);
		this->quotient_remainder(n, this);
		natural_modular_expt(this, &me, b, n);
	} else if (b == 0U) {
		(*this) = 1U;
	} else {
		this->quotient_remainder(n, this);
	}

	return (*this);
}

Natural& GYDM::Natural::modular_expt(const Natural& b, uint64_t n) {
	if (b.is_fixnum()) {
		this->modular_expt(b.fixnum64_ref(0U), n);
	} else {
		size_t product_size = sizeof(uint64_t) * 2U;
		Natural me = 1U;

		me.smart_prealloc(product_size);
		this->smart_prealloc(product_size);
		NATURAL_MODULAR_EXPT(this, me, b, n);
	}

	return (*this);
}

Natural& GYDM::Natural::modular_expt(const Natural& b, const Natural& n) {
	// Algorithm: repeated modular multiplication and squaring (the idea is the same as `expt`)

	/** Theorem
	 * f(a, b)
	 *   = 1 % n,              b = 0;
	 *   = a % n,              b = 1;
	 *   = f^2(a, b >> 1) % n, b is even;
	 *   = a*f(a, b - 1) % n,  b is odd;
	 */

	bool fixed_b = b.is_fixnum();
	bool fixed_n = n.is_fixnum();

	if (fixed_b || fixed_b) {
		if (fixed_b && fixed_b) {
			this->modular_expt(b.fixnum64_ref(0U), n.fixnum64_ref(0U));
		} else if (fixed_b) {
			this->modular_expt(b.fixnum64_ref(0U), n);
		} else if (fixed_n) {
			this->modular_expt(b, n.fixnum64_ref(0U));
		}
	} else {
		size_t product_size = n.payload * 2U;
		Natural me = 1U;

		me.smart_prealloc(product_size);
		this->smart_prealloc(product_size);
		NATURAL_MODULAR_EXPT(this, me, b, n);
	}

	return (*this);
}

Natural& GYDM::Natural::modular_expt(uint64_t b, const Natural& n) {
	if (b > 0U) {
		if (n.is_fixnum()) {
			this->modular_expt(b, n.fixnum64_ref(0));
		} else {
			size_t product_size = n.payload * 2U;
			Natural me = 1U;

			me.smart_prealloc(product_size);
			this->smart_prealloc(product_size);
			this->quotient_remainder(n, this);
			natural_modular_expt(this, &me, b, n);
		}
	} else if (b == 0U) {
		(*this) = 1U;
	} else {
		this->quotient_remainder(n, this);
	}

	return (*this);
}

/*************************************************************************************************/
Natural GYDM::Natural::operator~() {
	Natural ones_complement(*this);

	
	for (size_t idx = ones_complement.capacity - ones_complement.payload; idx < ones_complement.capacity; idx++) {
		ones_complement.natural[idx] = ~ones_complement.natural[idx];
	}

	ones_complement.skip_leading_zeros(ones_complement.payload);

	return ones_complement;
}

Natural& GYDM::Natural::operator<<=(uint64_t rhs) {	
	if ((!this->is_zero()) && (rhs > 0U)) {
		size_t shift_byts = (size_t)(rhs / 8);
		size_t shift_bits = (size_t)(rhs % 8);
		size_t shift_load = this->payload + shift_byts;
		size_t original_idx0 = this->capacity - this->payload;
		size_t total = shift_load;

		if (shift_bits == 0U) {
			if (this->capacity < total) {
				this->recalloc(total, shift_byts);
			} else if (shift_byts > 0U) {
				memmove(this->natural + (this->capacity - shift_load), this->natural + original_idx0, this->payload);
				memset(this->natural + (this->capacity - shift_byts), '\0', shift_byts);
			}
		} else {
			const uint8_t* src = this->natural;
			size_t shift_diff = this->capacity - shift_load - original_idx0;
			
			if (((uint16_t)(src[original_idx0]) << shift_bits) > 0xFFU) {
				total++;
			}

			if (this->capacity < total) { // no `this->expand`ing to avoid redundant copy 
				shift_diff += (total - this->capacity);

				this->capacity = total;
				this->natural = this->malloc(this->capacity);
				
				/** NOTE
				 * There is no need to set other slots of `natural` to zero since
				 *   after shifting all slots will be set by algorithm.
				 */
			}

			if ((shift_byts > 0) || (total > shift_load)) {
				this->natural[this->capacity - total] = '\0';
			}

			for (size_t idx = original_idx0; idx < original_idx0 + this->payload; idx++) {
				uint16_t digit = ((uint16_t)(src[idx]) << shift_bits);
				size_t shift_idx = idx + shift_diff;

				if (digit <= 0xFFU) {
					this->natural[shift_idx] = (uint8_t)digit;
				} else {
					this->natural[shift_idx] = (uint8_t)(digit & 0xFFU);
					this->natural[shift_idx - 1] ^= (uint8_t)(digit >> 8U);
				}
			}

			if (shift_byts > 0U) {
				memset(this->natural + (this->capacity - shift_byts), '\0', shift_byts);
			}

			if (src != this->natural) {
				delete[] src;
			}
		}

		this->payload = total;
	}

	return (*this);
}

Natural& GYDM::Natural::operator>>=(uint64_t rhs) {
	if ((!this->is_zero()) && (rhs != 0U)) {
		size_t shift_byts = (size_t)(rhs / 8);
		
		if (this->payload <= shift_byts) {
			this->bzero();
		} else {
			size_t shift_bits = (size_t)(rhs % 8);
			size_t shift_cbits = 8 - shift_bits;
			size_t original_idx0 = this->capacity - this->payload;
			
			this->payload -= shift_byts;

			if (shift_bits == 0U) {
				memmove(this->natural + (this->capacity - this->payload), this->natural + original_idx0, this->payload);
				//memset(this->natural + original_idx0, '\0', shift_byts); // reduntant
			} else {
				size_t shift_endp1 = original_idx0 + this->payload;
				size_t shift_diff = this->capacity - shift_endp1 - 1;
				size_t mask = (1 << shift_bits) - 1;

				this->natural[this->capacity - 1] = (this->natural[shift_endp1 - 1] >> shift_bits);
				
				for (size_t idx = shift_endp1 - 1; idx > original_idx0; idx--) {
					size_t shift_idx = idx + shift_diff;
					uint8_t digit = this->natural[idx - 1];
					uint8_t tail = digit & mask;
					
					this->natural[shift_idx] = (digit >> shift_bits);

					if (tail > 0U) {
						this->natural[shift_idx + 1] ^= (tail << shift_cbits);
					}
				}

				// reduntant zeroing since other operators do not assume it
				//if (shift_byts > 0U) {
				//	memset(this->natural + original_idx0, '\0', shift_byts);
				//}

				if (this->natural[this->capacity - this->payload] == 0U) {
					this->payload--;
				}
			}
		}
	}

	return (*this);
}

Natural& GYDM::Natural::operator&=(uint64_t rhs) {
	size_t idx0 = this->capacity - fxmin(this->payload, sizeof(uint64_t));

	this->payload = 0U;
	for (size_t idx = this->capacity; (rhs > 0U) && (idx > idx0); idx--) {
		size_t payload_idx = idx - 1U;

		this->natural[payload_idx] &= (rhs & 0xFFU);

		if (this->natural[payload_idx] > 0U) {
			this->payload = this->capacity - payload_idx;
		}

		rhs >>= 8ULL;
	}

	return (*this);
}

Natural& GYDM::Natural::operator&=(const Natural& rhs) {
	size_t upsize = fxmin(this->payload, rhs.payload);

	this->payload = 0U;
	for (size_t idx = 1; idx <= upsize; idx++) {
		size_t payload_idx = this->capacity - idx;

		this->natural[payload_idx] &= rhs.natural[rhs.capacity - idx];

		if (this->natural[payload_idx] > 0U) {
			this->payload = this->capacity - payload_idx;
		}
	}

	return (*this);
}

Natural& GYDM::Natural::operator|=(uint64_t rhs) {
	if (rhs > 0U) {
		size_t digits = fxmax(this->payload, sizeof(uint64_t));
		size_t nidx = this->capacity - 1;

		if (this->capacity < digits) {
			this->recalloc(digits);
			nidx = this->capacity - 1;
		} else {
			memset(this->natural, '\0', this->capacity - this->payload);
		}

		do {
			this->natural[nidx--] |= (rhs & 0xFFU);
			rhs >>= 8U;
		} while (rhs > 0U);

		this->payload = fxmax(this->payload, (this->capacity - nidx - 1));
	}

	return (*this);
}

Natural& GYDM::Natural::operator|=(const Natural& rhs) {
	if (!rhs.is_zero()) {
		size_t digits = fxmax(this->payload, rhs.payload);
		size_t cdigits = fxmin(this->payload, rhs.payload);
		size_t ddigits = digits - cdigits;
		size_t lcapacity = this->capacity;
		uint8_t* lsrc = this->natural;
		
		if (this->capacity < digits) {
			this->capacity = digits;
			this->natural = this->malloc(this->capacity);
		}

		for (size_t idx = 1; idx <= cdigits; idx++) {
			this->natural[this->capacity - idx] = (lsrc[lcapacity - idx] | rhs.natural[rhs.capacity - idx]);
		}

		if (ddigits > 0U) {
			if (this->payload < rhs.payload) {
				memcpy(this->natural + (this->capacity - digits), rhs.natural + (rhs.capacity - rhs.payload), ddigits);
			} else if (lsrc != this->natural) {
				memcpy(this->natural + (this->capacity - digits), lsrc + (lcapacity - this->payload), ddigits);
			}
		}

		this->payload = digits;
		
		if (this->natural != lsrc) {
			delete[] lsrc;
		}
	}

	return (*this);
}

Natural& GYDM::Natural::operator^=(uint64_t rhs) {
	if (rhs > 0U) {
		size_t digits = fxmax(this->payload, sizeof(uint64_t));
		size_t nidx = this->capacity - 1;

		if (this->capacity < digits) {
			this->recalloc(digits);
			nidx = this->capacity - 1;
		} else {
			memset(this->natural, '\0', this->capacity - this->payload);
		}

		do {
			this->natural[nidx--] ^= (rhs & 0xFFU);
			rhs >>= 8U;
		} while (rhs > 0U);

		{ // resolve the payload
			size_t rhs_payload = (this->capacity - nidx - 1);

			if (this->payload == rhs_payload) {
				this->skip_leading_zeros(rhs_payload);
			} else if (this->payload < rhs_payload) {
				this->payload = rhs_payload;
			}
		}
	}

	return (*this);
}

Natural& GYDM::Natural::operator^=(const Natural& rhs) {
	if (!rhs.is_zero()) {
		size_t digits = fxmax(this->payload, rhs.payload);
		size_t cdigits = fxmin(this->payload, rhs.payload);
		size_t ddigits = digits - cdigits;
		size_t lcapacity = this->capacity;
		uint8_t* lsrc = this->natural;
		
		if (this->capacity < digits) {
			this->capacity = digits;
			this->natural = this->malloc(this->capacity);
		}

		for (size_t idx = 1; idx <= cdigits; idx++) {
			this->natural[this->capacity - idx] = (lsrc[lcapacity - idx] ^ rhs.natural[rhs.capacity - idx]);
		}

		if (ddigits > 0U) {
			if (this->payload < rhs.payload) {
				memcpy(this->natural + (this->capacity - digits), rhs.natural + (rhs.capacity - rhs.payload), ddigits);
			} else if (lsrc != this->natural) {
				memcpy(this->natural + (this->capacity - digits), lsrc + (lcapacity - this->payload), ddigits);
			}

			this->payload = digits;
		} else {
			this->skip_leading_zeros(cdigits);
		}

		if (this->natural != lsrc) {
			delete[] lsrc;
		}
	}

	return (*this);
}

bool GYDM::Natural::is_bit_set(uint64_t m) {
	bool set = false;

	if (m <= (this->payload * 8U)) {
		uint64_t q = m / 8;
		uint64_t r = m % 8;
		uint8_t digit = this->natural[this->capacity - q - 1];

		set = ((digit & (1 << r)) > 0U);
	}

	return set;
}

Natural GYDM::Natural::bit_field(uint64_t start, uint64_t endp1) { // counting from right side
	size_t startq = size_t(start / 8);
	size_t endq = size_t(endp1) / 8;
	size_t endr = size_t(endp1 % 8);
	Natural sub(nullptr, 0LL);

	if (startq < this->payload) {
		size_t startr = (size_t)(start % 8);

		if (endq >= this->payload) {
			endq = this->payload;
			endr = 0U;
		}

		if (endq >= startq) {
			sub.payload = (endq - startq) + ((endr > 0) ? 1 : 0);

			if (sub.payload > sub.capacity) {
				sub.expand(sub.payload - sub.capacity);
			}

			memcpy(sub.natural + (sub.capacity - sub.payload), this->natural + (this->capacity - startq - sub.payload), sub.payload);
		}

		if (endr > 0U) {
			sub.natural[sub.capacity - sub.payload] &= ((1U << endr) - 1U);
		}

		sub >>= startr;
	}

	return sub;
}

uint64_t GYDM::Natural::bitfield(uint64_t start, uint64_t endp1) { // counting from right side
	uint64_t sub = 0x0U;
	
	endp1 = fxmin(start + 64ULL, fxmin(endp1, this->payload * 8ULL + 1ULL));

	if ((endp1 > 0) && (endp1 > start)) {
		size_t startq = size_t(start / 8);
		size_t startr = size_t(start % 8);
		size_t endq = size_t(endp1) / 8;
		size_t endr = size_t(endp1) % 8;
		size_t terminator = this->capacity - startq - 1;

		if (endq > startq) {
			size_t idx = this->capacity - endq - 1;

			if (endr > 0) {
				sub = this->natural[idx++] & ((1U << endr) - 1U);
			} else {
				idx++;
			}

			while (idx < terminator) {
				sub <<= 8;
				sub |= this->natural[idx++];
			};

			sub <<= (8 - startr);
			sub |= (this->natural[terminator] >> startr);
		} else if (endq == startq) {
			sub = (this->natural[terminator] & ((1U << endr) - 1U)) >> startr;
		}
	}

	return sub;
}

int64_t GYDM::Natural::signed_bitfield(uint64_t start, uint64_t endp1) { // counting from right side
	uint64_t mask_length = fxmin(endp1 - start, 64ULL) - 1;
	uint64_t raw = this->bitfield(start, endp1);
	int64_t sint = 0LL;

	if ((raw >> mask_length) > 0) {
		uint64_t mask = (1 << mask_length) - 1U;
		
		sint = _S64(raw | ((~0ULL) & (~mask)));
	} else {
		sint = _S64(raw);
	}

	return sint;
}

/*************************************************************************************************/
uint8_t& GYDM::Natural::operator[](int idx) {
	size_t bidx = 0U;

	if (this->payload == 0U) {
		// WARNING: this is an undefined behavior.
	} else if (idx >= 0) {
		if (_SZ(idx) < this->payload) {
			bidx = this->capacity - this->payload + idx;
		}
	} else {
		if (idx >= -int(this->payload)) {
			bidx = this->capacity + idx;
		}
	}

	return this->natural[bidx];
}

size_t GYDM::Natural::fixnum_count(Fixnum type) const {
	size_t modulus = 8U;

	switch (type) {
	case Fixnum::Uint64: modulus = 8U; break;
	case Fixnum::Uint32: modulus = 4U; break;
	case Fixnum::Uint16: modulus = 2U; break;
	}

	return fixnum_length(this->payload, modulus);
}

uint16_t GYDM::Natural::fixnum16_ref(int slot_idx, size_t offset) const {
	return fixnum_ref<uint16_t>(this->natural, this->payload, this->capacity, slot_idx, offset, 2U);
}

uint32_t GYDM::Natural::fixnum32_ref(int slot_idx, size_t offset) const {
	return fixnum_ref<uint32_t>(this->natural, this->payload, this->capacity, slot_idx, offset, 4U);
}

uint64_t GYDM::Natural::fixnum64_ref(int slot_idx, size_t offset) const {
	return fixnum_ref<uint64_t>(this->natural, this->payload, this->capacity, slot_idx, offset, 8U);
}

/*************************************************************************************************/
size_t GYDM::Natural::expand(size_t size) {
	if (size > 0) {
		if (this->natural != nullptr) {
			this->recalloc(this->capacity + size);
		} else {
			this->capacity += size;
			this->natural = this->malloc(this->capacity);
		}
	}

	return this->capacity;
}

/*************************************************************************************************/
GYDM::Natural::Natural(void* null, int64_t capacity) : natural(nullptr), capacity(0L), payload(0L) {
	this->capacity = ((capacity > 0) ? _SZ(capacity) : sizeof(uint64_t));
	this->natural = this->malloc(this->capacity);
}

void GYDM::Natural::replaced_by_fixnum(uint64_t n) {
	this->payload = 0U;

	while (n > 0U) {
		this->payload++;
		this->natural[this->capacity - this->payload] = _U8(n & 0xFFU);
		
		n >>= 8U;
	}
}

void GYDM::Natural::from_memory(const uint8_t nbytes[], size_t nstart, size_t nend) {
	if (nend > nstart) {
		this->capacity = nend - nstart;
		this->natural = this->malloc(this->capacity);

		for (size_t idx = 0; idx < this->capacity; idx++) {
			size_t sidx = idx + nstart;

			if (nbytes[sidx] == 0) {
				this->natural[idx] = 0;
			} else {
				this->payload = this->capacity - idx;
				memcpy(this->natural + idx, nbytes + sidx, nend - sidx);
				break;
			}
		}
	}
}

void GYDM::Natural::from_memory(const uint16_t nchars[], size_t nstart, size_t nend) {
	if (nend > nstart) {
		this->capacity = (nend - nstart) * 2;
		this->natural = this->malloc(this->capacity);
		size_t idx = 0U;

		for (size_t sidx = nstart; sidx < nend; sidx++) {
			uint16_t ch = _U16(nchars[sidx]);

			this->natural[idx++] = _U8(ch >> 8);
			this->natural[idx++] = _U8(ch & 0xFFU);

			if (this->payload == 0) {
				if (ch > 0xFFU) {
					this->payload = this->capacity - (idx - 2);
				} else if (ch > 0) {
					this->payload = this->capacity - (idx - 1);
				}
			}
		}
	}
}

void GYDM::Natural::from_base16(const uint8_t nbytes[], size_t nstart, size_t nend) {
	if (nend > nstart) {
		size_t span = nend - nstart;
		
		this->capacity = span / 2 + (span % 2);
		this->natural = this->malloc(this->capacity);
		this->payload = natural_from_base16(this->natural, nbytes, nstart, nend, this->capacity);
	}
}

void GYDM::Natural::from_base16(const uint16_t nchars[], size_t nstart, size_t nend) {
	if (nend > nstart) {
		size_t span = nend - nstart;
		
		this->capacity = span / 2 + (span % 2);
		this->natural = this->malloc(this->capacity);
		this->payload = natural_from_base16(this->natural, nchars, nstart, nend, this->capacity);
	}
}

void GYDM::Natural::from_base10(const uint8_t nbytes[], size_t nstart, size_t nend) {
	if (nend > nstart) {
		this->capacity = nend - nstart;
		this->natural = this->malloc(this->capacity);
		this->payload = natural_from_base(10U, this->natural, nbytes, int(nstart), int(nend), this->capacity);
	}
}

void GYDM::Natural::from_base10(const uint16_t nchars[], size_t nstart, size_t nend) {
	if (nend > nstart) {
		this->capacity = nend - nstart;
		this->natural = this->malloc(this->capacity);
		this->payload = natural_from_base(10U, this->natural, nchars, int(nstart), int(nend), this->capacity);
	}
}

void GYDM::Natural::from_base8(const uint8_t nbytes[], size_t nstart, size_t nend) {
	if (nend > nstart) {
		size_t span = nend - nstart;
		
		this->capacity = (span / 3 + ((span % 3 == 0) ? 0 : 1)) * 2;
		this->natural = this->malloc(this->capacity);
		this->payload = natural_from_base(8U, this->natural, nbytes, int(nstart), int(nend), this->capacity);
	}
}

void GYDM::Natural::from_base8(const uint16_t nchars[], size_t nstart, size_t nend) {
	if (nend > nstart) {
		size_t span = nend - nstart;
		
		this->capacity = (span / 3 + ((span % 3 == 0) ? 0 : 1)) * 2;
		this->natural = this->malloc(this->capacity);
		this->payload = natural_from_base(8U, this->natural, nchars, int(nstart), int(nend), this->capacity);
	}
}

/*************************************************************************************************/
void GYDM::Natural::add_digit(uint8_t digit) {
	size_t idx = this->capacity - 1U;
	
	if (this->payload == 0) {
		this->natural[idx] = digit;
		this->payload++;
	} else {
		while (digit > 0U) {
			uint16_t sum = this->natural[idx] + digit;

			if (sum <= 0xFF) {
				this->natural[idx] = _U8(sum);
				digit = 0U;
			} else {
				size_t payload_idx = this->capacity - this->payload;

				this->natural[idx] = _U8(sum & 0xFFU);
				digit = sum >> 8U;

				if (idx > payload_idx) {
					idx--;
					continue;
				}
				
				if (idx == 0U) {
					this->expand(1U);
					this->natural[0] = digit;
				} else if (idx == payload_idx) {
					this->natural[idx - 1] = digit;
				}

				this->payload++;
				digit = 0U;
			}
		}
	}
}

void GYDM::Natural::times_digit(uint8_t rhs) {
	if (rhs > 1ULL) {
		uint16_t carry = 0U;

		for (size_t idx = 1U; idx <= this->payload; idx++) {
			uint16_t digit = this->natural[this->capacity - idx] * rhs + carry;

			if (digit <= 0xFFU) {
				this->natural[this->capacity - idx] = _U8(digit);
				carry = 0U;
			} else {
				this->natural[this->capacity - idx] = _U8(digit & 0xFFU);
				carry = digit >> 8U;
			}
		}

		if (carry > 0U) {
			if (this->capacity == this->payload) {
				this->expand(1U);
			} 
			
			this->payload++;
			this->natural[this->capacity - this->payload] = _U8(carry);
		}
	} else if (rhs == 0U) {
		this->bzero();
	}
}

void GYDM::Natural::divide_digit(uint8_t divisor, Natural* oremainder) {
	if (divisor > 1ULL) {
		uint16_t remainder = 0U;

		for (size_t idx = this->capacity - this->payload; idx < this->capacity; idx++) {
			uint16_t dividend = (remainder << 8U) ^ this->natural[idx];

			if (dividend < divisor) {
				this->natural[idx] = 0U;
				remainder = dividend;
			} else {
				this->natural[idx] = _U8(dividend / divisor);
				remainder = dividend % divisor;
			}
		}

		this->skip_leading_zeros(this->payload);

		if (oremainder != nullptr) {
			(*oremainder) = remainder;
		}
	} else if (divisor == 1ULL) {
		if (oremainder != nullptr) {
			oremainder->bzero();
		}
	}
}

int GYDM::Natural::compare_to_one() const {
	int cmp = int(this->payload) - 1;

	if (cmp == 0) {
		cmp = this->natural[this->capacity - 1U] - 1;
	}

	return cmp;
}

/*************************************************************************************************/
void GYDM::Natural::bzero() {
	this->payload = 0U;

	// Method should not assume zeroed memory.

#ifndef NDEBUG
	if (this->natural != nullptr) {
		memset(this->natural, '\0', this->capacity);
	}
#endif
}

void GYDM::Natural::skip_leading_zeros(size_t new_payload) {
	uint8_t* cursor = this->natural + (this->capacity - new_payload);

	// WARNING: Invokers take responsibilities to ensure that `payload` is not out of index.

	this->payload = new_payload;
	while ((this->payload > 0U) && ((*cursor++) == 0U)) {
		this->payload--;
	}
}

void GYDM::Natural::decrease_from_slot(size_t slot) {
	while (slot <= this->payload) {
		uint8_t digit = this->natural[this->capacity - slot];

		if (digit > 0U) {
			this->natural[this->capacity - slot] = digit - 1U;

			if ((digit == 1) && (slot == this->payload)) {
				this->payload--;
			}

			break;
		} else {
			if (slot < this->payload) {
				this->natural[this->capacity - slot] = 0xFFU;
				slot++;
			} else {
				this->bzero();
				break;
			}
		}
	}
}

uint8_t* GYDM::Natural::malloc(size_t size) {
	uint8_t* memory = new uint8_t[size];

	// NOTE: Method should not assume zeroed memory.

#ifndef NDEBUG
	memset(memory, _S32(size), size);
#endif

	return memory;
}

void GYDM::Natural::recalloc(size_t newsize, size_t shift) {
	uint8_t* src = this->natural;
	size_t zero_size = (this->capacity - this->payload);

	this->capacity = newsize;
	this->natural = this->malloc(this->capacity);

	{ // do copying and shifting
		size_t payload_idx0 = this->capacity - this->payload /* this is not the `zero_size` */ - shift;

		memset(this->natural, '\0', this->capacity);
		memcpy(this->natural + payload_idx0, src + zero_size, this->payload);
	}

	delete[] src;
}

void GYDM::Natural::smart_prealloc(size_t size) {
	if (size + this->payload > this->capacity) {
		this->expand(size + this->payload - this->capacity);
	}
}
