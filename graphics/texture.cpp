#include "texture.hpp"

#include "../datum/box.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
void WarGrey::STEM::Texture::feed_extent(int* width, int* height) {
    if (this->_self != nullptr) {
        SDL_QueryTexture(this->_self, nullptr, nullptr, width, height);
    }
}

void WarGrey::STEM::Texture::feed_extent(float* width, float* height) {
    int w, h;
    
    this->feed_extent(&w, &h);
    SET_BOX(width, float(w));
    SET_BOX(height, float(h));
}
