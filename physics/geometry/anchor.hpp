#pragma once

namespace WarGrey::STEM {
    enum class MatterAnchor { LT, CT, RT, LC, CC, RC, LB, CB, RB };
    
    struct __lambda__ Anchor {
    public:
        Anchor(float frac = 0.0) : Anchor(frac, frac) {}
        Anchor(float fx, float fy) : fx(fx), fy(fy) {}
        Anchor(const WarGrey::STEM::Anchor& a) : fx(a.fx), fy(a.fy) {}
        Anchor(const WarGrey::STEM::MatterAnchor& name);

        void reset(float frac = 0.0F) { this->reset(frac, frac); }
        void reset(float fx, float fy) { this->fx = fx; this->fy = fy; }
        void reset(const WarGrey::STEM::Anchor& a) { this->fx = a.fx; this->fy = a.fy; }
        void reset(const WarGrey::STEM::MatterAnchor& name);

        WarGrey::STEM::Anchor& operator=(float frac) { this->reset(frac); return (*this); }
        WarGrey::STEM::Anchor& operator=(const WarGrey::STEM::Anchor& c) { this->reset(c); return (*this); }
        WarGrey::STEM::Anchor& operator=(const WarGrey::STEM::MatterAnchor& name) { this->reset(name); return (*this); }

    public:
        bool is_zero() const { return (this->fx == 0.0F) && (this->fy == 0.0F); }
        
        friend inline bool operator!=(const WarGrey::STEM::Anchor& lhs, const WarGrey::STEM::Anchor& rhs)
            { return (lhs.fx != rhs.fx) || (lhs.fy != rhs.fy); }

	    friend inline bool operator==(const WarGrey::STEM::Anchor& lhs, const WarGrey::STEM::Anchor& rhs)
            { return (lhs.fx == rhs.fx) && (lhs.fy == rhs.fy); }
        
    public:
        float fx;
        float fy;
    };
}
