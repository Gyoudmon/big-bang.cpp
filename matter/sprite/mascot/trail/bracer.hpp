#pragma once

#include "citizen.hpp"

namespace GYDM {
    enum class BracerMode { Walk, Run, Win, Lose };

    class __lambda__ Bracer : public GYDM::Citizen {
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
    class __lambda__ Estelle : public GYDM::Bracer {
    public:
        Estelle(const char* nickname = nullptr) : Bracer("Estelle", nickname) {}
        virtual ~Estelle() {}

    public:
        const char* name() override { return "Estelle Bright"; }
        GYDM::CreatureGender gender() override { return CreatureGender::Female; }

    protected:
        void feed_canvas_size(BracerMode mode, float* width, float* height) override;
    };

    class __lambda__ Joshua : public GYDM::Bracer {
    public:
        Joshua(const char* nickname = nullptr) : Bracer("Joshua", nickname) {}
        virtual ~Joshua() {}

    public:
        const char* name() override { return "Joshua Bright"; }
        GYDM::CreatureGender gender() override { return CreatureGender::Male; }
    };

    class __lambda__ Scherazard : public GYDM::Bracer {
    public:
        Scherazard(const char* nickname = nullptr) : Bracer("Scherazard", nickname) {}
        const char* name() override { return "Scherazard Harvey"; }
        virtual ~Scherazard() {}

    public:
        GYDM::CreatureGender gender() override { return CreatureGender::Female; }
    };

    class __lambda__ Olivier : public GYDM::Bracer {
    public:
        Olivier(const char* nickname = nullptr) : Bracer("Olivier", nickname) {}
        virtual ~Olivier() {}

    public:
        const char* name() override { return "Olivier Lenheim"; }
        GYDM::CreatureGender gender() override { return CreatureGender::Male; }
    };

    class __lambda__ Klose : public GYDM::Bracer {
    public:
        Klose(const char* nickname = nullptr) : Bracer("Klose", nickname) {}
        virtual ~Klose() {}

    public:
        const char* name() override { return "Klose Rinz"; /* Klaudia von Auslese */ }
        GYDM::CreatureGender gender() override { return CreatureGender::Female; }
    };

    class __lambda__ Agate : public GYDM::Bracer {
    public:
        Agate(const char* nickname = nullptr) : Bracer("Agate", nickname) {}
        virtual ~Agate() {}

    public:
        const char* name() override { return "Agate Crosner"; }
        GYDM::CreatureGender gender() override { return CreatureGender::Male; }
    };

    class __lambda__ Tita : public GYDM::Bracer {
    public:
        Tita(const char* nickname = nullptr) : Bracer("Tita", nickname) {}
        virtual ~Tita() {}

    public:
        const char* name() override { return "Tita Russell"; }
        GYDM::CreatureGender gender() override { return CreatureGender::Female; }

    protected:
        void feed_canvas_size(BracerMode mode, float* width, float* height) override;
    };

    class __lambda__ Zin : public GYDM::Bracer {
    public:
        Zin(const char* nickname = nullptr) : Bracer("Zin", nickname) {}
        virtual ~Zin() {}

    public:
        const char* name() override { return "Zin Vathek"; }
        GYDM::CreatureGender gender() override { return CreatureGender::Male; }

    protected:
        void feed_canvas_size(BracerMode mode, float* width, float* height) override;
    };
}
