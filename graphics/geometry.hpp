#pragma once

#include <SDL2/SDL.h>

namespace WarGrey::STEM {
    __lambda__ SDL_RendererFlip game_scales_to_flip(float x_scale, float y_scale);
    __lambda__ void game_flip_to_signs(SDL_RendererFlip flip, float* hsgn = nullptr, float* vsgn = nullptr);
    __lambda__ void game_flip_to_signs(SDL_RendererFlip flip, double* hsgn = nullptr, double* vsgn = nullptr);    
}
