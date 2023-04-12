#pragma once

#include "citizen.hpp"

namespace WarGrey::STEM {
    enum class BracerMode { Walk, Run, Win, Lose };

    class Bracer : public WarGrey::STEM::Citizen {
    public:
        Bracer(const char* name);
        virtual ~Bracer() {}

    public:
        void switch_mode(BracerMode mode, int repeat = -1, MatterAnchor anchor = MatterAnchor::CC);
        BracerMode current_mode() { return this->mode; }

    protected:
        void on_costumes_load() override;
        void on_nward(double theta_rad, double vx, double vy) override;
        void on_eward(double theta_rad, double vx, double vy) override;
        void on_sward(double theta_rad, double vx, double vy) override;
        void on_wward(double theta_rad, double vx, double vy) override;
        void on_wnward(double theta_rad, double vx, double vy) override;
        void on_wsward(double theta_rad, double vx, double vy) override;
        void on_enward(double theta_rad, double vx, double vy) override;
        void on_esward(double theta_rad, double vx, double vy) override;

    protected:
        virtual void feed_canvas_size(BracerMode mode, float* width, float* height);
        virtual void on_walk_mode(int repeat);
        virtual void on_run_mode(int repeat);
        virtual void on_win_mode(int repeat);
        virtual void on_lose_mode(int repeat);

    protected:
        void retrigger_heading_change_event();

    private:
        void do_mode_switching(BracerMode mode, MatterAnchor anchor);

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
        void feed_canvas_size(BracerMode mode, float* width, float* height) override;
    };

    class Joshua : public WarGrey::STEM::Bracer {
    public:
        Joshua() : Bracer("Joshua") {}
        const char* name() override { return "Joshua Bright"; }
        virtual ~Joshua() {}
    };

    class Scherazard : public WarGrey::STEM::Bracer {
    public:
        Scherazard() : Bracer("Scherazard") {}
        const char* name() override { return "Scherazard Harvey"; }
        virtual ~Scherazard() {}
    };

    class Olivier : public WarGrey::STEM::Bracer {
    public:
        Olivier() : Bracer("Olivier") {}
        const char* name() override { return "Olivier Lenheim"; }
        virtual ~Olivier() {}
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

    class Tita : public WarGrey::STEM::Bracer {
    public:
        Tita() : Bracer("Tita") {}
        const char* name() override { return "Tita Russell"; }
        virtual ~Tita() {}

    protected:
        void feed_canvas_size(BracerMode mode, float* width, float* height) override;
    };

    class Zin : public WarGrey::STEM::Bracer {
    public:
        Zin() : Bracer("Zin") {}
        const char* name() override { return "Zin Vathek"; }
        virtual ~Zin() {}

    protected:
        void feed_canvas_size(BracerMode mode, float* width, float* height) override;
    };
}
