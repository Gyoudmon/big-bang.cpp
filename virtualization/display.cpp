#include "display.hpp"

#include "../graphics/image.hpp"
#include "../datum/string.hpp"

using namespace GYDM;

/*************************************************************************************************/
bool GYDM::IDisplay::save_snapshot(const std::string& path) {
    return this->save_snapshot(path.c_str());
}

bool GYDM::IDisplay::save_snapshot(const char* pname) {
    SDL_Surface* snapshot_png = this->snapshot();
    bool okay = game_save_image(snapshot_png, pname);

    SDL_FreeSurface(snapshot_png);

    return okay;
}

/*************************************************************************************************/
void GYDM::IDisplay::notify_updated() {
    if (this->is_in_update_sequence()) {
        this->update_is_needed = true;
    } else {
        this->refresh();
        this->update_is_needed = false;
    }
}

void GYDM::IDisplay::end_update_sequence() {
    this->update_sequence_depth -= 1;

    if (this->update_sequence_depth < 1) {
        this->update_sequence_depth = 0;

        if (this->should_update()) {
            this->refresh();
            this->update_is_needed = false;
        }
    }
}
