#pragma once

#include "../folder.hpp"
#include "../../../datum/path.hpp"

/* https://github.com/SuperTux/supertux */

namespace WarGrey::STEM {
    class __lambda__ Coinlet : public WarGrey::STEM::Sprite {
    public:
        Coinlet(const std::string& name, int idx) : Coinlet(name.c_str(), idx) {}
        Coinlet(const char* name, int idx)
            : WarGrey::STEM::Sprite(WarGrey::STEM::digimon_mascot_path("coinlet", ""))
            , _name(name), _idx(idx) {}
        virtual ~Coinlet() noexcept {}

    public:
        int preferred_local_fps() override { return 16; }
        const char* name() override { return this->_name.c_str(); }

    public:
        int get_index() { return this->_idx; }

    protected:
        int get_initial_costume_index() override { return -1; }

    private:
        std::string _name;
        int _idx;
    };
}
