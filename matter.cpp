#include "matter.hpp"
#include "plane.hpp"

#include "datum/box.hpp"
#include "graphics/image.hpp"
#include "physics/geometry/vector.hpp"

#include <typeinfo>

using namespace GYDM;

/**************************************************************************************************/
GYDM::IMatter::~IMatter() {
    if (this->info != nullptr) {
        delete this->info;
        this->info = nullptr;
    }

    if (this->_metatdata != nullptr) {
        delete this->_metatdata;
        this->_metatdata = nullptr;
    }
}

IPlane* GYDM::IMatter::master() const {
    IPlane* plane = nullptr;

    if (this->info != nullptr) {
        plane = this->info->master;
    }

    return plane;
}

SDL_Renderer* GYDM::IMatter::master_renderer() const {
    IPlane* master = this->master();
    SDL_Renderer* renderer = nullptr;

    if (master != nullptr) {
        renderer = master->master_renderer();
    }

    return renderer;
}

void GYDM::IMatter::attach_metadata(IMatterMetadata* metadata) {
    if (this->_metatdata != nullptr) {
        delete this->_metatdata;
    }

    this->_metatdata = metadata;
}

bool GYDM::IMatter::is_colliding(const Dot& local_pt) {
    Box box = this->get_bounding_box();
    Margin margin = this->get_margin();
    bool okay = true;

    if ((local_pt.x < margin.left) || (local_pt.y < margin.top)
        || (local_pt.x > (box.width() - margin.right))
        || (local_pt.y > (box.height() - margin.bottom))) {
        okay = false;
    }

    return okay;
}

void GYDM::IMatter::scale(float x_ratio, float y_ratio, const Anchor& anchor) {
    if (this->can_resize) {
        if ((x_ratio != 1.0F) || (y_ratio != 1.0F)) {
            Box box = this->get_bounding_box();

	        this->moor(anchor);
            this->on_resize(box.width() * x_ratio, box.height() * y_ratio, box.width(), box.height());
	        this->notify_updated();
        }
    }
}

void GYDM::IMatter::scale_to(float x_ratio, float y_ratio, const Anchor& anchor) {
    if (this->can_resize) {
        float nwidth, nheight;
        Box cbox = this->get_bounding_box();
        Box obox = this->get_original_bounding_box();

        nwidth = obox.width() * x_ratio;
        nheight = obox.height() * y_ratio;

        if ((nwidth != cbox.width()) || (nheight != cbox.height())) {
	        this->moor(anchor);
            this->on_resize(nwidth, nheight, cbox.width(), cbox.height());
	        this->notify_updated();
        }
    }
}

void GYDM::IMatter::scale_by_size(float size, bool given_width, const Anchor& anchor) {
    if (this->can_resize) {
        float nwidth, nheight;
        Box box = this->get_bounding_box();
        
        if (given_width) {
            nwidth = size;
            nheight = box.height() * (size / box.width());
        } else {
            nheight = size;
            nwidth = box.width() * (size / box.height());
        }
        
        if ((nwidth != box.width()) || (nheight != box.height())) {
            this->moor(anchor);
            this->on_resize(nwidth, nheight, box.width(), box.height());
	        this->notify_updated();
        }
    }
}

void GYDM::IMatter::resize(float nwidth, float nheight, const Anchor& anchor) {
    if (this->can_resize) {
        Box box = this->get_bounding_box();

        if (nwidth == 0.0F) {
            nwidth = box.width();
        }

        if (nheight == 0.0F) {
            nheight = box.height();
        }

	    if ((box.width() != nwidth) || (box.height() != nheight)) {
            this->moor(anchor);
            this->on_resize(nwidth, nheight, box.width(), box.height());
	        this->notify_updated();
	    }
    }
}

void GYDM::IMatter::notify_updated() {
    if (this->info != nullptr) {
        if (!this->anchor.is_zero()) {
            Dot dot = this->info->master->get_matter_location(this, this->anchor);

            /** NOTE
             * Gliding dramatically increasing the complexity of moving as glidings might be queued,
             *   the anchored moving here therefore uses relative target position,
             *   and do the moving immediately.
             **/

            if (dot != this->anchor_dot) {
                this->info->master->move(this, Vector(this->anchor_dot.x - dot.x, this->anchor_dot.y - dot.y), true);
            }

            this->clear_moor();
        }

        this->info->master->notify_updated(this);
    }
}

void GYDM::IMatter::notify_timeline_restart(uint32_t count0, int duration) {
    if (this->info != nullptr) {
        this->info->master->notify_matter_timeline_restart(this, count0, duration);
    }
}

void GYDM::IMatter::moor(const Anchor& anchor) {
    if (this->anchor != anchor) {
        if (this->info != nullptr) {
            this->anchor = anchor;
            this->anchor_dot = this->info->master->get_matter_location(this, anchor);
        }
    }
}

void GYDM::IMatter::clear_moor() {
    this->anchor.reset();
    // this->anchor_dot *= 0.0F;
}

bool GYDM::IMatter::has_caret() {
    bool careted = false;

    if (this->info != nullptr) {
        careted = (this->info->master->get_focused_matter() == this);
    }

    return careted;
}

void GYDM::IMatter::show(bool yes_no) {
    if (this->invisible == yes_no) {
        this->invisible = !yes_no;
        this->notify_updated();
    }
}

Dot GYDM::IMatter::get_location(const Anchor& a) {
    Dot dot(flnan_f, flnan_f);

    if (this->info != nullptr) {
        dot = this->info->master->get_matter_location(this, a);
    }

    return dot;
}

void GYDM::IMatter::log_message(Log level, const std::string& msg) {
    if (this->info != nullptr) {
        this->info->master->log_message(level, msg);
    }
}

const char* GYDM::IMatter::name() {
    return typeid(this).name();
}
