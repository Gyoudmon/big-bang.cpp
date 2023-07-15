#pragma once

#include <SDL2/SDL.h>

namespace WarGrey::STEM {
    /** NOTE: the HSB and HSV are identical **/

    enum class ColorMixture {
        None = SDL_BLENDMODE_NONE,
        Add = SDL_BLENDMODE_ADD,

        // The following two are identical
        Subtract = SDL_BLENDMODE_MUL,
        Multiply = SDL_BLENDMODE_MUL,
        
        Modulate = SDL_BLENDMODE_MOD,
        Alpha = SDL_BLENDMODE_BLEND
    };

    /*********************************************************************************************/
    inline SDL_BlendMode color_mixture_to_blend_mode(WarGrey::STEM::ColorMixture mixture) { return static_cast<SDL_BlendMode>(mixture); }
    inline ColorMixture blend_mode_to_color_mixture(SDL_BlendMode mode) { return static_cast<ColorMixture>(mode); }

    __lambda__ unsigned char color_component_to_byte(double com);
    __lambda__ unsigned char color_component_clamp_to_byte(double com);
    
    __lambda__ unsigned int RGB_Contrast(unsigned int hex);
    __lambda__ unsigned int RGB_Contrast_For_Background(unsigned int hex);
    __lambda__ unsigned int RGB_Add(unsigned int hex1, unsigned int hex2);

    __lambda__ void RGB_FillColor(SDL_Color* c, unsigned int hex, double alpha);
    __lambda__ void RGB_FillColor(SDL_Color* c, unsigned int hex, unsigned char alpha = 255U);

    __lambda__ unsigned int Hexadecimal_From_Color(SDL_Color* c, double* alpha = nullptr);
    __lambda__ unsigned int Hexadecimal_From_Color(SDL_Color* c, unsigned char* alpha);

    __lambda__ unsigned int Hexadecimal_From_RGB(double red, double green, double blue);
    __lambda__ unsigned int Hexadecimal_From_RGB(unsigned char red, unsigned char green, unsigned char blue);
    __lambda__ unsigned int Hexadecimal_From_HSV(double hue, double saturation, double brightness);
    __lambda__ unsigned int Hexadecimal_From_HSL(double hue, double saturation, double lightness);
    __lambda__ unsigned int Hexadecimal_From_HSI(double hue, double saturation, double intensity);

    __lambda__ double HSB_Hue_From_Hexadecimal(unsigned int hex);
    __lambda__ double HSB_Hue_From_RGB(unsigned char r, unsigned char g, unsigned char b);

    __lambda__ void HSV_From_RGB(unsigned char red, unsigned char green, unsigned char blue, double* hue, double* saturation, double* brightness);
    __lambda__ void HSL_From_RGB(unsigned char red, unsigned char green, unsigned char blue, double* hue, double* saturation, double* lightness);
    
    __lambda__ void RGB_From_HSV(double hue, double saturation, double brightness, unsigned char* red, unsigned char* green, unsigned char* blue);
    __lambda__ void RGB_From_HSL(double hue, double saturation, double lightness, unsigned char* red, unsigned char* green, unsigned char* blue);
    __lambda__ void RGB_From_HSI(double hue, double saturation, double intensity, unsigned char* red, unsigned char* green, unsigned char* blue);

    __lambda__ void RGB_From_Hexadecimal(unsigned int hex, unsigned char* red, unsigned char* green, unsigned char* blue);
    __lambda__ void RGB_From_Hexadecimal(unsigned int hex, unsigned char* red, unsigned char* green, unsigned char* blue, double* alpha);
    __lambda__ void RGB_From_Hexadecimal(unsigned int hex, unsigned char* red, unsigned char* green, unsigned char* blue, unsigned char* alpha);

    __lambda__ void RGB_From_Hexadecimal(int hex, unsigned char* red, unsigned char* green, unsigned char* blue);
    __lambda__ void RGB_From_Hexadecimal(int hex, unsigned char* red, unsigned char* green, unsigned char* blue, double* alpha);
    __lambda__ void RGB_From_Hexadecimal(int hex, unsigned char* red, unsigned char* green, unsigned char* blue, unsigned char* alpha);

    __lambda__ int RGB_SetRenderDrawColor(SDL_Renderer* renderer, unsigned int hex, double alpha);
    __lambda__ int RGB_SetRenderDrawColor(SDL_Renderer* renderer, unsigned int hex, unsigned char alpha = 0xFFU);
    __lambda__ int HSV_SetRenderDrawColor(SDL_Renderer* renderer, double hue, double saturation, double brightness, double alpha);
    __lambda__ int HSV_SetRenderDrawColor(SDL_Renderer* renderer, double hue, double saturation, double brightness, unsigned char alpha = 0xFFU);
    __lambda__ int HSL_SetRenderDrawColor(SDL_Renderer* renderer, double hue, double saturation, double lightness, double alpha);
    __lambda__ int HSL_SetRenderDrawColor(SDL_Renderer* renderer, double hue, double saturation, double lightness, unsigned char alpha = 0xFFU);
    __lambda__ int HSI_SetRenderDrawColor(SDL_Renderer* renderer, double hue, double saturation, double intensity, double alpha);
    __lambda__ int HSI_SetRenderDrawColor(SDL_Renderer* renderer, double hue, double saturation, double intensity, unsigned char alpha = 0xFFU);

    /*********************************************************************************************/
    enum class CIE_Standard { Primary , D65 };

    __lambda__ void CIE_RGB_normalize(double* R, double* G, double* B);
    __lambda__ void CIE_RGB_to_XYZ(CIE_Standard type, int hex, double* X, double* Y, double* Z, bool gamma = true);
    __lambda__ void CIE_RGB_to_XYZ(CIE_Standard type, unsigned char R, unsigned char G, unsigned char B, double* X, double* Y, double* Z, bool gamma = true);
    __lambda__ void CIE_RGB_to_XYZ(CIE_Standard type, double R, double G, double B, double* X, double* Y, double* Z, bool gamma = true);
    // RGB might be negative, hence only one version of `CIE_XYZ_to_RGB`
    __lambda__ void CIE_XYZ_to_RGB(CIE_Standard type, double X, double Y, double Z, double* R, double* G, double* B, bool gamma = true);
    __lambda__ void CIE_xyY_to_XYZ(double x, double y, double* X, double* Y, double* Z, double L = 1.0);
    __lambda__ void CIE_XYZ_to_xyY(double X, double Y, double Z, double* x, double* y);
}
