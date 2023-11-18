#pragma once

#include "citizen.hpp"

namespace WarGrey::STEM {
    enum class BracerMode { Walk, Run, Win, Lose };

    class __lambda__ Bracer : public WarGrey::STEM::Citizen {
    public:
        Bracer(const char* name, const char* nickname = nullptr);
        virtual ~Bracer() {}

    public:
        void try_switch_mode(BracerMode mode, int repeat = -1, MatterAnchor anchor = MatterAnchor::CC);
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
    class __lambda__ Estelle : public WarGrey::STEM::Bracer {
    public:
        Estelle(const char* nickname = nullptr) : Bracer("Estelle", nickname) {}
        virtual ~Estelle() {}

    public:
        const char* name() override { return "Estelle Bright"; }
        WarGrey::STEM::CreatureGender gender() override { return CreatureGender::Female; }

    protected:
        void feed_canvas_size(BracerMode mode, float* width, float* height) override;
    };

    class __lambda__ Joshua : public WarGrey::STEM::Bracer {
    public:
        Joshua(const char* nickname = nullptr) : Bracer("Joshua", nickname) {}
        virtual ~Joshua() {}

    public:
        const char* name() override { return "Joshua Bright"; }
        WarGrey::STEM::CreatureGender gender() override { return CreatureGender::Male; }
    };

    class __lambda__ Scherazard : public WarGrey::STEM::Bracer {
    public:
        Scherazard(const char* nickname = nullptr) : Bracer("Scherazard", nickname) {}
        const char* name() override { return "Scherazard Harvey"; }
        virtual ~Scherazard() {}

    public:
        WarGrey::STEM::CreatureGender gender() override { return CreatureGender::Female; }
    };

    class __lambda__ Olivier : public WarGrey::STEM::Bracer {
    public:
        Olivier(const char* nickname = nullptr) : Bracer("Olivier", nickname) {}
        virtual ~Olivier() {}

    public:
        const char* name() override { return "Olivier Lenheim"; }
        WarGrey::STEM::CreatureGender gender() override { return CreatureGender::Male; }
    };

    class __lambda__ Klose : public WarGrey::STEM::Bracer {
    public:
        Klose(const char* nickname = nullptr) : Bracer("Klose", nickname) {}
        virtual ~Klose() {}

    public:
        const char* name() override { return "Klose Rinz"; /* Klaudia von Auslese */ }
        WarGrey::STEM::CreatureGender gender() override { return CreatureGender::Female; }
    };

    class __lambda__ Agate : public WarGrey::STEM::Bracer {
    public:
        Agate(const char* nickname = nullptr) : Bracer("Agate", nickname) {}
        virtual ~Agate() {}

    public:
        const char* name() override { return "Agate Crosner"; }
        WarGrey::STEM::CreatureGender gender() override { return CreatureGender::Male; }
    };

    class __lambda__ Tita : public WarGrey::STEM::Bracer {
    public:
        Tita(const char* nickname = nullptr) : Bracer("Tita", nickname) {}
        virtual ~Tita() {}

    public:
        const char* name() override { return "Tita Russell"; }
        WarGrey::STEM::CreatureGender gender() override { return CreatureGender::Female; }

    protected:
        void feed_canvas_size(BracerMode mode, float* width, float* height) override;
    };

    class __lambda__ Zin : public WarGrey::STEM::Bracer {
    public:
        Zin(const char* nickname = nullptr) : Bracer("Zin", nickname) {}
        virtual ~Zin() {}

    public:
        const char* name() override { return "Zin Vathek"; }
        WarGrey::STEM::CreatureGender gender() override { return CreatureGender::Male; }

    protected:
        void feed_canvas_size(BracerMode mode, float* width, float* height) override;
    };
}
