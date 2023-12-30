#pragma once

#include "../../datum/string.hpp"

namespace GYDM {
    enum class MatterAnchor { LT, CT, RT, LC, CC, RC, LB, CB, RB };
    
    struct __lambda__ Anchor {
    public:
        Anchor(float frac = 0.0) : Anchor(frac, frac) {}
        Anchor(float fx, float fy) : fx(fx), fy(fy) {}
        Anchor(const GYDM::Anchor& a) : fx(a.fx), fy(a.fy) {}
        Anchor(const GYDM::MatterAnchor& name);

        void reset(float frac = 0.0F) { this->reset(frac, frac); }
        void reset(float fx, float fy) { this->fx = fx; this->fy = fy; }
        void reset(const GYDM::Anchor& a) { this->fx = a.fx; this->fy = a.fy; }
        void reset(const GYDM::MatterAnchor& name);

        GYDM::Anchor& operator=(float frac) { this->reset(frac); return (*this); }
        GYDM::Anchor& operator=(const GYDM::Anchor& c) { this->reset(c); return (*this); }
        GYDM::Anchor& operator=(const GYDM::MatterAnchor& name) { this->reset(name); return (*this); }

    public:
        bool is_zero() const { return (this->fx == 0.0F) && (this->fy == 0.0F); }
        
        friend inline bool operator!=(const GYDM::Anchor& lhs, const GYDM::Anchor& rhs)
            { return (lhs.fx != rhs.fx) || (lhs.fy != rhs.fy); }

	    friend inline bool operator==(const GYDM::Anchor& lhs, const GYDM::Anchor& rhs)
            { return (lhs.fx == rhs.fx) && (lhs.fy == rhs.fy); }

    public:
        std::string desc() const { return make_nstring("(%f, %f)", this->fx, this->fy); }
        
    public:
        float fx;
        float fy;
    };
}
