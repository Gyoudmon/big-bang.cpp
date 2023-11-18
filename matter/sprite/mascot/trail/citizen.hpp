#pragma once

#include "../../folder.hpp"
#include "../../../../datum/path.hpp"

#include "../../../../physics/motion/map2d.hpp"

#include "../creature.hpp"

#include <vector>

/* Protagonists in the RPG 空の軌跡 */

namespace WarGrey::STEM {
    #define TRAIL_KIDS_PATH "trail/Kids"
    #define TRAIL_STUDENTS_PATH "trail/Students"
    #define TRAIL_SPECIALS_PATH "trail/Specials"

    class __lambda__ Citizen : public WarGrey::STEM::Sprite, public WarGrey::STEM::ICreature, protected WarGrey::STEM::I8WayMotion {
    public:
        static size_t special_name_count();
        static std::vector<std::string> list_special_names();
        
        static Citizen* create_special(const std::string& name, const char* nickname = nullptr) {
            return Citizen::create_special(name.c_str(), nickname);
        }

        static Citizen* create_special(const char* name, const char* nickname = nullptr) {
            return new Citizen(digimon_mascot_path(name, "", TRAIL_SPECIALS_PATH), nickname);
        }

    public:
        Citizen(const std::string& fullpath, const char* nickname = nullptr);
        Citizen(const std::string& name, const char* rootdir, const char* nickname) : Citizen(name.c_str(), rootdir, nickname) {}
        Citizen(const char* name, const char* rootdir, const char* nickname);
        Citizen(size_t seq, const char* rootdir, const char* nickname = nullptr);
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
    class __lambda__ TrailKid : public WarGrey::STEM::Citizen {
    public:
        static size_t name_count();
        static std::vector<std::string> list_names();
        static TrailKid* randomly_create();

    public:
        TrailKid(const std::string& name, const char* nickname = nullptr) : TrailKid(name.c_str(), nickname) {}
        TrailKid(const char* name, const char* nickname = nullptr);
        TrailKid(size_t seq, const char* nickname = nullptr);
        virtual ~TrailKid() {}
    };

    // https://kiseki.fandom.com/wiki/Jenis_Royal_Academy
    class __lambda__ TrailStudent : public WarGrey::STEM::Citizen {
    public:
        static size_t name_count();
        static std::vector<std::string> list_names();
        static TrailStudent* randomly_create();

    public:
        TrailStudent(const std::string& name, const char* nickname = nullptr) : TrailStudent(name.c_str(), nickname) {}
        TrailStudent(const char* name, const char* nickname = nullptr);
        TrailStudent(size_t seq, const char* nickname = nullptr);
        virtual ~TrailStudent() {}
    };
}
