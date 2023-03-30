#pragma once

#include "../../matter/sprite/folder.hpp"
#include "../../datum/path.hpp"

#include "../../physics/motion/map2d.hpp"

#include <vector>

/* Protagonists in the RPG 空の軌跡 */

namespace WarGrey::STEM {
    #define TRIAL_KIDS_PATH "digitama/big_bang/mascot/trail/Kids"
    #define TRIAL_STUDENTS_PATH "digitama/big_bang/mascot/trail/Students"
    #define TRIAL_SPECIALS_PATH "digitama/big_bang/mascot/trail/Specials"

    class Citizen : public WarGrey::STEM::Sprite, protected WarGrey::STEM::I8WayMotion {
    public:
        static std::vector<std::string> list_special_names();
        static Citizen* create_special(const std::string& name) { return Citizen::create_special(name.c_str()); }
        static Citizen* create_special(const char* name) { return new Citizen(digimon_mascot_path(name, "", TRIAL_SPECIALS_PATH)); }

    public:
        Citizen(const std::string& fullpath);
        virtual ~Citizen() {}

        void construct(SDL_Renderer* renderer) override;

    public:
        int preferred_local_fps() override { return 15; }

    protected:
        void on_heading_changed(double theta_rad, double vx, double vy, double prev_vr) override;
        void on_nward(double theta_rad, double vx, double vy) override;
        void on_eward(double theta_rad, double vx, double vy) override;
        void on_sward(double theta_rad, double vx, double vy) override;
        void on_wward(double theta_rad, double vx, double vy) override;
        void on_wnward(double theta_rad, double vx, double vy) override;
        void on_wsward(double theta_rad, double vx, double vy) override;
        void on_enward(double theta_rad, double vx, double vy) override;
        void on_esward(double theta_rad, double vx, double vy) override;
    };

    /*********************************************************************************************/
    // https://kiseki.fandom.com/wiki/Perzel_Farm
    // https://kiseki.fandom.com/wiki/Mercia_Orphanage
    // https://kiseki.fandom.com/wiki/Category:Trails_in_the_Sky_FC_NPCs
    class TrailKid : public WarGrey::STEM::Citizen {
    public:
        static std::vector<std::string> list_names();

    public:
        TrailKid(const std::string& name) : TrailKid(name.c_str()) {}
        TrailKid(const char* name);
        virtual ~TrailKid() {}
    };

    // https://kiseki.fandom.com/wiki/Jenis_Royal_Academy
    class TrailStudent : public WarGrey::STEM::Citizen {
    public:
        static std::vector<std::string> list_names();

    public:
        TrailStudent(const std::string& name) : TrailStudent(name.c_str()) {}
        TrailStudent(const char* name);
        virtual ~TrailStudent() {}
    };
}
