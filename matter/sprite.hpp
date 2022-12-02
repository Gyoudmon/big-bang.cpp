#pragma once

#include "../matter.hpp"

#include "movable.hpp"

namespace WarGrey::STEM {
    class ISprite : public WarGrey::STEM::IMatter, public WarGrey::STEM::IMovable {
        public:
            ISprite();
    };
}
