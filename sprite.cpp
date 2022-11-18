#include "sprite.hpp"
#include "image.hpp"

using namespace WarGrey::STEM;

/**************************************************************************************************/
void WarGrey::STEM::ISprite::fill_margin(float x, float y, float* top, float* right, float* bottom, float* left) {
    if (top != nullptr) (*top) = 0.0F;
    if (bottom != nullptr) (*bottom) = 0.0F;
    if (left != nullptr) (*left) = 0.0F;
    if (right != nullptr) (*right) = 0.0F;
}

SDL_Surface* WarGrey::STEM::ISprite::snapshot() {
    SDL_Surface* photograph = nullptr;
    float width, height;

    this->fill_extent(0.0F, 0.0F, &width, &height);
    photograph = game_blank_image(width, height);

    if (photograph != nullptr) {
        SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(photograph);
        
        if (renderer != nullptr) {
            this->draw(renderer, 0, 0, width, height);
            SDL_RenderPresent(renderer);
            SDL_DestroyRenderer(renderer);
        }
    }

    return photograph;
}

bool WarGrey::STEM::ISprite::save_snapshot(const std::string& pname) {
    return this->save_snapshot(pname.c_str());
}

bool WarGrey::STEM::ISprite::save_snapshot(const char* pname) {
    SDL_Surface* photograph = this->snapshot();
    bool okay = game_save_image(photograph, pname);

    SDL_FreeSurface(photograph);

    return okay;
}

