#pragma once

#include "../folder.hpp"
#include "../../../datum/path.hpp"

#include "../../../physics/motion/platformer.hpp"

#include "creature.hpp"

/* https://github.com/SuperTux/supertux */

namespace GYDM {
    class __lambda__ Tuxmon : public GYDM::Sprite, public virtual GYDM::ICreature, protected virtual GYDM::IPlatformMotion {
    public:
        Tuxmon(bool walk_only = false);
        virtual ~Tuxmon() {}

    public:
        int preferred_local_fps() override { return 15; }

    protected:
        void on_motion_stopped() override;
        void on_heading_changed(double theta_rad, double vx, double vy, double prev_vr) override;
        
    protected:
        void on_walk(double theta_rad, double vx, double vy) override;
        void on_jump(double theta_rad, double vx, double vy) override;

    protected:
        int get_initial_costume_index() override;
        void feed_flip_signs(double* hsgn, double* vsgn) override;
        void horizontal_flip() override { Sprite::flip(true, false); }
    };
}
