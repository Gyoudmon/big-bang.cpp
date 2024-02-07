#pragma once

#include "../forward.hpp"

#include "../physics/algebra/point.hpp"
#include "../physics/algebra/vector.hpp"
#include "../physics/geometry/port.hpp"

namespace GYDM {
    class __lambda__ Position {
    public:
        Position() : Position(0.0F, 0.0F) {}
        Position(float x, float y) : dot(x, y) {}
        Position(const GYDM::Dot& dot) : Position(dot.x, dot.y) {}
        Position(const GYDM::Vector& vec) : Position(vec.x, vec.y) {}

        template<typename T>
        Position(T x, T y) : Position(float(x), float(y)) {}

        Position(const GYDM::IMatter* target, const GYDM::Port& port)
            : dot(port), xtarget(target) {}

        Position(const GYDM::IMatter* xtarget, const GYDM::Port& xport,
                    const GYDM::IMatter* ytarget, const GYDM::Port& yport)
            : dot(xport.fx, yport.fy), xtarget(xtarget), ytarget(ytarget) {}

        Position(const GYDM::Position& pos);

        ~Position() noexcept { /* instances of Position don't own matters */ }

    public:
        GYDM::Position& operator=(const GYDM::Position& c);

    public:
        GYDM::Dot calculate_point() const;
        GYDM::Vector get_offset() const { return this->offset; }
        void set_offset(const GYDM::Vector& vec) { this->offset = vec; }

    public:
        std::string desc() const;

    private:
        GYDM::Port dot; // also serves as an absolute location
        const GYDM::IMatter* xtarget = nullptr;
        const GYDM::IMatter* ytarget = nullptr;
        GYDM::Vector offset;
    };
}
