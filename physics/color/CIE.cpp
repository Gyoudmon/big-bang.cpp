#include "CIE.hpp"

#include "../../datum/box.hpp"

#include "../mathematics.hpp"

using namespace GYDM;

/*************************************************************************************************/
static inline double color_gamma_encode(double c) {
    return (c <= 0.0031308) ? c * 12.92 : flexpt(c, 1.0 / 2.4 ) * 1.055 - 0.055;
}

static inline double color_gamma_decode(double c) {
    return (c <= 0.04045) ? c / 12.92 : flexpt((c + 0.055) / 1.055, 2.4);
}

static inline void cie_rgb_normalize(double* R, double* G, double* B) {
    double L = flmax(*R, *G, *B);

    SET_BOX(R, *R / L);
    SET_BOX(G, *G / L);
    SET_BOX(B, *B / L);
}

/*************************************************************************************************/
void GYDM::CIE_XYZ_to_RGB(CIE_Standard type, double X, double Y, double Z, double* R, double* G, double* B, bool gamma) {
    switch (type) {
    case CIE_Standard::Primary: {
        SET_BOX(R, +2.36461 * X - 0.89654 * Y - 0.46807 * Z);
        SET_BOX(G, -0.51517 * X + 1.42641 * Y + 0.08876 * Z);
        SET_BOX(B, +0.00520 * X - 0.01441 * Y + 1.00920 * Z);
    }; break;
    case CIE_Standard::D65: {
        SET_BOX(R, +3.240479 * X - 1.537150 * Y - 0.498535 * Z);
        SET_BOX(G, -0.969256 * X + 1.875991 * Y + 0.041556 * Z);
        SET_BOX(B, +0.055648 * X - 0.204043 * Y + 1.057311 * Z);
    }; break;
    }

    cie_rgb_normalize(R, G, B);
    
    if (gamma) {
        if ((*R) >= 0.0) (*R) = color_gamma_encode(*R);
        if ((*G) >= 0.0) (*G) = color_gamma_encode(*G);
        if ((*B) >= 0.0) (*B) = color_gamma_encode(*B);
    }
}

void GYDM::CIE_RGB_to_XYZ(CIE_Standard type, double R, double G, double B, double* X, double* Y, double* Z, bool gamma) {
    if (gamma) {
        R = color_gamma_decode(R);
        G = color_gamma_decode(G);
        B = color_gamma_decode(B);
    }

    switch (type) {
    case CIE_Standard::Primary: {
        SET_BOX(X, 0.49000 * R + 0.31000 * G + 0.20000 * B);
        SET_BOX(Y, 0.17697 * R + 0.81240 * G + 0.01063 * B);
        SET_BOX(Z, 0.00000 * R + 0.01000 * G + 0.99000 * B);
    }; break;
    case CIE_Standard::D65: {
        SET_BOX(X, 0.412453 * R + 0.357580 * G + 0.180423 * B);
        SET_BOX(Y, 0.212671 * R + 0.715160 * G + 0.072169 * B);
        SET_BOX(Z, 0.019334 * R + 0.119193 * G + 0.950227 * B);
    }; break;
    }
}

void GYDM::CIE_xyY_to_XYZ(double x, double y, double* X, double* Y, double* Z, double L) {
    double z = 1.0 - x - y;

    SET_BOX(X, L * x / y);
    SET_BOX(Y, L);
    SET_BOX(Z, L * z / y);
}

void GYDM::CIE_XYZ_to_xyY(double X, double Y, double Z, double* x, double* y) {
    double L = X + Y + Z;

    SET_BOX(x, X / L);
    SET_BOX(y, Y / L);
}
