#pragma once

#include "../../folder.hpp"
#include "../../../../datum/path.hpp"

#include "../../../../physics/motion/map2d.hpp"

#include <vector>

/* Protagonists in the RPG 空の軌跡 */

namespace WarGrey::STEM {
    #define TRAIL_KIDS_PATH "trail/Kids"
    #define TRAIL_STUDENTS_PATH "trail/Students"
    #define TRAIL_SPECIALS_PATH "trail/Specials"

    class Citizen : public WarGrey::STEM::Sprite, protected WarGrey::STEM::I8WayMotion {
    public:
        static size_t special_name_count();
        static std::vector<std::string> list_special_names();
        static Citizen* create_special(const std::string& name) { return Citizen::create_special(name.c_str()); }
        static Citizen* create_special(const char* name) { return new Citizen(digimon_mascot_path(name, "", TRAIL_SPECIALS_PATH)); }

    public:
        Citizen(const std::string& fullpath);
        Citizen(const std::string& name, const char* rootdir) : Citizen(name.c_str(), rootdir) {}
        Citizen(const char* name, const char* rootdir);
        Citizen(size_t seq, const char* rootdir);
        virtual ~Citizen() {}

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

    protected:
        void on_costumes_load() override;
    };

    /*********************************************************************************************/
    // https://kiseki.fandom.com/wiki/Perzel_Farm
    // https://kiseki.fandom.com/wiki/Mercia_Orphanage
    // https://kiseki.fandom.com/wiki/Category:Trails_in_the_Sky_FC_NPCs
    class TrailKid : public WarGrey::STEM::Citizen {
    public:
        static size_t name_count();
        static std::vector<std::string> list_names();
        static TrailKid* randomly_create();

    public:
        TrailKid(const std::string& name) : TrailKid(name.c_str()) {}
        TrailKid(const char* name);
        TrailKid(size_t seq);
        virtual ~TrailKid() {}
    };

    // https://kiseki.fandom.com/wiki/Jenis_Royal_Academy
    class TrailStudent : public WarGrey::STEM::Citizen {
    public:
        static size_t name_count();
        static std::vector<std::string> list_names();
        static TrailStudent* randomly_create();

    public:
        TrailStudent(const std::string& name) : TrailStudent(name.c_str()) {}
        TrailStudent(const char* name);
        TrailStudent(size_t seq);
        virtual ~TrailStudent() {}
    };
}
