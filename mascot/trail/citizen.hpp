#pragma once

#include "../../matter/sprite/folder.hpp"
#include "../../datum/path.hpp"

#include "../../physics/motion/map2d.hpp"

/* Protagonists in the RPG 空の軌跡 */

namespace WarGrey::STEM {
    class Citizen : public WarGrey::STEM::Sprite, protected WarGrey::STEM::I8WayMotion {
    public:
        Citizen(const std::string& fullpath) : Sprite(fullpath) {}
        virtual ~Citizen() {}

        void construct(SDL_Renderer* renderer) override;

    public:
        int preferred_local_fps() override { return 15; }

    protected:
        void on_heading_changed(float theta_rad, float vx, float vy, float prev_vr) override;
        void on_nward(float theta_rad, float vx, float vy) override;
        void on_eward(float theta_rad, float vx, float vy) override;
        void on_sward(float theta_rad, float vx, float vy) override;
        void on_wward(float theta_rad, float vx, float vy) override;
        void on_wnward(float theta_rad, float vx, float vy) override;
        void on_wsward(float theta_rad, float vx, float vy) override;
        void on_enward(float theta_rad, float vx, float vy) override;
        void on_esward(float theta_rad, float vx, float vy) override;
    };

    class TrailKid : public WarGrey::STEM::Citizen {
    public:
        TrailKid(const char* name)
            : Citizen(digimon_mascot_path(name, "", "digitama/big_bang/mascot/trail/Kids")) {}

        virtual ~TrailKid() {}
    };
}
