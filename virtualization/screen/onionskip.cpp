#include "onionskip.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
void WarGrey::STEM::OnionSkip::refresh() {
    this->display()->refresh();
}

void WarGrey::STEM::OnionSkip::begin_update_sequence() {
    this->update_sequence_depth += 1;
}

void WarGrey::STEM::OnionSkip::end_update_sequence() {
    this->update_sequence_depth -= 1;

    if (this->update_sequence_depth < 1) {
        this->update_sequence_depth = 0;

        if (this->update_is_needed) {
            this->refresh();
            this->update_is_needed = false;
        }
    }
}

void WarGrey::STEM::OnionSkip::notify_updated() {
    if (this->in_update_sequence()) {
        this->update_is_needed = true;
    } else {
        this->refresh();
        this->update_is_needed = false;
    }
}

