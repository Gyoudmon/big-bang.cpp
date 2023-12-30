#pragma once

#include "bytes.hpp"

#include <cstdint>

namespace GYDM {
	enum class Fixnum { Uint16, Uint32, Uint64 };

	class __lambda__ Natural {
	public:
		~Natural() noexcept;
		
		Natural();
		Natural(uint64_t n);

		Natural(const GYDM::bytes& nstr, size_t nstart = 0, size_t nend = 0);
		Natural(const std::string& nstr, size_t nstart = 0, size_t nend = 0);
		Natural(const std::wstring& nstr, size_t nstart = 0, size_t nend = 0);
		Natural(uintptr_t base, const GYDM::bytes& nstr, size_t nstart = 0, size_t nend = 0);
		Natural(uintptr_t base, const std::string& nstr, size_t nstart = 0, size_t nend = 0);
		Natural(uintptr_t base, const std::wstring& nstr, size_t nstart = 0, size_t nend = 0);
		
		template<typename BYTE, size_t N>
		Natural(const BYTE(&ns)[N], size_t nstart = 0, size_t nend = N)
			: Natural(reinterpret_cast<const BYTE*>(ns), nstart, nend) {}

		template<typename BYTE, size_t N>
		Natural(uintptr_t base, const BYTE(&ns)[N], size_t nstart = 0, size_t nend = N)
			: Natural(base, reinterpret_cast<const BYTE*>(ns), nstart, nend) {}

		template<typename BYTE>
		Natural(const BYTE ns[], size_t nstart, size_t nend)
			: Natural(0U, ns, nstart, nend) {}

		template<typename BYTE>
		Natural(uintptr_t base, const BYTE ns[], size_t nstart, size_t nend) : natural(nullptr), capacity(0U), payload(0U) {
			switch (base) {
			case 16: this->from_base16(ns, nstart, nend); break;
			case 10: this->from_base10(ns, nstart, nend); break;
			case 8:  this->from_base8(ns, nstart, nend); break;
			default: this->from_memory(ns, nstart, nend);
			}
		}

	public:
		Natural(const GYDM::Natural& n); // copy constructor
		Natural(GYDM::Natural&& n);      // move constructor

		GYDM::Natural& operator=(uint64_t n);
		GYDM::Natural& operator=(const GYDM::Natural& n);     // copy assignment operator
		GYDM::Natural& operator=(GYDM::Natural&& n) noexcept; // move assignment operator

	public: // NOTE: C++20 has operator<=>
		int compare(uint64_t rhs) const;
		int compare(const GYDM::Natural& rhs) const;

		friend inline bool operator<(const GYDM::Natural& lhs, const GYDM::Natural& rhs) { return (lhs.compare(rhs) < 0); }
		friend inline bool operator<(const GYDM::Natural& lhs, uint64_t rhs) { return (lhs.compare(rhs) < 0); }
		friend inline bool operator<(uint64_t lhs, const GYDM::Natural& rhs) { return (rhs.compare(lhs) > 0); }
		friend inline bool operator==(const GYDM::Natural& lhs, const GYDM::Natural& rhs) { return (lhs.compare(rhs) == 0); }
		friend inline bool operator==(const GYDM::Natural& lhs, uint64_t rhs) { return (lhs.compare(rhs) == 0); }
		friend inline bool operator==(uint64_t lhs, const GYDM::Natural& rhs) { return (rhs.compare(lhs) == 0); }

		friend inline bool operator>(const GYDM::Natural& lhs, const GYDM::Natural& rhs) { return (lhs.compare(rhs) > 0); }
		friend inline bool operator>(const GYDM::Natural& lhs, uint64_t rhs) { return (lhs.compare(rhs) > 0); }
		friend inline bool operator>(uint64_t lhs, const GYDM::Natural& rhs) { return (rhs.compare(lhs) < 0); }
		friend inline bool operator<=(const GYDM::Natural& lhs, const GYDM::Natural& rhs) { return (lhs.compare(rhs) <= 0); }
		friend inline bool operator<=(const GYDM::Natural& lhs, uint64_t rhs) { return (lhs.compare(rhs) <= 0); }
		friend inline bool operator<=(uint64_t lhs, const GYDM::Natural& rhs) { return (rhs.compare(lhs) >= 0); }
		friend inline bool operator>=(const GYDM::Natural& lhs, const GYDM::Natural& rhs) { return (lhs.compare(rhs) >= 0); }
		friend inline bool operator>=(const GYDM::Natural& lhs, uint64_t rhs) { return (lhs.compare(rhs) >= 0); }
		friend inline bool operator>=(uint64_t lhs, const GYDM::Natural& rhs) { return (rhs.compare(lhs) <= 0); }
		friend inline bool operator!=(const GYDM::Natural& lhs, const GYDM::Natural& rhs) { return (lhs.compare(rhs) != 0); }
		friend inline bool operator!=(const GYDM::Natural& lhs, uint64_t rhs) { return (lhs.compare(rhs) != 0); }
		friend inline bool operator!=(uint64_t lhs, const GYDM::Natural& rhs) { return (rhs.compare(lhs) != 0); }

	public:
		GYDM::Natural& operator++();
		inline GYDM::Natural operator++(int postfix) { Natural snapshot(*this); this->operator++(); return snapshot; }

		GYDM::Natural& operator--();
		inline GYDM::Natural operator--(int postfix) { Natural snapshot(*this); this->operator--(); return snapshot; }

		GYDM::Natural& operator+=(uint64_t rhs);
		GYDM::Natural& operator+=(const GYDM::Natural& rhs);

		GYDM::Natural& operator-=(uint64_t rhs);
		GYDM::Natural& operator-=(const GYDM::Natural& rhs);
		
		GYDM::Natural& operator*=(uint64_t rhs);
		GYDM::Natural& operator*=(const GYDM::Natural& rhs);

		inline GYDM::Natural& operator/=(uint64_t rhs) { return this->quotient_remainder(rhs, nullptr); }
		inline GYDM::Natural& operator/=(const GYDM::Natural& rhs) { return this->quotient_remainder(rhs, nullptr); }
		inline GYDM::Natural& operator%=(uint64_t rhs) { return this->quotient_remainder(rhs, this); };
		inline GYDM::Natural& operator%=(const GYDM::Natural& rhs) { return this->quotient_remainder(rhs, this); };

		friend inline GYDM::Natural operator+(GYDM::Natural lhs, uint64_t rhs) { return lhs += rhs; }
		friend inline GYDM::Natural operator+(uint64_t lhs, GYDM::Natural rhs) { return rhs += lhs; }
		friend inline GYDM::Natural operator+(GYDM::Natural lhs, const GYDM::Natural& rhs) { return lhs += rhs; }

		// NOTE: the compiler will cast the number into Natural when encountered `n - Natural`;
		friend inline GYDM::Natural operator-(GYDM::Natural lhs, uint64_t rhs) { return lhs -= rhs; }
		friend inline GYDM::Natural operator-(GYDM::Natural lhs, const GYDM::Natural& rhs) { return lhs -= rhs; }

		friend inline GYDM::Natural operator*(GYDM::Natural lhs, uint64_t rhs) { return lhs *= rhs; }
		friend inline GYDM::Natural operator*(uint64_t lhs, GYDM::Natural rhs) { return rhs *= lhs; }
		friend inline GYDM::Natural operator*(GYDM::Natural lhs, const GYDM::Natural& rhs) { return lhs *= rhs; }

		// NOTE: the compiler will cast the number into Natural when encountered `n / Natural` or `n % Natural`
		friend inline GYDM::Natural operator/(GYDM::Natural lhs, uint64_t rhs) { return lhs /= rhs; }
		friend inline GYDM::Natural operator/(GYDM::Natural lhs, const GYDM::Natural& rhs) { return lhs /= rhs; }
		friend inline GYDM::Natural operator%(GYDM::Natural lhs, uint64_t rhs) { return lhs %= rhs; }
		friend inline GYDM::Natural operator%(GYDM::Natural lhs, const GYDM::Natural& rhs) { return lhs %= rhs; }

	public:
		GYDM::Natural& expt(uint64_t e);
		GYDM::Natural& expt(const GYDM::Natural& e);

		GYDM::Natural& modular_expt(uint64_t b, uint64_t n);
		GYDM::Natural& modular_expt(uint64_t b, const GYDM::Natural& n);
		GYDM::Natural& modular_expt(const GYDM::Natural& b, uint64_t n);
		GYDM::Natural& modular_expt(const GYDM::Natural& b, const GYDM::Natural& n);

		GYDM::Natural& quotient_remainder(uint64_t divisor, Natural* remainder = nullptr);
		GYDM::Natural& quotient_remainder(const GYDM::Natural& divisor, Natural* remainder = nullptr);

		friend inline GYDM::Natural expt(GYDM::Natural b, uint64_t e) { return b.expt(e); }
		friend inline GYDM::Natural expt(uint64_t b, GYDM::Natural e) { return Natural(b).expt(e); }
		friend inline GYDM::Natural expt(GYDM::Natural b, const GYDM::Natural& e) { return b.expt(e); }

		friend inline GYDM::Natural modular_expt(GYDM::Natural a, uint64_t b, uint64_t n) { return a.modular_expt(b, n); }
		friend inline GYDM::Natural modular_expt(GYDM::Natural a, uint64_t b, const GYDM::Natural& n) { return a.modular_expt(b, n); }
		friend inline GYDM::Natural modular_expt(GYDM::Natural a, const GYDM::Natural& b, uint64_t n) { return a.modular_expt(b, n); }
		friend inline GYDM::Natural modular_expt(GYDM::Natural a, const GYDM::Natural& b, const GYDM::Natural& n) { return a.modular_expt(b, n); }

	public:
		GYDM::Natural operator~();

		GYDM::Natural& operator<<=(uint64_t rhs);
		GYDM::Natural& operator>>=(uint64_t rhs);

		GYDM::Natural& operator&=(uint64_t rhs);
		GYDM::Natural& operator&=(const GYDM::Natural& rhs);
		GYDM::Natural& operator|=(uint64_t rhs);
		GYDM::Natural& operator|=(const GYDM::Natural& rhs);
		GYDM::Natural& operator^=(uint64_t rhs);
		GYDM::Natural& operator^=(const GYDM::Natural& rhs);

		friend inline GYDM::Natural operator<<(GYDM::Natural lhs, uint64_t rhs) { return lhs <<= rhs; }
		friend inline GYDM::Natural operator>>(GYDM::Natural lhs, uint64_t rhs) { return lhs >>= rhs; }

		friend inline GYDM::Natural operator&(GYDM::Natural lhs, uint64_t rhs) { return lhs &= rhs; }
		friend inline GYDM::Natural operator&(uint64_t lhs, GYDM::Natural rhs) { return rhs &= lhs; }
		friend inline GYDM::Natural operator&(GYDM::Natural lhs, const GYDM::Natural& rhs) { return lhs &= rhs; }
		friend inline GYDM::Natural operator|(GYDM::Natural lhs, uint64_t rhs) { return lhs |= rhs; }
		friend inline GYDM::Natural operator|(uint64_t lhs, GYDM::Natural rhs) { return rhs |= lhs; }
		friend inline GYDM::Natural operator|(GYDM::Natural lhs, const GYDM::Natural& rhs) { return lhs |= rhs; }
		friend inline GYDM::Natural operator^(GYDM::Natural lhs, uint64_t rhs) { return lhs ^= rhs; }
		friend inline GYDM::Natural operator^(uint64_t lhs, GYDM::Natural rhs) { return rhs ^= lhs; }
		friend inline GYDM::Natural operator^(GYDM::Natural lhs, const GYDM::Natural& rhs) { return lhs ^= rhs; }

		bool is_bit_set(uint64_t m);
		GYDM::Natural bit_field(uint64_t start, uint64_t endp1);
		uint64_t bitfield(uint64_t start, uint64_t endp1);
		int64_t signed_bitfield(uint64_t start, uint64_t endp1);

	public:
		uint8_t& operator[] (int idx);
		uint16_t fixnum16_ref(int idx, size_t offset = 0U) const;
		uint32_t fixnum32_ref(int idx, size_t offset = 0U) const;
		uint64_t fixnum64_ref(int idx, size_t offset = 0U) const;

	public:
		bool is_zero() const;
		bool is_one() const;
		bool is_fixnum() const;
		bool is_odd() const;
		bool is_even() const;

	public:
		size_t length() const;
		size_t integer_length(uint8_t alignment = 0U) const;
		size_t fixnum_count(GYDM::Fixnum type = Fixnum::Uint64) const;

	public:
		size_t expand(size_t size);

	public:
		GYDM::bytes to_bytes() const;
		GYDM::bytes to_hexstring(char ten = 'A') const;
		GYDM::bytes to_binstring(uint8_t alignment = 0U) const;

	private:
		Natural(void* null, int64_t capacity);
		void replaced_by_fixnum(uint64_t n);
		void from_memory(const uint8_t nbytes[], size_t nstart, size_t nend);
		void from_memory(const uint16_t nchars[], size_t nstart, size_t nend);
		void from_base16(const uint8_t nbytes[], size_t nstart, size_t nend);
		void from_base16(const uint16_t nchars[], size_t nstart, size_t nend);
		void from_base10(const uint8_t nbytes[], size_t nstart, size_t nend);
		void from_base10(const uint16_t nchars[], size_t nstart, size_t nend);
		void from_base8(const uint8_t nbytes[], size_t nstart, size_t nend);
		void from_base8(const uint16_t nchars[], size_t nstart, size_t nend);

	private:
		void add_digit(uint8_t digit);
		void times_digit(uint8_t digit);
		void divide_digit(uint8_t digit, GYDM::Natural* remainder);
		int compare_to_one() const;

	private:
		void on_moved();
		void bzero();
		void skip_leading_zeros(size_t new_payload);
		void decrease_from_slot(size_t slot);
		uint8_t* malloc(size_t size);
		void recalloc(size_t new_size, size_t shift = 0U);
		void smart_prealloc(size_t size);
		
	private:
		uint8_t* natural;
		size_t capacity;
		size_t payload;
	};
}
