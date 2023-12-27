#pragma once

#include <SDL2/SDL.h>

/*************************************************************************************************/
namespace GYDM {
    bool inline is_shift_pressed() { return (SDL_GetModState() & KMOD_SHIFT); }
    bool inline is_ctrl_pressed() { return (SDL_GetModState() & KMOD_CTRL); }
    bool inline is_alt_pressed() { return (SDL_GetModState() & KMOD_ALT); }

    void inline feed_current_mouse_location(float* mx, float* my) {
        int fxmx, fxmy;

        SDL_GetMouseState(&fxmx, &fxmy);
        
        if (mx != nullptr) *mx = float(fxmx);
        if (my != nullptr) *my = float(fxmy);
    }
}
