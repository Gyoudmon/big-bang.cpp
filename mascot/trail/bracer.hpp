#pragma once

#include "citizen.hpp"

namespace WarGrey::STEM {
    enum class BracerMode { Walk, Run, Win };

    class Bracer : public WarGrey::STEM::Citizen {
    public:
        Bracer(const char* name);
        virtual ~Bracer() {}

    public:
        void switch_mode(BracerMode mode, int repeat = -1, MatterAnchor anchor = MatterAnchor::CC);
        BracerMode current_mode() { return this->mode; }

    protected:
        void on_costumes_load() override;
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
        virtual void on_walk_mode(int repeat);
        virtual void on_run_mode(int repeat);
        virtual void on_win_mode(int repeat);

    protected:
        void retrigger_heading_change_event();

    private:
        BracerMode mode;
    };


    /*********************************************************************************************/
    class Estelle : public WarGrey::STEM::Bracer {
    public:
        Estelle() : Bracer("Estelle") {}
        const char* name() override { return "Estelle Bright"; }
        virtual ~Estelle() {}

    protected:
        void on_win_mode(int repeat) override;
    };

    class Joshua : public WarGrey::STEM::Bracer {
    public:
        Joshua() : Bracer("Joshua") {}
        const char* name() override { return "Joshua Bright"; }
        virtual ~Joshua() {}
    };

    class Klose : public WarGrey::STEM::Bracer {
    public:
        Klose() : Bracer("Klose") {}
        const char* name() override { return "Klose Rinz"; /* Klaudia von Auslese */ }
        virtual ~Klose() {}
    };

    class Agate : public WarGrey::STEM::Bracer {
    public:
        Agate() : Bracer("Agate") {}
        const char* name() override { return "Agate Crosner"; }
        virtual ~Agate() {}
    };
}
