#pragma once

#include <SDL2/SDL.h>

namespace WarGrey::STEM {
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
}
