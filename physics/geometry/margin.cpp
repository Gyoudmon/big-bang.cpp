#include "margin.hpp"

using namespace GYDM;

/*************************************************************************************************/
GYDM::Margin& GYDM::Margin::scale(float sx, float sy) {
    if (sx >= 0.0F) {
        this->left *= sx;
        this->right *= sx;
    } else {
        this->left = -this->right * sx;
        this->right = -this->left * sx;
    }
    
    if (sy >= 0.0F) {
        this->top *= sy;
        this->bottom *= sy;
    } else {
        this->top = -this->bottom * sy;
        this->bottom = -this->top * sy;
    }

    return (*this);
}
