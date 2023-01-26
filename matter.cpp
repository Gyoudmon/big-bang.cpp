#include "matter.hpp"
#include "plane.hpp"

#include "datum/string.hpp"
#include "datum/box.hpp"

#include "graphics/image.hpp"

using namespace WarGrey::STEM;

/**************************************************************************************************/
WarGrey::STEM::IMatter::~IMatter() {
    if (this->info != nullptr) {
        delete this->info;
        this->info = nullptr;
    }
}

IPlane* WarGrey::STEM::IMatter::master() {
    IPlane* plane = nullptr;

    if (this->info != nullptr) {
        plane = this->info->master;
    }

    return plane;
}

SDL_Renderer* WarGrey::STEM::IMatter::master_renderer() {
    IPlane* master = this->master();
    SDL_Renderer* renderer = nullptr;

    if (master != nullptr) {
        renderer = master->master_renderer();
    }

    return renderer;
}

void WarGrey::STEM::IMatter::feed_extent(float x, float y, float* w, float* h) {
    SET_VALUES(w, 0.0F, h, 0.0F);
}

void WarGrey::STEM::IMatter::feed_margin(float x, float y, float* top, float* right, float* bottom, float* left) {
    SET_VALUES(top, 0.0F, bottom, 0.0F);
    SET_VALUES(left, 0.0F, right, 0.0F);
}

void WarGrey::STEM::IMatter::scale(float x_ratio, float y_ratio, MatterAnchor anchor) {
    if (this->can_resize) {
        if ((x_ratio != 1.0F) || (y_ratio != 1.0F)) {
            float width, height;
            float x = 0.0F;
            float y = 0.0F;

            this->feed_location(&x, &y, MatterAnchor::LT);
            this->feed_extent(x, y, &width, &height);

	        this->moor(anchor);
            this->on_resize(width * x_ratio, height * y_ratio, width, height);
	        this->notify_updated();
        }
    }
}

void WarGrey::STEM::IMatter::scale_to(float x_ratio, float y_ratio, MatterAnchor anchor) {
    if (this->can_resize) {
        float cwidth, cheight, owidth, oheight, nwidth, nheight;
        float x = 0.0F;
        float y = 0.0F;

        this->feed_location(&x, &y, MatterAnchor::LT);
        this->feed_extent(x, y, &cwidth, &cheight);
        this->feed_original_extent(x, y, &owidth, &oheight);

        nwidth = owidth * x_ratio;
        nheight = oheight * y_ratio;

        if ((nwidth != cwidth) || (nheight != cheight)) {
	        this->moor(anchor);
            this->on_resize(nwidth, nheight, cwidth, cheight);
	        this->notify_updated();
        }
    }
}

void WarGrey::STEM::IMatter::resize(float w, float h, MatterAnchor anchor) {
    if (this->can_resize) {
        if ((w > 0.0F) && (h > 0.0F)) {
            float width, height;
            float x = 0.0F;
            float y = 0.0F;

            this->feed_location(&x, &y, MatterAnchor::LT);
            this->feed_extent(x, y, &width, &height);

	        if ((width != w) || (height != h)) {
                this->moor(anchor);
                this->on_resize(w, h, width, height);
	            this->notify_updated();
	        }
        }
    }
}

void WarGrey::STEM::IMatter::notify_updated() {
    if (this->info != nullptr) {
        if (this->anchor != MatterAnchor::LT) {
            this->info->master->move_to(this, this->anchor_x, this->anchor_y, this->anchor);
            this->clear_moor();
        }

        this->info->master->notify_updated(this);
    }
}

void WarGrey::STEM::IMatter::notify_timeline_restart(uint32_t count0) {
    if (this->info != nullptr) {
        this->info->master->notify_matter_timeline_restart(this, count0);
    }
}

void WarGrey::STEM::IMatter::moor(MatterAnchor anchor) {
    if (anchor != MatterAnchor::LT) {
        if (this->info != nullptr) {
            this->anchor = anchor;
            this->info->master->feed_matter_location(this, &this->anchor_x, &this->anchor_y, anchor);
        }
    }
}

void WarGrey::STEM::IMatter::clear_moor() {
    this->anchor = MatterAnchor::LT;
}

bool WarGrey::STEM::IMatter::has_caret() {
    bool careted = false;

    if (this->info != nullptr) {
        careted = (this->info->master->get_focused_matter() == this);
    }

    return careted;
}

void WarGrey::STEM::IMatter::feed_location(float* x, float* y, WarGrey::STEM::MatterAnchor a) {
    if (this->info != nullptr) {
        this->info->master->feed_matter_location(this, x, y, a);
    }
}

void WarGrey::STEM::IMatter::log_message(const char* fmt, ...) {
    if (this->info != nullptr) {
        VSNPRINT(text, fmt);
        this->log_message(-1, text);
    }
}

void WarGrey::STEM::IMatter::log_message(int fgc, const char* fmt, ...) {
    if (this->info != nullptr) {
        VSNPRINT(text, fmt);
        this->log_message(fgc, text);
    }
}

void WarGrey::STEM::IMatter::log_message(int fgc, const std::string& msg) {
    if (this->info != nullptr) {
        this->info->master->log_message(fgc, msg);
    }
}
