#include <SDL2/SDL.h>   /* Simple DirectMedia Layer 头文件, 放前面以兼容 macOS */
#include <cmath>

#include "colorspace.hpp"

#include "../datum/flonum.hpp"
#include "../datum/fixnum.hpp"
#include "../datum/box.hpp"

#include "../physics/mathematics.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
#define UCHAR(v) static_cast<unsigned char>(flround(v * 255.0))
#define GAMUT(v) (double(v) / 255.0)

static void feed_rgb(double r, double g, double b, unsigned char* red, unsigned char* green, unsigned char* blue) {
    SET_BOX(red, UCHAR(r));
    SET_BOX(green, UCHAR(g));
    SET_BOX(blue, UCHAR(b));
}

static void feed_rgb_from_hue(double hue, double chroma, double m, unsigned char* red, unsigned char* green, unsigned char* blue) {
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

    feed_rgb(r, g, b, red, green, blue);
}

static void feed_rgb_from_hsi_sector(double hue, double saturation, double intensity, char com, unsigned char* r, unsigned char* g, unsigned char* b) {
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
        case 'r': feed_rgb(major, minor, midor, r, g, b); break;
        case 'g': feed_rgb(midor, major, minor, r, g, b); break;
        default:  feed_rgb(minor, midor, major, r, g, b); break;
        }
    }
}

static double rgb_to_hue(unsigned char red, unsigned char green, unsigned char blue, double* flM, double* flm, double* flchroma) {
    unsigned char M = fxmax(red, green, blue);
    unsigned char m = fxmin(red, green, blue);
    double chroma = double(M) - double(m);
    
    SET_BOX(flchroma, chroma);
    SET_BOX(flM, GAMUT(M));
    SET_BOX(flm, GAMUT(m));
    
    if (chroma == 0.0F) {
        return flnan_f;
    } else if (M == green) {
        return 60.0 * ((double(blue) - double(red)) / chroma + 2.0F);
    } else if (M == blue) {
        return 60.0 * ((double(red) - double(green)) / chroma + 4.0F);
    } else if (green < blue) {
        return 60.0 * ((double(green) - double(blue)) / chroma + 6.0F);
    } else {
        return 60.0 * ((double(green) - double(blue)) / chroma);
    }
}

static inline double color_gamma_encode(double c) {
    return (c <= 0.0031308) ? c * 12.92 : flexpt(c, 1.0 / 2.4 ) * 1.055 - 0.055;
}

static inline double color_gamma_decode(double c) {
    return (c <= 0.04045) ? c / 12.92 : flexpt((c + 0.055) / 1.055, 2.4);
}

/*************************************************************************************************/
unsigned char WarGrey::STEM::color_component_to_byte(double com) {
    return UCHAR(com);
}

unsigned char WarGrey::STEM::color_component_clamp_to_byte(double com) {
    return UCHAR(flmin(flmax(com, 0.0), 1.0));
}

/*************************************************************************************************/
void WarGrey::STEM::RGB_FillColor(SDL_Color* c, unsigned int hex, double alpha) {
    RGB_FillColor(c, hex, UCHAR(alpha));
}

void WarGrey::STEM::RGB_FillColor(SDL_Color* c, unsigned int hex, unsigned char alpha) {
    if (c != nullptr) {
        RGB_From_Hexadecimal(hex, &c->r, &c->g, &c->b);
        c->a = alpha;
    }
}

/*************************************************************************************************/
unsigned int WarGrey::STEM::RGB_Contrast(unsigned int hex) {
    unsigned char r, g, b;
    double h, s, v;
    
    RGB_From_Hexadecimal(hex, &r, &g, &b);
    HSV_From_RGB(r, g, b, &h, &s, &v);

    if (flisnan(h)) {
        return RGB_Contrast_For_Background(hex);
    } else {
        return Hexadecimal_From_HSV(h + 180.0, s, v);
    }
}

unsigned int WarGrey::STEM::RGB_Contrast_For_Background(unsigned int hex) {
    unsigned char r, g, b;
    double perceptive_luminance;

    RGB_From_Hexadecimal(hex, &r, &g, &b);
    
    perceptive_luminance = 1.0 - (double(r) * 0.299 + double(g) * 0.587 + double(b) * 0.114) / 255.0;

	if (perceptive_luminance < 0.5) {
		return 0x000000U;
	} else {
		return 0xFFFFFFU;
	}
}

unsigned int WarGrey::STEM::RGB_Add(unsigned int hex1, unsigned int hex2) {
    unsigned char r1, g1, b1, r2, g2, b2;
    int r, g, b;

    RGB_From_Hexadecimal(hex1, &r1, &g1, &b1);
    RGB_From_Hexadecimal(hex2, &r2, &g2, &b2);

    r = fxmin(int(r1) + int(r2), 255);
    g = fxmin(int(g1) + int(g2), 255);
    b = fxmin(int(b1) + int(b2), 255);

    return Hexadecimal_From_RGB(static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b));
}

/*************************************************************************************************/
int WarGrey::STEM::RGB_SetRenderDrawColor(SDL_Renderer* renderer, unsigned int hex, double alpha) {
    return RGB_SetRenderDrawColor(renderer, hex, UCHAR(alpha));
}

int WarGrey::STEM::RGB_SetRenderDrawColor(SDL_Renderer* renderer, unsigned int hex, unsigned char alpha) {
    unsigned char r, g, b;

    RGB_From_Hexadecimal(hex, &r, &g, &b);
    
    return SDL_SetRenderDrawColor(renderer, r, g, b, alpha);
}

int WarGrey::STEM::HSV_SetRenderDrawColor(SDL_Renderer* renderer, double hue, double saturation, double brightness, double alpha) {
    return HSV_SetRenderDrawColor(renderer, hue, saturation, brightness, UCHAR(alpha));
}

int WarGrey::STEM::HSV_SetRenderDrawColor(SDL_Renderer* renderer, double hue, double saturation, double brightness, unsigned char alpha) {
    unsigned char r, g, b;

    RGB_From_HSV(hue, saturation, brightness, &r, &g, &b);
    
    return SDL_SetRenderDrawColor(renderer, r, g, b, alpha);
}

int WarGrey::STEM::HSL_SetRenderDrawColor(SDL_Renderer* renderer, double hue, double saturation, double lightness, double alpha) {
    return HSV_SetRenderDrawColor(renderer, hue, saturation, lightness, UCHAR(alpha));
}

int WarGrey::STEM::HSL_SetRenderDrawColor(SDL_Renderer* renderer, double hue, double saturation, double lightness, unsigned char alpha) {
    unsigned char r, g, b;

    RGB_From_HSV(hue, saturation, lightness, &r, &g, &b);
    
    return SDL_SetRenderDrawColor(renderer, r, g, b, alpha);
}

int WarGrey::STEM::HSI_SetRenderDrawColor(SDL_Renderer* renderer, double hue, double saturation, double intensity, double alpha) {
    return HSV_SetRenderDrawColor(renderer, hue, saturation, intensity, UCHAR(alpha));
}

int WarGrey::STEM::HSI_SetRenderDrawColor(SDL_Renderer* renderer, double hue, double saturation, double intensity, unsigned char alpha) {
    unsigned char r, g, b;

    RGB_From_HSV(hue, saturation, intensity, &r, &g, &b);
    
    return SDL_SetRenderDrawColor(renderer, r, g, b, alpha);
}

/*************************************************************************************************/
void WarGrey::STEM::RGB_From_HSV(double hue, double saturation, double brightness, unsigned char* r, unsigned char* g, unsigned char* b) {
    double chroma = saturation * brightness;
    double m = brightness - chroma;
    
    feed_rgb_from_hue(hue, chroma, m, r, g, b);
}

void WarGrey::STEM::RGB_From_HSL(double hue, double saturation, double lightness, unsigned char* r, unsigned char* g, unsigned char* b) {
    double chroma = saturation * (1.0f - flabs(lightness * 2.0f - 1.0f));
    double m = lightness - chroma * 0.5f;
    
    feed_rgb_from_hue(hue, chroma, m, r, g, b);
}

void WarGrey::STEM::RGB_From_HSI(double hue, double saturation, double intensity, unsigned char* r, unsigned char* g, unsigned char* b) {
    if ((saturation == 0.0f) || flisnan(hue)) {
        feed_rgb(intensity, intensity, intensity, r, g, b);
    } else if (hue < 120.0f) {
        feed_rgb_from_hsi_sector(hue, saturation, intensity, 'r', r, g, b);
    } else if (hue < 240.0f) {
        feed_rgb_from_hsi_sector(hue - 120.0f, saturation, intensity, 'g', r, g, b);
    } else {
        feed_rgb_from_hsi_sector(hue - 240.0f, saturation, intensity, 'b', r, g, b);
    }
}

/*************************************************************************************************/
double WarGrey::STEM::HSB_Hue_From_RGB(unsigned char red, unsigned char green, unsigned char blue) {
    double hue;

    HSV_From_RGB(red, green, blue, &hue, nullptr, nullptr);

    return hue;
}

double WarGrey::STEM::HSB_Hue_From_Hexadecimal(unsigned int hex) {
    unsigned char r, g, b;

    RGB_From_Hexadecimal(hex, &r, &g, &b);
    
    return HSB_Hue_From_RGB(r, g, b);
}

void WarGrey::STEM::HSV_From_RGB(unsigned char red, unsigned char green, unsigned char blue, double* h, double* s, double* b) {
    double M, m, chroma;
    double hue = rgb_to_hue(red, green, blue, &M, &m, &chroma);

    SET_BOX(h, hue);
    SET_BOX(b, M);
    SET_BOX(s, (M == 0.0) ? 0.0 : chroma / M);
}

void WarGrey::STEM::HSL_From_RGB(unsigned char red, unsigned char green, unsigned char blue, double* h, double* s, double* l) {
    double M, m, chroma;
    double hue = rgb_to_hue(red, green, blue, &M, &m, &chroma);
    double lightness = (M + m) * 0.5;
    double abs_2L_1 = flabs(2.0 * lightness - 1.0);

    SET_BOX(h, hue);
    SET_BOX(s, (abs_2L_1 == 1.0) ? 0.0 : chroma / (1.0 - abs_2L_1));
    SET_BOX(l, lightness);
}

/*************************************************************************************************/
void WarGrey::STEM::RGB_From_Hexadecimal(unsigned int hex, unsigned char* r, unsigned char* g, unsigned char* b) {
    SET_BOX(r, static_cast<unsigned char>((hex >> 16) & 0xFF));
    SET_BOX(g, static_cast<unsigned char>((hex >> 8) & 0xFF));
    SET_BOX(b, static_cast<unsigned char>(hex & 0xFF));
}

void WarGrey::STEM::RGB_From_Hexadecimal(unsigned int hex, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a) {
    SET_BOX(r, static_cast<unsigned char>((hex >> 24) & 0xFF));
    SET_BOX(g, static_cast<unsigned char>((hex >> 16) & 0xFF));
    SET_BOX(b, static_cast<unsigned char>((hex >> 8) & 0xFF));
    SET_BOX(a, static_cast<unsigned char>(hex & 0xFF));
}

void WarGrey::STEM::RGB_From_Hexadecimal(unsigned int hex, unsigned char* r, unsigned char* g, unsigned char* b, double* a) {
    unsigned char alpha;

    RGB_From_Hexadecimal(hex, r, g, b, &alpha);
    SET_BOX(a, GAMUT(alpha));
}


void WarGrey::STEM::RGB_From_Hexadecimal(int hex, unsigned char* r, unsigned char* g, unsigned char* b) {
    RGB_From_Hexadecimal(static_cast<unsigned int>(hex), r, g, b);
}

void WarGrey::STEM::RGB_From_Hexadecimal(int hex, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a) {
    RGB_From_Hexadecimal(static_cast<unsigned int>(hex), r, g, b, a);
}

void WarGrey::STEM::RGB_From_Hexadecimal(int hex, unsigned char* r, unsigned char* g, unsigned char* b, double* a) {
    RGB_From_Hexadecimal(static_cast<unsigned int>(hex), r, g, b, a);
}

unsigned int WarGrey::STEM::Hexadecimal_From_Color(SDL_Color* c, double* a) {
    unsigned char alpha;
    unsigned int hex = Hexadecimal_From_Color(c, &alpha);

    SET_BOX(a, GAMUT(alpha));

    return hex;
}

unsigned int WarGrey::STEM::Hexadecimal_From_Color(SDL_Color* c, unsigned char* alpha) {
    SET_BOX(alpha, c->a);

    return Hexadecimal_From_RGB(c->r, c->g, c->b);
}

unsigned int WarGrey::STEM::Hexadecimal_From_RGB(double red, double green, double blue) {
    unsigned char r = color_component_clamp_to_byte(red);
    unsigned char g = color_component_clamp_to_byte(green);
    unsigned char b = color_component_clamp_to_byte(blue);
     
    return Hexadecimal_From_RGB(r, g, b);
}

unsigned int WarGrey::STEM::Hexadecimal_From_RGB(unsigned char red, unsigned char green, unsigned char blue) {
    return (red << 16U) | (green << 8U) | blue;
}

unsigned int WarGrey::STEM::Hexadecimal_From_HSV(double hue, double saturation, double brightness) {
    unsigned char r, g, b;

    RGB_From_HSV(hue, saturation, brightness, &r, &g, &b);
    
    return Hexadecimal_From_RGB(r, g, b);
}

unsigned int WarGrey::STEM::Hexadecimal_From_HSL(double hue, double saturation, double lightness) {
    unsigned char r, g, b;

    RGB_From_HSL(hue, saturation, lightness, &r, &g, &b);
    
    return Hexadecimal_From_RGB(r, g, b);
}

unsigned int WarGrey::STEM::Hexadecimal_From_HSI(double hue, double saturation, double intensity) {
    unsigned char r, g, b;

    RGB_From_HSI(hue, saturation, intensity, &r, &g, &b);
    
    return Hexadecimal_From_RGB(r, g, b);
}

/*************************************************************************************************/
void WarGrey::STEM::CIE_RGB_normalize(double* R, double* G, double* B) {
    double L = flmax(*R, *G, *B);

    SET_BOX(R, *R / L);
    SET_BOX(G, *G / L);
    SET_BOX(B, *B / L);
}

void WarGrey::STEM::CIE_RGB_to_XYZ(CIE_Standard type, int hex, double* X, double* Y, double* Z, bool gamma) {
    unsigned char r, g, b;

    RGB_From_Hexadecimal(hex, &r, &g, &b);
    CIE_RGB_to_XYZ(type, r, g, b, X, Y, Z, gamma);
}

void WarGrey::STEM::CIE_RGB_to_XYZ(CIE_Standard type, unsigned char R, unsigned char G, unsigned char B, double* X, double* Y, double* Z, bool gamma) {
    CIE_RGB_to_XYZ(type, GAMUT(R), GAMUT(G), GAMUT(B), X, Y, Z, gamma);
}

void WarGrey::STEM::CIE_RGB_to_XYZ(CIE_Standard type, double R, double G, double B, double* X, double* Y, double* Z, bool gamma) {
    if (gamma) {
        R = color_gamma_decode(R);
        G = color_gamma_decode(G);
        B = color_gamma_decode(B);
    }

    switch (type) {
        case CIE_Standard::Primary: {
            SET_BOX(X, 0.49000 * R + 0.31000 * G + 0.20000 * B);
            SET_BOX(Y, 0.17697 * R + 0.81240 * G + 0.01063 * B);
            SET_BOX(Z, 0.00000 * R + 0.01000 * G + 0.99000 * B);
        }; break;
        case CIE_Standard::D65: {
            SET_BOX(X, 0.412453 * R + 0.357580 * G + 0.180423 * B);
            SET_BOX(Y, 0.212671 * R + 0.715160 * G + 0.072169 * B);
            SET_BOX(Z, 0.019334 * R + 0.119193 * G + 0.950227 * B);
        }; break;
    }
}

void WarGrey::STEM::CIE_XYZ_to_RGB(CIE_Standard type, double X, double Y, double Z, double* R, double* G, double* B, bool gamma) {
    switch (type) {
        case CIE_Standard::Primary: {
            SET_BOX(R, +2.36461 * X - 0.89654 * Y - 0.46807 * Z);
            SET_BOX(G, -0.51517 * X + 1.42641 * Y + 0.08876 * Z);
            SET_BOX(B, +0.00520 * X - 0.01441 * Y + 1.00920 * Z);
        }; break;
        case CIE_Standard::D65: {
            SET_BOX(R, +3.240479 * X - 1.537150 * Y - 0.498535 * Z);
            SET_BOX(G, -0.969256 * X + 1.875991 * Y + 0.041556 * Z);
            SET_BOX(B, +0.055648 * X - 0.204043 * Y + 1.057311 * Z);
        }; break;
    }

    CIE_RGB_normalize(R, G, B);
    
    if (gamma) {
        if ((*R) >= 0.0) (*R) = color_gamma_encode(*R);
        if ((*G) >= 0.0) (*G) = color_gamma_encode(*G);
        if ((*B) >= 0.0) (*B) = color_gamma_encode(*B);
    }
}

void WarGrey::STEM::CIE_xyY_to_XYZ(double x, double y, double* X, double* Y, double* Z, double L) {
    double z = 1.0 - x - y;

    SET_BOX(X, L * x / y);
    SET_BOX(Y, L);
    SET_BOX(Z, L * z / y);
}

void WarGrey::STEM::CIE_XYZ_to_xyY(double X, double Y, double Z, double* x, double* y) {
    double L = X + Y + Z;

    SET_BOX(x, X / L);
    SET_BOX(y, Y / L);
}
