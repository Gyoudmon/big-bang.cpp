#pragma once

#include "../algebra/point.hpp"
#include "../algebra/vector.hpp"
#include "port.hpp"

#include "../../datum/flonum.hpp"
#include "../mathematics.hpp"

namespace GYDM {
    class __lambda__ Margin {
    public:
        Margin() : Margin(0.0F) {}
        Margin(float s) : Margin(s, s, s, s) {}
        Margin(float v, float h) : Margin(v, h, v, h) {}
        Margin(float t, float r, float b, float l) : top(t), right(r), bottom(b), left(l) {}
        Margin(const GYDM::Margin& m) : top(m.top), right(m.right), bottom(m.bottom), left(m.left) {}
        Margin(const GYDM::Margin& m, float sx, float sy) : Margin(m) { this->scale(sx, sy); }
        
        GYDM::Margin& operator=(const GYDM::Margin& m) {
            this->left = m.left;
            this->top = m.top;
            this->bottom = m.bottom;
            this->right = m.right;
           
            return (*this);
        }

        ~Margin() noexcept {}

    public:
        GYDM::Dot ltdot() const { return { this->left, this->top }; }
        GYDM::Dot rtdot() const { return { this->right, this->top }; }
        GYDM::Dot lbdot() const { return { this->left, this->bottom }; }
        GYDM::Dot rbdot() const { return { this->right, this->top }; }

        float vertical() const { return this->top + this->bottom; }
        float horizon() const { return this->left + this->right; }

    public:
        bool is_zero() const {
            return (this->left == 0.0F) && (this->right == 0.0F)
                    && (this->top == 0.0F) && (this->bottom == 0.0F);
        }

        GYDM::Margin& scale(float s) { return this->scale(s, s); }
        GYDM::Margin& scale(float sx, float sy);

    public:
        bool operator!=(const Margin& m) const { return !(this->operator==(m)); }
        bool operator==(const Margin& m) const {
            return (this->top == m.top) && (this->right == m.right)
                    && (this->bottom == m.bottom) && (this->left == m.left);
        }
        
        GYDM::Margin operator*(float s) const { GYDM::Margin m(*this); m *= s; return m; }
        GYDM::Margin operator/(float d) const { GYDM::Margin m(*this); m /= d; return m; }

        friend inline GYDM::Margin operator*(float lhs, GYDM::Margin rhs) { return rhs *= lhs; }

        GYDM::Margin& operator*=(float s) { return this->scale(s, s); }
        GYDM::Margin& operator/=(float d) { return this->scale(1.0F / d); }

    public:
        float top = 0.0F;
        float right = 0.0F;
        float bottom = 0.0F;
        float left = 0.0F;
    };
}
