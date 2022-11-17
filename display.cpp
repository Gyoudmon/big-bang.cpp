#include "display.hpp"
#include "image.hpp"

#include "datum/string.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
bool WarGrey::STEM::IDisplay::save_snapshot(const std::string& path) {
    return this->save_snapshot(path.c_str());
}

bool WarGrey::STEM::IDisplay::save_snapshot(const char* pname) {
    SDL_Surface* snapshot_png = this->snapshot();
    bool okay = game_save_image(snapshot_png, pname);

    SDL_FreeSurface(snapshot_png);

    return okay;
}

/*************************************************************************************************/
void WarGrey::STEM::IDisplay::notify_updated() {
    if (this->in_update_sequence()) {
        this->update_is_needed = true;
    } else {
        this->refresh();
        this->update_is_needed = false;
    }
}

void WarGrey::STEM::IDisplay::end_update_sequence() {
    this->update_sequence_depth -= 1;

    if (this->update_sequence_depth < 1) {
        this->update_sequence_depth = 0;

        if (this->update_is_needed) {
            this->refresh();
            this->update_is_needed = false;
        }
    }

}

/*************************************************************************************************/
void WarGrey::STEM::IDisplay::log_message(const char* fmt, ...) {
    VSNPRINT(text, fmt);
    this->log_message(text);
}

void WarGrey::STEM::IDisplay::log_message(const std::string& msg) {
    this->log_message(-1, msg);
}

void WarGrey::STEM::IDisplay::log_message(int fgc, const char* fmt, ...) {
    VSNPRINT(text, fmt);
    this->log_message(fgc, text);
}

