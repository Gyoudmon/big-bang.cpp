#include <SDL2/SDL.h>   /* Simple DirectMedia Layer 头文件, 放前面以兼容 macOS */
#include <cmath>

#include "colorspace.hpp"

#include "flonum.hpp"
#include "fixnum.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
#define UCHAR(v) ((unsigned char)flround(v * 255.0))

static int sdl_set_render_draw_color(SDL_Renderer* renderer, float r, float g, float b, unsigned char a) {
    return SDL_SetRenderDrawColor(renderer, UCHAR(r), UCHAR(g), UCHAR(b), a);
}

/*************************************************************************************************/
static int set_render_color_from_hue(SDL_Renderer* renderer, float hue, float chroma, float m, unsigned char a) {
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

    return sdl_set_render_draw_color(renderer, r, g, b, a);
}

static int set_render_color_from_hsi_sector(SDL_Renderer* renderer, float hue, float saturation, float intensity, char color_component, unsigned char alpha) {
    float cosH_60H = 2.0f; // if hue == 0.0 or hue == 120.0;

    if ((hue != 0.0f) && (hue != 120.0f)) {
        float H = hue * (pi_f / 180.0f);
        cosH_60H = flcos(H) / flcos(pi_f / 3.0f - H);
    }

    {
        float major = intensity * (1.0f + saturation * cosH_60H);
        float midor = intensity * (1.0f - saturation);
        float minor = (intensity * 3.0f) - (major + midor);

        switch (color_component) {
        case 'r': return sdl_set_render_draw_color(renderer, major, minor, midor, alpha); break;
        case 'g': return sdl_set_render_draw_color(renderer, midor, major, minor, alpha); break;
        default:  return sdl_set_render_draw_color(renderer, minor, midor, major, alpha); break;
        }
    }
}

/*************************************************************************************************/
int WarGrey::STEM::RGB_SetRenderDrawColor(SDL_Renderer* renderer, unsigned int hex, float alpha) {
    return RGB_SetRenderDrawColor(renderer, hex, UCHAR(alpha));
}

int WarGrey::STEM::RGB_SetRenderDrawColor(SDL_Renderer* renderer, unsigned int hex, unsigned char alpha) {
    unsigned char r, g, b;

    RGB_FromHexadecimal(hex, &r, &g, &b);
    
    return SDL_SetRenderDrawColor(renderer, r, g, b, alpha);
}

/*************************************************************************************************/
int WarGrey::STEM::HSV_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float value, float alpha) {
    return HSV_SetRenderDrawColor(renderer, hue, saturation, value, UCHAR(alpha));
}

int WarGrey::STEM::HSV_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float value, unsigned char alpha) {
    float chroma = saturation * value;
    float m = value - chroma;
    
    return set_render_color_from_hue(renderer, hue, chroma, m, alpha);
}

/*************************************************************************************************/
int WarGrey::STEM::HSL_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float lightness, float alpha) {
    return HSL_SetRenderDrawColor(renderer, hue, saturation, lightness, UCHAR(alpha));
}

int WarGrey::STEM::HSL_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float lightness, unsigned char alpha) {
    float chroma = saturation * (1.0f - flabs(lightness * 2.0f - 1.0f));
    float m = lightness - chroma * 0.5f;
    
    return set_render_color_from_hue(renderer, hue, chroma, m, alpha);
}

/*************************************************************************************************/
int WarGrey::STEM::HSI_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float intensity, float alpha) {
    return HSI_SetRenderDrawColor(renderer, hue, saturation, intensity, UCHAR(alpha));
}

int WarGrey::STEM::HSI_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float intensity, unsigned char alpha) {
    if ((saturation == 0.0f) || flisnan(saturation)) {
        return sdl_set_render_draw_color(renderer, intensity, intensity, intensity, alpha);
    } else if (hue < 120.0f) {
        return set_render_color_from_hsi_sector(renderer, hue, saturation, intensity, 'r', alpha);
    } else if (hue < 240.0f) {
        return set_render_color_from_hsi_sector(renderer, hue - 120.0f, saturation, intensity, 'g', alpha);
    } else {
        return set_render_color_from_hsi_sector(renderer, hue - 240.0f, saturation, intensity, 'b', alpha);
    }
}

/*************************************************************************************************/
void WarGrey::STEM::RGB_FromHexadecimal(unsigned int hex, unsigned char* r, unsigned char* g, unsigned char* b) {
    (*r) = (unsigned char)((hex >> 16) & 0xFF);
    (*g) = (unsigned char)((hex >> 8) & 0xFF);
    (*b) = (unsigned char)(hex & 0xFF);
}

void WarGrey::STEM::RGB_FromHexadecimal(unsigned int hex, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a) {
    (*r) = (unsigned char)((hex >> 24) & 0xFF);
    (*g) = (unsigned char)((hex >> 16) & 0xFF);
    (*b) = (unsigned char)((hex >> 8) & 0xFF);
    (*a) = (unsigned char)(hex & 0xFF);
}

