#include "geometry.hpp"

#include "../datum/box.hpp"

#include "../physics/mathematics.hpp"

using namespace WarGrey::STEM;


/*************************************************************************************************/
WarGrey::STEM::polygon_vertices WarGrey::STEM::regular_polygon_vertices(size_t n, float radius, float rotation) {
    // for inscribed regular polygon, the radius should be `Rcos(pi/n)`
    float start = degrees_to_radians(rotation);
    float delta = 2.0F * pi_f / float(n);
    polygon_vertices pvs;
    
    for (size_t idx = 0; idx < n; idx++) {
        float theta = start + delta * float(idx);
        float px = radius * flcos(theta);
        float py = radius * flsin(theta);
        
        pvs.push_back(std::pair<float, float>(px, py));
    }

    return pvs;
}

/*************************************************************************************************/
SDL_RendererFlip WarGrey::STEM::game_scales_to_flip(float x_scale, float y_scale) {
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

void WarGrey::STEM::game_flip_to_signs(SDL_RendererFlip flip, double* hsgn, double* vsgn) {
    switch (flip) {
    case SDL_FLIP_HORIZONTAL: SET_VALUES(hsgn, -1.0, vsgn, 1.0); break;
    case SDL_FLIP_VERTICAL: SET_VALUES(hsgn, 1.0, vsgn, -1.0); break;
    case SDL_FLIP_NONE: SET_VALUES(hsgn, 1.0, vsgn, 1.0); break;
    default: SET_VALUES(hsgn, -1.0, vsgn, -1.0); break;
    }
}

void WarGrey::STEM::game_flip_to_signs(SDL_RendererFlip flip, float* hsgn, float* vsgn) {
    double flhsgn, flvsgn;
    
    game_flip_to_signs(flip, &flhsgn, &flvsgn);
    SET_BOX(hsgn, float(flhsgn));
    SET_BOX(vsgn, float(flvsgn));
}
