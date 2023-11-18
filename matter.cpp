#include "matter.hpp"
#include "plane.hpp"

#include "datum/string.hpp"
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

void WarGrey::STEM::IMatter::scale(float x_ratio, float y_ratio, float afx, float afy) {
    if (this->can_resize) {
        if ((x_ratio != 1.0F) || (y_ratio != 1.0F)) {
            float width, height;
            float x = 0.0F;
            float y = 0.0F;

            this->feed_location(&x, &y, 0.0F, 0.0F);
            this->feed_extent(x, y, &width, &height);

	        this->moor(afx, afy);
            this->on_resize(width * x_ratio, height * y_ratio, width, height);
	        this->notify_updated();
        }
    }
}

void WarGrey::STEM::IMatter::scale_to(float x_ratio, float y_ratio, float afx, float afy) {
    if (this->can_resize) {
        float cwidth, cheight, owidth, oheight, nwidth, nheight;
        float x = 0.0F;
        float y = 0.0F;

        this->feed_location(&x, &y, 0.0F, 0.0F);
        this->feed_extent(x, y, &cwidth, &cheight);
        this->feed_original_extent(x, y, &owidth, &oheight);

        nwidth = owidth * x_ratio;
        nheight = oheight * y_ratio;

        if ((nwidth != cwidth) || (nheight != cheight)) {
	        this->moor(afx, afy);
            this->on_resize(nwidth, nheight, cwidth, cheight);
	        this->notify_updated();
        }
    }
}

void WarGrey::STEM::IMatter::scale_by_size(float size, bool given_width, float afx, float afy) {
    if (this->can_resize) {
        float width, height, nwidth, nheight;
        float x = 0.0F;
        float y = 0.0F;

        this->feed_location(&x, &y, 0.0F, 0.0F);
        this->feed_extent(x, y, &width, &height);

        if (given_width) {
            nwidth = size;
            nheight = height * (size / width);
        } else {
            nheight = size;
            nwidth = width * (size / height);
        }
        
        if ((nwidth != width) || (nheight != height)) {
            this->moor(afx, afy);
            this->on_resize(nwidth, nheight, width, height);
	        this->notify_updated();
        }
    }
}

void WarGrey::STEM::IMatter::resize(float nwidth, float nheight, float afx, float afy) {
    if (this->can_resize) {
        float width, height;
        float x = 0.0F;
        float y = 0.0F;

        this->feed_location(&x, &y, 0.0F, 0.0F);
        this->feed_extent(x, y, &width, &height);

        if (nwidth == 0.0F) {
            nwidth = width;
        }

        if (nheight == 0.0F) {
            nheight = height;
        }

	    if ((width != nwidth) || (height != nheight)) {
            this->moor(afx, afy);
            this->on_resize(nwidth, nheight, width, height);
	        this->notify_updated();
	    }
    }
}

void WarGrey::STEM::IMatter::notify_updated() {
    if (this->info != nullptr) {
        if ((this->anchor_fx != 0.0F) || (this->anchor_fy != 0.0F)) {
            float cx, cy;
            
            this->info->master->feed_matter_location(this, &cx, &cy, this->anchor_fx, this->anchor_fy);

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

void WarGrey::STEM::IMatter::moor(float afx, float afy) {
    if ((afx != 0.0F) || (afy != 0.0F)) {
        if (this->info != nullptr) {
            this->anchor_fx = afx;
            this->anchor_fy = afy;
            this->info->master->feed_matter_location(this, &this->anchor_x, &this->anchor_y, afx, afy);
        }
    }
}

void WarGrey::STEM::IMatter::clear_moor() {
    this->anchor_fx = 0.0F;
    this->anchor_fy = 0.0F;
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

void WarGrey::STEM::IMatter::feed_location(float* x, float* y, MatterAnchor a) {
    if (this->info != nullptr) {
        this->info->master->feed_matter_location(this, x, y, a);
    }
}

void WarGrey::STEM::IMatter::feed_location(float* x, float* y, float fx, float fy) {
    if (this->info != nullptr) {
        this->info->master->feed_matter_location(this, x, y, fx, fy);
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

const char* WarGrey::STEM::IMatter::name() {
    return typeid(this).name();
}

/**************************************************************************************************/
void WarGrey::STEM::IMatter::moor(MatterAnchor anchor) {
    if (anchor != MatterAnchor::LT) {
        if (this->info != nullptr) {
            float afx, afy;

            matter_anchor_fraction(anchor, &afx, &afy);
            this->moor(afx, afy);
        }
    }
}

void WarGrey::STEM::IMatter::scale(float x_ratio, float y_ratio, MatterAnchor anchor) {
    if (this->can_resize) {
        float afx, afy;

        matter_anchor_fraction(anchor, &afx, &afy);
        this->scale(x_ratio, y_ratio, afx, afy);
    }
}

void WarGrey::STEM::IMatter::scale_to(float x_ratio, float y_ratio, MatterAnchor anchor) {
    if (this->can_resize) {
        float afx, afy;

        matter_anchor_fraction(anchor, &afx, &afy);
        this->scale_to(x_ratio, y_ratio, afx, afy);
    }
}

void WarGrey::STEM::IMatter::resize(float nwidth, float nheight, MatterAnchor anchor) {
    if (this->can_resize) {
        float afx, afy;

        matter_anchor_fraction(anchor, &afx, &afy);
        this->resize(nwidth, nheight, afx, afy);
    }
}

void WarGrey::STEM::IMatter::resize_by_width(float size, MatterAnchor anchor) {
    if (this->can_resize) {
        float afx, afy;

        matter_anchor_fraction(anchor, &afx, &afy);
        this->resize_by_width(size, afx, afy);
    }
}

void WarGrey::STEM::IMatter::resize_by_height(float size, MatterAnchor anchor) {
    if (this->can_resize) {
        float afx, afy;

        matter_anchor_fraction(anchor, &afx, &afy);
        this->resize_by_height(size, afx, afy);
    }
}
