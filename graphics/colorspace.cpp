#include <SDL2/SDL.h>   /* Simple DirectMedia Layer 头文件, 放前面以兼容 macOS */
#include <cmath>

#include "colorspace.hpp"

#include "../datum/flonum.hpp"
#include "../datum/fixnum.hpp"
#include "../datum/box.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
#define UCHAR(v) static_cast<unsigned char>(flround(v * 255.0F))

static void feed_rgb(float r, float g, float b, unsigned char* red, unsigned char* green, unsigned char* blue) {
    SET_BOX(red, UCHAR(r));
    SET_BOX(green, UCHAR(g));
    SET_BOX(blue, UCHAR(b));
}

static void feed_rgb_from_hue(float hue, float chroma, float m, unsigned char* red, unsigned char* green, unsigned char* blue) {
    float r = m;
    float g = m;
    float b = m;
    
    if (!flisnan(hue)) {
        float hue_60 = hue / 60.0f;
        float flhue = flfloor(hue_60);
        int fxhue = int(flhue);
        float x = chroma * (1.0f - flabs(float(fxhue % 2) - (flhue - hue_60) - 1.0f));
        
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

static void feed_rgb_from_hsi_sector(float hue, float saturation, float intensity, char com, unsigned char* r, unsigned char* g, unsigned char* b) {
    float cosH_60H = 2.0f; // if hue == 0.0 or hue == 120.0;

    if ((hue != 0.0f) && (hue != 120.0f)) {
        float H = hue * (pi_f / 180.0f);
        cosH_60H = flcos(H) / flcos(pi_f / 3.0f - H);
    }

    {
        float major = intensity * (1.0f + saturation * cosH_60H);
        float midor = intensity * (1.0f - saturation);
        float minor = (intensity * 3.0f) - (major + midor);

        switch (com) {
        case 'r': feed_rgb(major, minor, midor, r, g, b); break;
        case 'g': feed_rgb(midor, major, minor, r, g, b); break;
        default:  feed_rgb(minor, midor, major, r, g, b); break;
        }
    }
}

/*************************************************************************************************/
unsigned char WarGrey::STEM::color_component_normalize(float com) {
    return UCHAR(com);
}

SDL_BlendMode WarGrey::STEM::color_mixture_to_blend_mode(ColorMixture mixture) {
    SDL_BlendMode mode = SDL_BLENDMODE_NONE;

    switch (mixture) {
        case ColorMixture::Add: mode = SDL_BLENDMODE_ADD; break;
        case ColorMixture::Subtract: /* case ColorMixture::Multiply: */ mode = SDL_BLENDMODE_MUL; break;
        case ColorMixture::Alpha: mode = SDL_BLENDMODE_BLEND; break;
        case ColorMixture::Modulate: mode = SDL_BLENDMODE_MOD; break;
        default: mode = SDL_BLENDMODE_NONE;
    }

    return mode;
}

ColorMixture WarGrey::STEM::blend_mode_to_color_mixture(SDL_BlendMode mode) {
    ColorMixture mixture = ColorMixture::None;

    switch (mode) {
        case SDL_BLENDMODE_ADD: mixture = ColorMixture::Add; break;
        case SDL_BLENDMODE_MUL: mixture = ColorMixture::Subtract; break;
        case SDL_BLENDMODE_BLEND: mixture = ColorMixture::Alpha; break;
        case SDL_BLENDMODE_MOD: mixture = ColorMixture::Modulate; break;
        default: mixture = ColorMixture::None;
    }

    return mixture;
}

/*************************************************************************************************/
void WarGrey::STEM::RGB_FillColor(SDL_Color* c, unsigned int hex, float alpha) {
    RGB_FillColor(c, hex, UCHAR(alpha));
}

void WarGrey::STEM::RGB_FillColor(SDL_Color* c, unsigned int hex, unsigned char alpha) {
    if (c != nullptr) {
        RGB_From_Hexadecimal(hex, &c->r, &c->g, &c->b);
        c->a = alpha;
    }
}

/*************************************************************************************************/
int WarGrey::STEM::RGB_SetRenderDrawColor(SDL_Renderer* renderer, unsigned int hex, float alpha) {
    return RGB_SetRenderDrawColor(renderer, hex, UCHAR(alpha));
}

int WarGrey::STEM::RGB_SetRenderDrawColor(SDL_Renderer* renderer, unsigned int hex, unsigned char alpha) {
    unsigned char r, g, b;

    RGB_From_Hexadecimal(hex, &r, &g, &b);
    
    return SDL_SetRenderDrawColor(renderer, r, g, b, alpha);
}

int WarGrey::STEM::HSV_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float brightness, float alpha) {
    return HSV_SetRenderDrawColor(renderer, hue, saturation, brightness, UCHAR(alpha));
}

int WarGrey::STEM::HSV_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float brightness, unsigned char alpha) {
    unsigned char r, g, b;

    RGB_From_HSV(hue, saturation, brightness, &r, &g, &b);
    
    return SDL_SetRenderDrawColor(renderer, r, g, b, alpha);
}

int WarGrey::STEM::HSL_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float lightness, float alpha) {
    return HSV_SetRenderDrawColor(renderer, hue, saturation, lightness, UCHAR(alpha));
}

int WarGrey::STEM::HSL_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float lightness, unsigned char alpha) {
    unsigned char r, g, b;

    RGB_From_HSV(hue, saturation, lightness, &r, &g, &b);
    
    return SDL_SetRenderDrawColor(renderer, r, g, b, alpha);
}

int WarGrey::STEM::HSI_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float intensity, float alpha) {
    return HSV_SetRenderDrawColor(renderer, hue, saturation, intensity, UCHAR(alpha));
}

int WarGrey::STEM::HSI_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float intensity, unsigned char alpha) {
    unsigned char r, g, b;

    RGB_From_HSV(hue, saturation, intensity, &r, &g, &b);
    
    return SDL_SetRenderDrawColor(renderer, r, g, b, alpha);
}

/*************************************************************************************************/
void WarGrey::STEM::RGB_From_HSV(float hue, float saturation, float brightness, unsigned char* r, unsigned char* g, unsigned char* b) {
    float chroma = saturation * brightness;
    float m = brightness - chroma;
    
    feed_rgb_from_hue(hue, chroma, m, r, g, b);
}

void WarGrey::STEM::RGB_From_HSL(float hue, float saturation, float lightness, unsigned char* r, unsigned char* g, unsigned char* b) {
    float chroma = saturation * (1.0f - flabs(lightness * 2.0f - 1.0f));
    float m = lightness - chroma * 0.5f;
    
    feed_rgb_from_hue(hue, chroma, m, r, g, b);
}

void WarGrey::STEM::RGB_From_HSI(float hue, float saturation, float intensity, unsigned char* r, unsigned char* g, unsigned char* b) {
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

void WarGrey::STEM::RGB_From_Hexadecimal(unsigned int hex, unsigned char* r, unsigned char* g, unsigned char* b, float* a) {
    unsigned char alpha;

    RGB_From_Hexadecimal(hex, r, g, b, &alpha);
    SET_BOX(a, float(alpha) / 255.0F);
}


void WarGrey::STEM::RGB_From_Hexadecimal(int hex, unsigned char* r, unsigned char* g, unsigned char* b) {
    RGB_From_Hexadecimal(static_cast<unsigned int>(hex), r, g, b);
}

void WarGrey::STEM::RGB_From_Hexadecimal(int hex, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a) {
    RGB_From_Hexadecimal(static_cast<unsigned int>(hex), r, g, b, a);
}

void WarGrey::STEM::RGB_From_Hexadecimal(int hex, unsigned char* r, unsigned char* g, unsigned char* b, float* a) {
    RGB_From_Hexadecimal(static_cast<unsigned int>(hex), r, g, b, a);
}

unsigned int WarGrey::STEM::Hexadecimal_From_Color(SDL_Color* c, float* a) {
    unsigned char alpha;
    unsigned int hex = Hexadecimal_From_Color(c, &alpha);

    SET_BOX(a, float(alpha) / 255.0F);

    return hex;
}

unsigned int WarGrey::STEM::Hexadecimal_From_Color(SDL_Color* c, unsigned char* alpha) {
    SET_BOX(alpha, c->a);

    return Hexadecimal_From_RGB(c->r, c->g, c->b);
}

unsigned int WarGrey::STEM::Hexadecimal_From_RGB(unsigned char red, unsigned char green, unsigned char blue) {
    return (red << 16U) | (green << 8U) | blue;
}

unsigned int WarGrey::STEM::Hexadecimal_From_HSV(float hue, float saturation, float brightness) {
    unsigned char r, g, b;

    RGB_From_HSV(hue, saturation, brightness, &r, &g, &b);
    
    return Hexadecimal_From_RGB(r, g, b);
}

unsigned int WarGrey::STEM::Hexadecimal_From_HSL(float hue, float saturation, float lightness) {
    unsigned char r, g, b;

    RGB_From_HSL(hue, saturation, lightness, &r, &g, &b);
    
    return Hexadecimal_From_RGB(r, g, b);
}

unsigned int WarGrey::STEM::Hexadecimal_From_HSI(float hue, float saturation, float intensity) {
    unsigned char r, g, b;

    RGB_From_HSI(hue, saturation, intensity, &r, &g, &b);
    
    return Hexadecimal_From_RGB(r, g, b);
}

/*************************************************************************************************/
float WarGrey::STEM::HSB_Hue_From_RGB(unsigned char red, unsigned char green, unsigned char blue) {
    unsigned char M = fxmax(red, green, blue);
    unsigned char m = fxmin(red, green, blue);
    float chroma = float(M) - float(m);
    
    if (chroma == 0.0F) {
        return flnan_f;
    } else if (M == green) {
        return 60.0F * ((float(blue) - float(red)) / chroma + 2.0F);
    } else if (M == blue) {
        return 60.0F * ((float(red) - float(green)) / chroma + 4.0F);
    } else if (green < blue) {
        return 60.0F * ((float(green) - float(blue)) / chroma + 6.0F);
    } else {
        return 60.0F * ((float(green) - float(blue)) / chroma);
    }
}

float WarGrey::STEM::HSB_Hue_From_Hexadecimal(unsigned int hex) {
    unsigned char r, g, b;

    RGB_From_Hexadecimal(hex, &r, &g, &b);
    
    return HSB_Hue_From_RGB(r, g, b);
}
