#pragma once

#include "../../folder.hpp"
#include "../../../../datum/path.hpp"

#include "../../../../physics/motion/map2d.hpp"

/* Protagonists in the RPG 空の軌跡 */

namespace Plteen {
    #define TRAIL_ANIMALS_PATH "trail/Animals"

    class __lambda__ Animal : public Plteen::Sprite, protected Plteen::I8WayMotion {
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
    class __lambda__ Cat : public Plteen::Animal {
    public:
        Cat();
        virtual ~Cat() {}

    public:
        Animal* asexually_reproduce() override { return new Cat(); }
    };

    class __lambda__ Cow : public Plteen::Animal {
    public:
        Cow();
        virtual ~Cow() {}

    public:
        Animal* asexually_reproduce() override { return new Cow(); }
    };

    class __lambda__ Rooster : public Plteen::Animal {
    public:
        Rooster();
        virtual ~Rooster() {}

    public:
        Animal* asexually_reproduce() override { return new Rooster(); }
    };

    class __lambda__ Pigeon : public Plteen::Animal {
    public:
        Pigeon();
        virtual ~Pigeon() {}

    public:
        Animal* asexually_reproduce() override { return new Pigeon(); }
    };
}
