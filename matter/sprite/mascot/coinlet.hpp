#pragma once

#include "../folder.hpp"
#include "../../../datum/path.hpp"

/* https://github.com/SuperTux/supertux */

namespace WarGrey::STEM {
    class __lambda__ Coinlet : public WarGrey::STEM::Sprite {
    public:
        Coinlet(const char* name, int idx)
            : WarGrey::STEM::Sprite(WarGrey::STEM::digimon_mascot_path("coinlet", ""))
            , name(name), idx(idx) {}

        Coinlet(const std::string& name, int idx) : Coinlet(name.c_str(), idx) {}
        
        virtual ~Coinlet() {}

    public:
        int preferred_local_fps() override { return 16; }

    protected:
        int get_initial_costume_index() override { return -1; }

    public:
        std::string name;
        int idx;
    };
}
