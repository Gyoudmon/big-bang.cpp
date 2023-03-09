#pragma once

#include <SDL2/SDL.h>

namespace WarGrey::STEM {
    enum class ColorMixture {
        None = 0,
        Add = 1,
        Subtract = 2, Multiply = 2, // Yes, they're identical
        Modulate = 3,
        Alpha = 4
    };

    SDL_BlendMode color_mixture_to_blend_mode(WarGrey::STEM::ColorMixture mixture);
    ColorMixture blend_mode_to_color_mixture(SDL_BlendMode mode);

    void RGB_FillColor(SDL_Color* c, unsigned int hex, float alpha);
    void RGB_FillColor(SDL_Color* c, unsigned int hex, unsigned char alpha = 255U);

    unsigned int Hexadecimal_FromColor(SDL_Color* c, float* alpha = nullptr);
    unsigned int Hexadecimal_FromColor(SDL_Color* c, unsigned char* alpha);

    void RGB_FromHexadecimal(unsigned int hex, unsigned char* red, unsigned char* green, unsigned char* blue);
    void RGB_FromHexadecimal(unsigned int hex, unsigned char* red, unsigned char* green, unsigned char* blue, float* alpha);
    void RGB_FromHexadecimal(unsigned int hex, unsigned char* red, unsigned char* green, unsigned char* blue, unsigned char* alpha);

    void RGB_FromHexadecimal(int hex, unsigned char* red, unsigned char* green, unsigned char* blue);
    void RGB_FromHexadecimal(int hex, unsigned char* red, unsigned char* green, unsigned char* blue, float* alpha);
    void RGB_FromHexadecimal(int hex, unsigned char* red, unsigned char* green, unsigned char* blue, unsigned char* alpha);

    int RGB_SetRenderDrawColor(SDL_Renderer* renderer, unsigned int hex, float alpha);
    int RGB_SetRenderDrawColor(SDL_Renderer* renderer, unsigned int hex, unsigned char alpha = 0xFFU);
    int HSV_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float value, float alpha);
    int HSV_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float value, unsigned char alpha = 0xFFU);
    int HSL_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float lightness, float alpha);
    int HSL_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float lightness, unsigned char alpha = 0xFFU);
    int HSI_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float intensity, float alpha);
    int HSI_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float intensity, unsigned char alpha = 0xFFU);
}
