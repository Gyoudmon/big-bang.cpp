#include "display.hpp"
#include "image.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
bool WarGrey::STEM::IDisplay::save_snapshot(const std::string& path) {
    return this->save_snapshot(path.c_str());
}

bool WarGrey::STEM::IDisplay::save_snapshot(const char* pname) {
    return game_save_image(this->snapshot(), pname);
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

