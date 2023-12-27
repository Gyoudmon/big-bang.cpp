#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>
#include <cmath>

#include "../../forward.hpp"

namespace GYDM {
    /** NOTE: the HSB and HSV are identical **/
    class __lambda__ RGBA {
    public:
        static GYDM::RGBA HSV(double hue, double saturation = 1.0, double brightness = 1.0, double alpha = 1.0);
        static GYDM::RGBA HSL(double hue, double saturation = 1.0, double lightness = 1.0, double alpha = 1.0);
        static GYDM::RGBA HSI(double hue, double saturation = 1.0, double intensity = 1.0, double alpha = 1.0);

    public:
        RGBA() : r(0.0), g(0.0), b(0.0), a(0.0) { /* transparent color */ }

        RGBA(uint32_t hex, int alpha);
        RGBA(uint8_t red, uint8_t green, uint8_t blue, int alpha);
        RGBA(double red, double green, double blue, int alpha, bool allow_negative = false);
        
        RGBA(uint32_t hex, double alpha = 1.0);
        RGBA(uint8_t red, uint8_t green, uint8_t blue, double alpha = 1.0);
        RGBA(double red, double green, double blue, double alpha = 1.0, bool allow_negative = false);

        RGBA(const RGBA& c);
        RGBA(const RGBA& c, int alpha);
        RGBA(const RGBA& c, double alpha);

        ~RGBA() noexcept {}

    public:
        GYDM::RGBA& operator=(const RGBA& c); // copy assignment
        GYDM::RGBA& operator=(uint32_t hex);  // copy assignment
        double operator[](size_t i) const;
        
		friend inline GYDM::RGBA operator+(RGBA lhs, const RGBA& rhs) { return lhs += rhs; }
        friend inline GYDM::RGBA operator+(RGBA lhs, uint32_t rhs) { return lhs += rhs; }
		friend inline GYDM::RGBA operator+(uint32_t lhs, RGBA rhs) { return rhs += lhs; }

		friend inline GYDM::RGBA operator*(RGBA lhs, const RGBA& rhs) { return lhs *= rhs; }
        friend inline GYDM::RGBA operator*(RGBA lhs, uint32_t rhs) { return lhs *= rhs; }
		friend inline GYDM::RGBA operator*(uint32_t lhs, RGBA rhs) { return rhs *= lhs; }

        // C++20 has the `operator<=>`
        bool equal(const RGBA& rhs) const;
        bool equal(uint32_t rhs, double alpha = 1.0) const;
        friend inline bool operator==(const RGBA& lhs, const RGBA& rhs) { return lhs.equal(rhs); }
        friend inline bool operator==(const RGBA& lhs, uint32_t rhs) { return lhs.equal(rhs); }
        friend inline bool operator==(uint32_t lhs, const RGBA& rhs) { return rhs.equal(lhs); }
        friend inline bool operator!=(const RGBA& lhs, const RGBA& rhs) { return !lhs.equal(rhs); }
        friend inline bool operator!=(const RGBA& lhs, uint32_t rhs) { return !lhs.equal(rhs); }
        friend inline bool operator!=(uint32_t lhs, const RGBA& rhs) { return !rhs.equal(lhs); }

    public:
        GYDM::RGBA contrast() const;
        GYDM::RGBA contrast_for_background() const;

    public:
        void unbox(uint8_t* r = nullptr, uint8_t* g = nullptr, uint8_t* b = nullptr, uint8_t* a = nullptr) const;
        void unbox(double* r, double* g = nullptr, double* b = nullptr, double* a = nullptr) const;
        uint8_t R() const { return static_cast<uint8_t>(round(this->r * 255.0)); }
        uint8_t G() const { return static_cast<uint8_t>(round(this->g * 255.0)); }
        uint8_t B() const { return static_cast<uint8_t>(round(this->b * 255.0)); }
        uint8_t A() const { return static_cast<uint8_t>(round(this->a * 255.0)); }
        double red() const { return this->r; }
        double green() const { return this->g; }
        double blue() const { return this->b; }
        double alpha() const { return this->a; }

    public:
        uint32_t rgb() const;
        uint32_t rgba() const;
        double hue() const;
        double saturation() const;
        double brightness() const;

    public:
        bool is_transparent() const { return (this->a < 1.0e-6); }
        bool is_opacity() const { return !this->is_transparent(); }
        bool is_grayscale() const { return (this->r == this->g) && (this->g == this->b); }
        bool is_grayscale(double gray) const { return this->is_grayscale() && (this->r == gray); }
        bool is_grayscale(uint8_t gray) const { return this->is_grayscale() && (this->R() == gray); }
        bool is_grayscale(int gray) const { return this->is_grayscale(uint8_t(gray)); }
        bool is_black() const { return this->is_grayscale(0.0); }
        bool is_white() const { return this->is_grayscale(1.0); }

    public:
        std::string hexstring(bool needs_alpha = true, const char* fmt = nullptr) const;
        std::string hexstring(const char* fmt, bool needs_alpha = true) const { return this->hexstring(needs_alpha, fmt); }

    private:
        GYDM::RGBA& operator+=(const RGBA& rhs);
		GYDM::RGBA& operator+=(uint32_t rhs);

        GYDM::RGBA& operator*=(const RGBA& rhs);
		GYDM::RGBA& operator*=(uint32_t rhs);

    private:
        double r;
        double g;
        double b;
        double a;
    };

    /*********************************************************************************************/
    const GYDM::RGBA transparent;
}
