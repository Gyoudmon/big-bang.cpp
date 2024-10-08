#include "misc.hpp"

#include "../datum/box.hpp"

using namespace Plteen;

/*************************************************************************************************/
SDL_RendererFlip Plteen::game_scales_to_flip(float x_scale, float y_scale) {
    SDL_RendererFlip flip = SDL_FLIP_NONE;

    if ((x_scale < 0.0F) && (y_scale < 0.0F)) {
        flip = static_cast<SDL_RendererFlip>(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
    } else if (x_scale < 0.0F) {
        flip = SDL_FLIP_HORIZONTAL;
    } else if (y_scale < 0.0F) {
        flip = SDL_FLIP_VERTICAL;
    }

    return flip;
}

void Plteen::game_flip_to_signs(SDL_RendererFlip flip, double* hsgn, double* vsgn) {
    switch (flip) {
    case SDL_FLIP_HORIZONTAL: SET_VALUES(hsgn, -1.0, vsgn, 1.0); break;
    case SDL_FLIP_VERTICAL: SET_VALUES(hsgn, 1.0, vsgn, -1.0); break;
    case SDL_FLIP_NONE: SET_VALUES(hsgn, 1.0, vsgn, 1.0); break;
    default: SET_VALUES(hsgn, -1.0, vsgn, -1.0); break;
    }
}

void Plteen::game_flip_to_signs(SDL_RendererFlip flip, float* hsgn, float* vsgn) {
    double flhsgn, flvsgn;
    
    game_flip_to_signs(flip, &flhsgn, &flvsgn);
    SET_BOX(hsgn, float(flhsgn));
    SET_BOX(vsgn, float(flvsgn));
}

/*************************************************************************************************/
void Plteen::feed_rect(SDL_Rect* rect, const AABox<int>& box) {
    rect->x = box.x();
    rect->y = box.y();
    rect->w = box.width();
    rect->h = box.height();
}

void Plteen::feed_rect(SDL_FRect* rect, const Box& box) {
    rect->x = box.x();
    rect->y = box.y();
    rect->w = box.width();
    rect->h = box.height();
}
