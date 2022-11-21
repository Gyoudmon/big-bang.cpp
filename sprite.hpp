#pragma once

#include "matter.hpp"

#include "matter/movable.hpp"

namespace WarGrey::STEM {
    class Sprite : public WarGrey::STEM::IMatter, public WarGrey::STEM::IMovable {
        public:
            Sprite();
    };
}

