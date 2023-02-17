#pragma once

#include "../matter/sprite/folder.hpp"
#include "../datum/path.hpp"

/* https://github.com/SuperTux/supertux */

namespace WarGrey::STEM {
    class Tuxmon : public WarGrey::STEM::Sprite {
    public:
        Tuxmon();
        virtual ~Tuxmon() {}

    public:
        int preferred_local_fps() override { return 10; }

    protected:
        int get_initial_costume_index() override;
    };
}
