#include "matter.hpp"
#include "plane.hpp"

#include "datum/box.hpp"
#include "graphics/image.hpp"

#include <typeinfo>

using namespace WarGrey::STEM;

/**************************************************************************************************/
WarGrey::STEM::IMatter::~IMatter() {
    if (this->info != nullptr) {
        delete this->info;
        this->info = nullptr;
    }

    if (this->_metatdata != nullptr) {
        delete this->_metatdata;
        this->_metatdata = nullptr;
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

void WarGrey::STEM::IMatter::attach_metadata(IMatterMetadata* metadata) {
    if (this->_metatdata != nullptr) {
        delete this->_metatdata;
    }

    this->_metatdata = metadata;
}

void WarGrey::STEM::IMatter::feed_extent(float x, float y, float* w, float* h) {
    SET_VALUES(w, 0.0F, h, 0.0F);
}

void WarGrey::STEM::IMatter::feed_original_margin(float x, float y, float* top, float* right, float* bottom, float* left) {
    SET_VALUES(top, 0.0F, bottom, 0.0F);
    SET_VALUES(left, 0.0F, right, 0.0F);
}

bool WarGrey::STEM::IMatter::is_colliding_with_mouse(float lx, float ly) {
    float t, b, l, r, w, h;
    bool okay = true;

    this->feed_margin(0.0F, 0.0F, &t, &r, &b, &l);

    if ((t > 0.0F) || (l > 0.0F) || (r > 0.0F) || (b > 0.0F)) {
        if ((lx < l) || (ly < t)) {
            okay = false;
        } else {
            this->feed_extent(0.0F, 0.0F, &w, &h);

            if ((lx > (w - r)) || (ly > (h - b))) {
                okay = false;
            }
        }
    }

    return okay;
}

void WarGrey::STEM::IMatter::scale(float x_ratio, float y_ratio, const Anchor& anchor) {
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

void WarGrey::STEM::IMatter::scale_to(float x_ratio, float y_ratio, const Anchor& anchor) {
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

void WarGrey::STEM::IMatter::scale_by_size(float size, bool given_width, const Anchor& anchor) {
    if (this->can_resize) {
        float width, height, nwidth, nheight;
        float x = 0.0F;
        float y = 0.0F;

        this->feed_location(&x, &y, MatterAnchor::LT);
        this->feed_extent(x, y, &width, &height);

        if (given_width) {
            nwidth = size;
            nheight = height * (size / width);
        } else {
            nheight = size;
            nwidth = width * (size / height);
        }
        
        if ((nwidth != width) || (nheight != height)) {
            this->moor(anchor);
            this->on_resize(nwidth, nheight, width, height);
	        this->notify_updated();
        }
    }
}

void WarGrey::STEM::IMatter::resize(float nwidth, float nheight, const Anchor& anchor) {
    if (this->can_resize) {
        float width, height;
        float x = 0.0F;
        float y = 0.0F;

        this->feed_location(&x, &y, MatterAnchor::LT);
        this->feed_extent(x, y, &width, &height);

        if (nwidth == 0.0F) {
            nwidth = width;
        }

        if (nheight == 0.0F) {
            nheight = height;
        }

	    if ((width != nwidth) || (height != nheight)) {
            this->moor(anchor);
            this->on_resize(nwidth, nheight, width, height);
	        this->notify_updated();
	    }
    }
}

void WarGrey::STEM::IMatter::notify_updated() {
    if (this->info != nullptr) {
        if (!this->anchor.is_zero()) {
            float cx, cy;
            
            this->info->master->feed_matter_location(this, &cx, &cy, this->anchor);

            /** NOTE
             * Gliding dramatically increasing the complexity of moving as glidings might be queued,
             *   the anchored moving here therefore uses relative target position,
             *   and do the moving immediately.
             **/

            if ((cx != this->anchor_x) || (cy != this->anchor_y)) {
                this->info->master->move(this, this->anchor_x - cx, this->anchor_y - cy, true);
            }

            this->clear_moor();
        }

        this->info->master->notify_updated(this);
    }
}

void WarGrey::STEM::IMatter::notify_timeline_restart(uint32_t count0, int duration) {
    if (this->info != nullptr) {
        this->info->master->notify_matter_timeline_restart(this, count0, duration);
    }
}

void WarGrey::STEM::IMatter::moor(const Anchor& anchor) {
    if (this->anchor != anchor) {
        if (this->info != nullptr) {
            this->anchor = anchor;
            this->info->master->feed_matter_location(this, &this->anchor_x, &this->anchor_y, anchor);
        }
    }
}

void WarGrey::STEM::IMatter::clear_moor() {
    this->anchor.reset();
    this->anchor_x = 0.0F;
    this->anchor_y = 0.0F;
}

bool WarGrey::STEM::IMatter::has_caret() {
    bool careted = false;

    if (this->info != nullptr) {
        careted = (this->info->master->get_focused_matter() == this);
    }

    return careted;
}

void WarGrey::STEM::IMatter::show(bool yes_no) {
    if (this->invisible == yes_no) {
        this->invisible = !yes_no;
        this->notify_updated();
    }
}

void WarGrey::STEM::IMatter::feed_location(float* x, float* y, const Anchor& a) {
    if (this->info != nullptr) {
        this->info->master->feed_matter_location(this, x, y, a);
    }
}

void WarGrey::STEM::IMatter::log_message(Log level, const std::string& msg) {
    if (this->info != nullptr) {
        this->info->master->log_message(level, msg);
    }
}

const char* WarGrey::STEM::IMatter::name() {
    return typeid(this).name();
}
