#include "texture.hpp"

#include "../datum/box.hpp"

using namespace GYDM;

/*************************************************************************************************/
void GYDM::Texture::feed_extent(int* width, int* height) {
    if (this->_self != nullptr) {
        SDL_QueryTexture(this->_self, nullptr, nullptr, width, height);
    }
}

void GYDM::Texture::feed_extent(float* width, float* height) {
    int w, h;
    
    this->feed_extent(&w, &h);
    SET_BOX(width, float(w));
    SET_BOX(height, float(h));
}
