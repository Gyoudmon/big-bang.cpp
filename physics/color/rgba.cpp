#include "rgba.hpp"

#include "../../datum/box.hpp"
#include "../../datum/string.hpp"
#include "../../datum/fixnum.hpp"
#include "../../datum/flonum.hpp"

#include "../mathematics.hpp"

using namespace GYDM;

/*************************************************************************************************/
#define UCHAR(v) static_cast<uint8_t>(flround(v * 255.0))
#define GAMUT(v) (double(v) / 255.0)

static inline void rgb_add(double dstR, double dstG, double dstB, double srcR, double srcG, double srcB, double srcA, double& r, double& g, double& b) {
    double R = dstR + srcR * srcA;
    double G = dstG + srcG * srcA;
    double B = dstB + srcB * srcA;

    r = R;
    g = G;
    b = B;
}

static inline void rgb_mul(double dstR, double dstG, double dstB, double dstA, double srcR, double srcG, double srcB, double srcA, double& r, double& g, double& b, double& a) {
    double R = srcR * dstR + dstR * (1.0 - srcA);
    double G = srcG * dstG + dstG * (1.0 - srcA);
    double B = srcB * dstB + dstB * (1.0 - srcA);

    r = R;
    g = G;
    b = B;
    a = srcA * dstA + dstA * (1.0 - srcA);
}

static inline void hexadecimal_to_rgb(uint32_t hex, double* r, double* g, double* b) {
    SET_BOX(r, GAMUT((hex >> 16) & 0xFF));
    SET_BOX(g, GAMUT((hex >> 8) & 0xFF));
    SET_BOX(b, GAMUT(hex & 0xFF));
}

static inline uint32_t rgb_to_hexadecimal(double red, double green, double blue) {
    uint32_t r = UCHAR(red);
    uint32_t g = UCHAR(green);
    uint32_t b = UCHAR(blue);

    return (r << 16U) | (g << 8U) | b;
}

static void feed_rgb_from_hue(double hue, double chroma, double m, double* red, double* green, double* blue) {
    double r = m;
    double g = m;
    double b = m;

    hue = degrees_normalize(hue);
    
    if (!flisnan(hue)) {
        double hue_60 = hue / 60.0f;
        double flhue = flfloor(hue_60);
        int fxhue = int(flhue);
        double x = chroma * (1.0f - flabs(double(fxhue % 2) - (flhue - hue_60) - 1.0f));
        
        switch (fxhue) {
        case 0: r += chroma; g += x; break;
        case 1: r += x; g += chroma; break;
        case 2: g += chroma; b += x; break;
        case 3: g += x; b += chroma; break;
        case 4: r += x; b += chroma; break;
        case 5: r += chroma; b += x; break;
        }
    }

    SET_TRIPLETS(red, r, green, g, blue, b);
}

static void feed_rgb_from_hsi_sector(double hue, double saturation, double intensity, char com, double* r, double* g, double* b) {
    double cosH_60H = 2.0f; // if hue == 0.0 or hue == 120.0;

    if ((hue != 0.0f) && (hue != 120.0f)) {
        double H = hue * (pi_f / 180.0f);
        cosH_60H = flcos(H) / flcos(pi_f / 3.0f - H);
    }

    {
        double major = intensity * (1.0f + saturation * cosH_60H);
        double midor = intensity * (1.0f - saturation);
        double minor = (intensity * 3.0f) - (major + midor);

        switch (com) {
        case 'r': SET_TRIPLETS(r, major, g, minor, b, midor); break;
        case 'g': SET_TRIPLETS(r, midor, g, major, b, minor); break;
        default:  SET_TRIPLETS(r, minor, g, midor, b, major); break;
        }
    }
}

static double rgb_to_hue(double red, double green, double blue, double* flM, double* flm, double* flchroma) {
    double M = flmax(red, green, blue);
    double m = flmin(red, green, blue);
    double chroma = M - m;
    
    SET_TRIPLETS(flchroma, chroma, flM, M, flm, m);
    
    if (chroma == 0.0F) {
        return flnan_f;
    } else if (M == green) {
        return 60.0 * ((blue - red) / chroma + 2.0F);
    } else if (M == blue) {
        return 60.0 * ((red - green) / chroma + 4.0F);
    } else if (green < blue) {
        return 60.0 * ((green - blue) / chroma + 6.0F);
    } else {
        return 60.0 * ((green - blue) / chroma);
    }
}

static inline void hsv_to_rgb(double hue, double saturation, double brightness, double* r, double* g, double* b) {
    double chroma = saturation * brightness;
    double m = brightness - chroma;
    
    feed_rgb_from_hue(hue, chroma, m, r, g, b);
}

static inline void rgb_to_hsv(double red, double green, double blue, double* h, double* s, double* b) {
    double M, m, chroma;
    double hue = rgb_to_hue(red, green, blue, &M, &m, &chroma);

    SET_BOX(h, hue);
    SET_BOX(s, (M == 0.0) ? 0.0 : chroma / M);
    SET_BOX(b, M);
}

static inline void hsl_to_rgb(double hue, double saturation, double lightness, double* r, double* g, double* b) {
    double chroma = saturation * (1.0f - flabs(lightness * 2.0f - 1.0f));
    double m = lightness - chroma * 0.5f;
    
    feed_rgb_from_hue(hue, chroma, m, r, g, b);
}

/*
static void rgb_to_hsl(double red, double green, double blue, double* h, double* s, double* l) {
    double M, m, chroma;
    double hue = rgb_to_hue(red, green, blue, &M, &m, &chroma);
    double lightness = (M + m) * 0.5;
    double abs_2L_1 = flabs(2.0 * lightness - 1.0);

    SET_BOX(h, hue);
    SET_BOX(s, (abs_2L_1 == 1.0) ? 0.0 : chroma / (1.0 - abs_2L_1));
    SET_BOX(l, lightness);
}
*/

static inline void hsi_to_rgb(double hue, double saturation, double intensity, double* r, double* g, double* b) {
    if ((saturation == 0.0f) || flisnan(hue)) {
        SET_TRIPLETS(r, intensity, g, intensity, b, intensity);
    } else if (hue < 120.0f) {
        feed_rgb_from_hsi_sector(hue, saturation, intensity, 'r', r, g, b);
    } else if (hue < 240.0f) {
        feed_rgb_from_hsi_sector(hue - 120.0f, saturation, intensity, 'g', r, g, b);
    } else {
        feed_rgb_from_hsi_sector(hue - 240.0f, saturation, intensity, 'b', r, g, b);
    }
}

/*************************************************************************************************/
RGBA GYDM::RGBA::HSV(double hue, double saturation, double brightness, double alpha) {
    RGBA c;

    hsv_to_rgb(hue, saturation, brightness, &c.r, &c.g, &c.b);
    c.a = clamp(alpha, 0.0, 1.0);

    return c;
}

RGBA GYDM::RGBA::HSL(double hue, double saturation, double lightness, double alpha) {
    RGBA c;

    c.a = clamp(alpha, 0.0, 1.0);
    hsl_to_rgb(hue, saturation, lightness, &c.r, &c.g, &c.b);

    return c;
}

RGBA GYDM::RGBA::HSI(double hue, double saturation, double intensity, double alpha) {
    RGBA c;

    c.a = clamp(alpha, 0.0, 1.0);
    hsi_to_rgb(hue, saturation, intensity, &c.r, &c.g, &c.b);

    return c;
}

/*************************************************************************************************/
GYDM::RGBA::RGBA(uint32_t hex, double alpha) {
    this->a = clamp(alpha, 0.0, 1.0);
    hexadecimal_to_rgb(hex, &this->r, &this->g, &this->b);
}

GYDM::RGBA::RGBA(uint8_t r, uint8_t g, uint8_t b, double alpha) {
    this->a = clamp(alpha, 0.0, 1.0);
    this->r = GAMUT(r);
    this->g = GAMUT(g);
    this->b = GAMUT(b);
}

GYDM::RGBA::RGBA(double r, double g, double b, double alpha, bool allow_negative) {
    this->a = clamp(alpha, 0.0, 1.0);

    if (allow_negative) {
        this->r = r;
        this->g = g;
        this->b = b;
    } else {
        this->r = clamp(r, 0.0, 1.0);
        this->g = clamp(g, 0.0, 1.0);
        this->b = clamp(b, 0.0, 1.0);
    }
}

GYDM::RGBA::RGBA(const RGBA& c, double alpha) {
    this->a = clamp(alpha, 0.0, 1.0);
    this->r = c.r;
    this->g = c.g;
    this->b = c.b;
}

GYDM::RGBA::RGBA(const RGBA& c) {
    this->a = c.a;
    this->r = c.r;
    this->g = c.g;
    this->b = c.b;
}

GYDM::RGBA::RGBA(uint32_t hex, int alpha) {
    this->a = clamp(GAMUT(alpha), 0.0, 1.0);
    hexadecimal_to_rgb(hex, &this->r, &this->g, &this->b);
}

GYDM::RGBA::RGBA(uint8_t r, uint8_t g, uint8_t b, int alpha)
    : RGBA(r, g, b, GAMUT(alpha)) {}

GYDM::RGBA::RGBA(double r, double g, double b, int alpha,  bool allow_negative)
    : RGBA(r, g, b, GAMUT(alpha), allow_negative) {}

GYDM::RGBA::RGBA(const RGBA& c, int alpha)
    : RGBA(c, GAMUT(alpha)) {}

void GYDM::RGBA::unbox(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) const {
    SET_TRIPLETS(r, this->R(), g, this->G(), b, this->B());
    SET_BOX(a, this->A());
}

void GYDM::RGBA::unbox(double* r, double* g, double* b, double* a) const {
    SET_TRIPLETS(r, this->r, g, this->g, b, this->b);
    SET_BOX(a, this->a);
}

/*************************************************************************************************/
RGBA& GYDM::RGBA::operator=(const RGBA& c) {
    this->a = c.a;
    this->r = c.r;
    this->g = c.g;
    this->b = c.b;

    return (*this);
}

RGBA& GYDM::RGBA::operator=(uint32_t hex) {
    this->a = 1.0;
    hexadecimal_to_rgb(hex, &this->r, &this->g, &this->b);

    return (*this);
}

GYDM::RGBA& GYDM::RGBA::operator+=(const RGBA& rhs) {
    rgb_add(this->r, this->g, this->b,
                rhs.r, rhs.g, rhs.b, rhs.a,
                this->r, this->g, this->b);

    return (*this);
}

GYDM::RGBA& GYDM::RGBA::operator+=(uint32_t rhs) {
    double rr, rg, rb;

    hexadecimal_to_rgb(rhs, &rr, &rg, &rb);
    rgb_add(this->r, this->g, this->b,
                rr, rg, rb, 1.0,
                this->r, this->g, this->b);

    return (*this);
}

GYDM::RGBA& GYDM::RGBA::operator*=(const RGBA& rhs) {
    rgb_mul(this->r, this->g, this->b, this->a,
                rhs.r, rhs.g, rhs.b, rhs.a,
                this->r, this->g, this->b, this->a);

    return (*this);
}

GYDM::RGBA& GYDM::RGBA::operator*=(uint32_t rhs) {
    double rr, rg, rb;

    hexadecimal_to_rgb(rhs, &rr, &rg, &rb);
    rgb_mul(this->r, this->g, this->b, this->a,
                rr, rg, rb, 1.0,
                this->r, this->g, this->b, this->a);

    return (*this);
}

double GYDM::RGBA::operator[](size_t i) const {
    switch (i) {
    case 0: return this->r; break;
    case 1: return this->g; break;
    case 2: return this->b; break;
    case 3: return this->a; break;
    default: return flnan;
    }
}

bool GYDM::RGBA::equal(const RGBA& rhs) const {
    return (this->r == rhs.r)
            && (this->g == rhs.g)
            && (this->b == rhs.b)
            && (this->a == rhs.a);
}

bool GYDM::RGBA::equal(uint32_t rhs, double alpha) const {
    double r, g, b;

    hexadecimal_to_rgb(rhs, &r, &g, &b);

    return (this->r == r)
            && (this->g == g)
            && (this->b == b)
            && (this->a == alpha);
}

/*************************************************************************************************/
uint32_t GYDM::RGBA::rgb() const {
    return rgb_to_hexadecimal(this->r, this->g, this->b);
}

uint32_t GYDM::RGBA::rgba() const {
    return (this->rgb() << 8U) | UCHAR(this->a);
}

double GYDM::RGBA::hue() const {
    return rgb_to_hue(this->r, this->g, this->b, nullptr, nullptr, nullptr);
}

double GYDM::RGBA::saturation() const {
    double s;

    rgb_to_hsv(this->r, this->g, this->b, nullptr, &s, nullptr);

    return s;
}

double GYDM::RGBA::brightness() const {
    double v;

    rgb_to_hsv(this->r, this->g, this->b, nullptr, nullptr, &v);

    return v;
}

/*************************************************************************************************/
RGBA GYDM::RGBA::contrast() const {
    double h, s, v;
    
    rgb_to_hsv(this->r, this->g, this->b, &h, &s, &v);

    if (flisnan(h)) {
        return this->contrast_for_background();
    } else {
        return RGBA::HSV(h + 180.0, s, v, this->a);
    }
}

RGBA GYDM::RGBA::contrast_for_background() const {
    double perceptive_luminance;

    perceptive_luminance = 1.0 - (double(r) * 0.299 + double(g) * 0.587 + double(b) * 0.114);

	if (perceptive_luminance < 0.5) {
		return 0x000000U;
	} else {
		return 0xFFFFFFU;
	}
}

/*************************************************************************************************/
std::string GYDM::RGBA::hexstring(bool needs_alpha, const char* fmt) const {
    std::string representation;

    if (needs_alpha) {
        representation = hexnumber(this->rgba(), 4UL);
    } else {
        representation = hexnumber(this->rgb(), 3UL);
    }

    if (fmt != nullptr) {
        representation = make_nstring(fmt, representation.c_str());
    }

    return representation;
}
