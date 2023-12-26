#pragma once

namespace WarGrey::STEM {
    enum class MatterAnchor { LT, CT, RT, LC, CC, RC, LB, CB, RB };
    
    struct Anchor {
    public:
        Anchor(float fx = 0.0) : Anchor(fx, fx) {}
        Anchor(float fx, float fy) : fx(fx), fy(fy) {}
        Anchor(const WarGrey::STEM::MatterAnchor& name);

        void reset(float fx = 0.0F) { this->reset(fx, fx); }
        void reset(float fx, float fy) { this->fx = fx; this->fy = fy; }
        void reset(const WarGrey::STEM::MatterAnchor& name);

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
