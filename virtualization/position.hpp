#pragma once

#include "../forward.hpp"

#include "../physics/geometry/point.hpp"
#include "../physics/geometry/anchor.hpp"
#include "../physics/geometry/vector.hpp"

namespace WarGrey::STEM {
    class __lambda__ Position {
    public:
        Position() : Position(0.0F, 0.0F) {}
        Position(float x, float y) : dot(x, y) {}
        Position(const WarGrey::STEM::Dot& dot) : Position(dot.x, dot.y) {}
        Position(const WarGrey::STEM::Vector& vec) : Position(vec.x, vec.y) {}

        template<typename T>
        Position(T x, T y) : Position(float(x), float(y)) {}

        Position(const WarGrey::STEM::IMatter* target, const WarGrey::STEM::Anchor& anchor)
            : dot(anchor), xtarget(target) {}

        Position(const WarGrey::STEM::IMatter* xtarget, const WarGrey::STEM::Anchor& xanchor,
                    const WarGrey::STEM::IMatter* ytarget, const WarGrey::STEM::Anchor& yanchor)
            : dot(xanchor.fx, yanchor.fy), xtarget(xtarget), ytarget(ytarget) {}

        Position(const WarGrey::STEM::Position& pos) = default;
        Position(WarGrey::STEM::Position&& pos) = default;

        ~Position() noexcept { /* instances of Position don't own matters */ }

    public:
        WarGrey::STEM::Position& operator=(const WarGrey::STEM::Position& c) = default;
        WarGrey::STEM::Position& operator=(WarGrey::STEM::Position&& c) = default;

    public:
        WarGrey::STEM::Dot calculate_dot() const;
        WarGrey::STEM::Vector get_offset() const { return this->offset; }
        void set_offset(const WarGrey::STEM::Vector& vec) { this->offset = vec; }

    private:
        WarGrey::STEM::Anchor dot; // also serves as an absolute location
        const WarGrey::STEM::IMatter* xtarget = nullptr;
        const WarGrey::STEM::IMatter* ytarget = nullptr;
        WarGrey::STEM::Vector offset;
    };

    static const Position origin_position;
}
