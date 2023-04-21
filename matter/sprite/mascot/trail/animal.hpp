#pragma once

#include "../../folder.hpp"
#include "../../../../datum/path.hpp"

#include "../../../../physics/motion/map2d.hpp"

/* Protagonists in the RPG 空の軌跡 */

namespace WarGrey::STEM {
    #define TRAIL_ANIMALS_PATH "trail/Animals"

    class Animal : public WarGrey::STEM::Sprite, protected WarGrey::STEM::I8WayMotion {
    public:
        Animal(const std::string& fullpath);
        virtual ~Animal() {}

    public:
        int preferred_local_fps() override { return 15; }

    public:
        virtual Animal* asexually_reproduce() = 0;

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
    class Cat : public WarGrey::STEM::Animal {
    public:
        Cat();
        virtual ~Cat() {}

    public:
        Animal* asexually_reproduce() override { return new Cat(); }
    };

    class Cow : public WarGrey::STEM::Animal {
    public:
        Cow();
        virtual ~Cow() {}

    public:
        Animal* asexually_reproduce() override { return new Cow(); }
    };

    class Rooster : public WarGrey::STEM::Animal {
    public:
        Rooster();
        virtual ~Rooster() {}

    public:
        Animal* asexually_reproduce() override { return new Rooster(); }
    };

    class Pigeon : public WarGrey::STEM::Animal {
    public:
        Pigeon();
        virtual ~Pigeon() {}

    public:
        Animal* asexually_reproduce() override { return new Pigeon(); }
    };
}
