#include "ulpc.hpp"

#include "../../../../physics/mathematics.hpp"
#include "../../../../physics/random.hpp"

using namespace GYDM;

/*************************************************************************************************/
static const char* ulpc_moves [] /* [13] */ = {
    "uspellcast-1", "uspellcast-2", "uspellcast-3", "uspellcast-4", "uspellcast-5", "uspellcast-6", "uspellcast-7", "", "", "", "", "", "",
    "lspellcast-1", "lspellcast-2", "lspellcast-3", "lspellcast-4", "lspellcast-5", "lspellcast-6", "lspellcast-7", "", "", "", "", "", "",
    "dspellcast-1", "dspellcast-2", "dspellcast-3", "dspellcast-4", "dspellcast-5", "dspellcast-6", "dspellcast-7", "", "", "", "", "", "",
    "rspellcast-1", "rspellcast-2", "rspellcast-3", "rspellcast-4", "rspellcast-5", "rspellcast-6", "rspellcast-7", "", "", "", "", "", "",

    "uthrust-1", "uthrust-2", "uthrust-3", "uthrust-4", "uthrust-5", "uthrust-6", "uthrust-7", "uthrust-8", "", "", "", "", "",
    "lthrust-1", "lthrust-2", "lthrust-3", "lthrust-4", "lthrust-5", "lthrust-6", "lthrust-7", "lthrust-8", "", "", "", "", "",
    "dthrust-1", "dthrust-2", "dthrust-3", "dthrust-4", "dthrust-5", "dthrust-6", "dthrust-7", "dthrust-8", "", "", "", "", "",
    "rthrust-1", "rthrust-2", "rthrust-3", "rthrust-4", "rthrust-5", "rthrust-6", "rthrust-7", "rthrust-8", "", "", "", "", "",

    "uwalk-1", "uwalk-2", "uwalk-3", "uwalk-4", "uwalk-5", "uwalk-6", "uwalk-7", "uwalk-8", "uwalk-9", "", "", "", "",
    "lwalk-1", "lwalk-2", "lwalk-3", "lwalk-4", "lwalk-5", "lwalk-6", "lwalk-7", "lwalk-8", "lwalk-9", "", "", "", "",
    "dwalk-1", "dwalk-2", "dwalk-3", "dwalk-4", "dwalk-5", "dwalk-6", "dwalk-7", "dwalk-8", "dwalk-9", "", "", "", "",
    "rwalk-1", "rwalk-2", "rwalk-3", "rwalk-4", "rwalk-5", "rwalk-6", "rwalk-7", "rwalk-8", "rwalk-9", "", "", "", "",
    
    "uslash-1", "uslash-2", "uslash-3", "uslash-4", "uslash-5", "uslash-6", "", "", "", "", "", "", "",
    "lslash-1", "lslash-2", "lslash-3", "lslash-4", "lslash-5", "lslash-6", "", "", "", "", "", "", "",
    "dslash-1", "dslash-2", "dslash-3", "dslash-4", "dslash-5", "dslash-6", "", "", "", "", "", "", "",
    "rslash-1", "rslash-2", "rslash-3", "rslash-4", "rslash-5", "rslash-6", "", "", "", "", "", "", "",
    
    "ushoot-1", "ushoot-2", "ushoot-3", "ushoot-4", "ushoot-5", "ushoot-6", "ushoot-7", "ushoot-8", "ushoot-9", "ushoot-10", "ushoot-11", "ushoot-12", "ushoot-13",
    "lshoot-1", "lshoot-2", "lshoot-3", "lshoot-4", "lshoot-5", "lshoot-6", "lshoot-7", "lshoot-8", "lshoot-9", "lshoot-10", "lshoot-11", "lshoot-12", "lshoot-13",
    "dshoot-1", "dshoot-2", "dshoot-3", "dshoot-4", "dshoot-5", "dshoot-6", "dshoot-7", "dshoot-8", "dshoot-9", "dshoot-10", "dshoot-11", "dshoot-12", "dshoot-13",
    "rshoot-1", "rshoot-2", "rshoot-3", "rshoot-4", "rshoot-5", "rshoot-6", "rshoot-7", "rshoot-8", "rshoot-9", "rshoot-10", "rshoot-11", "rshoot-12", "rshoot-13",

    "hurt-1", "hurt-2", "hurt-3", "hurt-4", "hurt-5", "hurt-6", "", "", "", "", "", "", ""
};

/*************************************************************************************************/
GYDM::SpriteULPCSheet::SpriteULPCSheet(const std::string& pathname) : SpriteGridSheet(pathname, 21, 13) {}

void GYDM::SpriteULPCSheet::on_heading_changed(double theta_rad, double vx, double vy, double prev_vr) {
    I4WayMotion::dispatch_heading_event(theta_rad, vx, vy, prev_vr);
}

void GYDM::SpriteULPCSheet::on_eward(double theta_rad, double vx, double vy) {
    this->play("rwalk");
}

void GYDM::SpriteULPCSheet::on_wward(double theta_rad, double vx, double vy) {
    this->play("lwalk");
}

void GYDM::SpriteULPCSheet::on_sward(double theta_rad, double vx, double vy) {
    this->play("dwalk");
}

void GYDM::SpriteULPCSheet::on_nward(double theta_rad, double vx, double vy) {
    this->play("uwalk");
}

const char* GYDM::SpriteULPCSheet::costume_index_to_name(size_t idx) {
    return ulpc_moves[idx];
}
