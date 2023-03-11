#pragma once

#include <SDL2/SDL.h>

namespace WarGrey::STEM {
    /** NOTE: the HSB and HSV are identical **/

    enum class ColorMixture {
        None = 0,
        Add = 1,
        Subtract = 2, Multiply = 2, // Yes, they're identical
        Modulate = 3,
        Alpha = 4
    };

    SDL_BlendMode color_mixture_to_blend_mode(WarGrey::STEM::ColorMixture mixture);
    ColorMixture blend_mode_to_color_mixture(SDL_BlendMode mode);

    unsigned char color_component_normalize(float com);

    void RGB_FillColor(SDL_Color* c, unsigned int hex, float alpha);
    void RGB_FillColor(SDL_Color* c, unsigned int hex, unsigned char alpha = 255U);

    unsigned int Hexadecimal_From_Color(SDL_Color* c, float* alpha = nullptr);
    unsigned int Hexadecimal_From_Color(SDL_Color* c, unsigned char* alpha);

    float HSB_Hue_From_RGB(unsigned char r, unsigned char g, unsigned char b);
    float HSB_Hue_From_Hexadecimal(unsigned int hex);

    unsigned int Hexadecimal_From_RGB(unsigned char red, unsigned char green, unsigned char blue);
    unsigned int Hexadecimal_From_HSV(float hue, float saturation, float brightness);
    unsigned int Hexadecimal_From_HSL(float hue, float saturation, float lightness);
    unsigned int Hexadecimal_From_HSI(float hue, float saturation, float intensity);

    void RGB_From_HSV(float hue, float saturation, float brightness, unsigned char* red, unsigned char* green, unsigned char* blue);
    void RGB_From_HSL(float hue, float saturation, float lightness, unsigned char* red, unsigned char* green, unsigned char* blue);
    void RGB_From_HSI(float hue, float saturation, float intensity, unsigned char* red, unsigned char* green, unsigned char* blue);

    void RGB_From_Hexadecimal(unsigned int hex, unsigned char* red, unsigned char* green, unsigned char* blue);
    void RGB_From_Hexadecimal(unsigned int hex, unsigned char* red, unsigned char* green, unsigned char* blue, float* alpha);
    void RGB_From_Hexadecimal(unsigned int hex, unsigned char* red, unsigned char* green, unsigned char* blue, unsigned char* alpha);

    void RGB_From_Hexadecimal(int hex, unsigned char* red, unsigned char* green, unsigned char* blue);
    void RGB_From_Hexadecimal(int hex, unsigned char* red, unsigned char* green, unsigned char* blue, float* alpha);
    void RGB_From_Hexadecimal(int hex, unsigned char* red, unsigned char* green, unsigned char* blue, unsigned char* alpha);

    int RGB_SetRenderDrawColor(SDL_Renderer* renderer, unsigned int hex, float alpha);
    int RGB_SetRenderDrawColor(SDL_Renderer* renderer, unsigned int hex, unsigned char alpha = 0xFFU);
    int HSV_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float brightness, float alpha);
    int HSV_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float brightness, unsigned char alpha = 0xFFU);
    int HSL_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float lightness, float alpha);
    int HSL_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float lightness, unsigned char alpha = 0xFFU);
    int HSI_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float intensity, float alpha);
    int HSI_SetRenderDrawColor(SDL_Renderer* renderer, float hue, float saturation, float intensity, unsigned char alpha = 0xFFU);
}
