#pragma once

#include <SDL2/SDL.h>

#include "../physics/geometry/aabox.hpp"

namespace GYDM {
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
    inline SDL_BlendMode color_mixture_to_blend_mode(GYDM::ColorMixture mixture) { return static_cast<SDL_BlendMode>(mixture); }
    inline ColorMixture blend_mode_to_color_mixture(SDL_BlendMode mode) { return static_cast<ColorMixture>(mode); }

    /*********************************************************************************************/
    __lambda__ SDL_RendererFlip game_scales_to_flip(float x_scale, float y_scale);
    __lambda__ void game_flip_to_signs(SDL_RendererFlip flip, float* hsgn = nullptr, float* vsgn = nullptr);
    __lambda__ void game_flip_to_signs(SDL_RendererFlip flip, double* hsgn = nullptr, double* vsgn = nullptr);

    /*********************************************************************************************/
    __lambda__ void feed_rect(SDL_Rect* rect, const GYDM::AABox<int>& box);
    __lambda__ void feed_rect(SDL_FRect* rect, const GYDM::Box& box);
}
