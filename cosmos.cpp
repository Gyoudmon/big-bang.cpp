#include "cosmos.hpp"
#include "matter.hpp"

#include "virtualization/screen/onionskin.hpp"

using namespace GYDM;

/*************************************************************************************************/
#define PLANE_INFO(plane) (static_cast<LinkedPlaneInfo*>(plane->info))

namespace {
    struct LinkedPlaneInfo : public IPlaneInfo {
        LinkedPlaneInfo(IScreen* master) : IPlaneInfo(master) {};

        bool loaded = false;

        IPlane* next;
        IPlane* prev;
    };
}

/*************************************************************************************************/
static inline LinkedPlaneInfo* bind_plane_owership(IScreen* master, IPlane* plane) {
    auto info = new LinkedPlaneInfo(master);
    
    plane->info = info;

    return info;
}

static inline void reflow_plane(IPlane* plane, float width, float height) {
    plane->reflow(width, height);
}

static inline void construct_plane(IPlane* plane, float flwidth, float flheight, bool need_reflow) {
    plane->begin_update_sequence();

    plane->construct(flwidth, flheight);
    plane->load(flwidth, flheight);

    PLANE_INFO(plane)->loaded = true;

    if (need_reflow) {
        reflow_plane(plane, flwidth, flheight);
    }

    plane->end_update_sequence();
}

static inline void draw_plane(dc_t* dc, IPlane* plane, float x, float y, float width, float height) {
    plane->draw(dc, x, y, width, height);
}

/*************************************************************************************************/
GYDM::Cosmos::Cosmos(uint32_t fps, const RGBA& fgc, const RGBA& bgc) : IUniverse(fps, fgc, bgc) {
    this->screen = new OnionSkin(this);
}

GYDM::Cosmos::~Cosmos() {
    this->collapse();
    delete this->screen;
}

IPlane* GYDM::Cosmos::push_plane(IPlane* plane) {
    // NOTE: this method is designed to be invoked in `Cosmos::construct()`

    if (plane->info == nullptr) {
        LinkedPlaneInfo* info = bind_plane_owership(this->screen, plane);
        
        if (this->head_plane == nullptr) {
            this->head_plane = plane;
            this->recent_plane = plane;
            info->prev = this->head_plane;

            this->recent_plane_idx = 0;
            this->chunk_count = 1;
        } else { 
            LinkedPlaneInfo* head_info = PLANE_INFO(this->head_plane);
            LinkedPlaneInfo* prev_info = PLANE_INFO(head_info->prev);
             
            info->prev = head_info->prev;
            prev_info->next = plane;
            head_info->prev = plane;

            this->chunk_count ++;
        }

        info->next = this->head_plane;
    }

    return plane;
}

void GYDM::Cosmos::collapse() {
    if (this->head_plane != nullptr) {
        IPlane* temp_head = this->head_plane;
        LinkedPlaneInfo* temp_info = PLANE_INFO(temp_head);
        LinkedPlaneInfo* prev_info = PLANE_INFO(temp_info->prev);

        this->head_plane = nullptr;
        this->recent_plane = nullptr;
        prev_info->next = nullptr;

        do {
            IPlane* child = temp_head;

            temp_head = PLANE_INFO(temp_head)->next;

            delete child; // plane's destructor will delete the associated info object
        } while (temp_head != nullptr);
    }
}

bool GYDM::Cosmos::has_current_mission_completed() {
    return (this->recent_plane != nullptr) && this->recent_plane->has_mission_completed();
}

bool GYDM::Cosmos::can_exit() {
    return this->has_current_mission_completed()
            && (this->recent_plane == PLANE_INFO(this->recent_plane)->next);
}

/*************************************************************************************************/
void GYDM::Cosmos::on_big_bang(int width, int height) {
    if (this->recent_plane != nullptr) {
        construct_plane(this->recent_plane, float(width), float(height), false);
        this->set_window_title("%s", this->recent_plane->name());
    }
}

void GYDM::Cosmos::reflow(float width, float height) {
    if ((width > 0.0F) && (height > 0.0F)) {
        if (this->head_plane != nullptr) {
            IPlane* child = this->head_plane;

            do {
                LinkedPlaneInfo* info = PLANE_INFO(child);

                if (info->loaded) {
                    reflow_plane(child, width, height);
                }

                child = info->next;
            } while (child != this->head_plane);
        }
    }
}

void GYDM::Cosmos::on_game_start() {
    if ((this->recent_plane == this->head_plane) && (this->recent_plane != nullptr)) {
		this->notify_transfer(nullptr, this->recent_plane);
	}
}

void GYDM::Cosmos::on_elapse(uint64_t count, uint32_t interval, uint64_t uptime) {
    this->begin_update_sequence();

    /**
     * By designe, the `Plane`s are abstracted as world chunks,
     *   As such, no need to update other planes here.
     **/

    if (this->recent_plane != nullptr) {    
        this->recent_plane->begin_update_sequence();
        this->recent_plane->on_elapse(count, interval, uptime);
        this->recent_plane->end_update_sequence();
    }

    this->update(count, interval, uptime);

    this->end_update_sequence();
}

void GYDM::Cosmos::draw(dc_t* dc, int x, int y, int width, int height) {
    float flx = float(x);
    float fly = float(y);
    float flwidth = float(width);
    float flheight = float(height);

    // NOTE: only the current plane needs to be drawn

    if (this->recent_plane != nullptr) {
        draw_plane(dc, this->recent_plane, flx, fly, flwidth, flheight);
    }
}

/*************************************************************************************************/
void GYDM::Cosmos::on_mouse_event(SDL_MouseButtonEvent& m, bool pressed) {
    if (this->recent_plane != nullptr) {
        uint8_t button = m.button;
        float flx = float(m.x);
        float fly = float(m.y);
        uint8_t clicks = m.clicks;

        this->begin_update_sequence();
        if (pressed) {
            this->recent_plane->on_pointer_pressed(button, flx, fly, clicks);
        } else {
            this->recent_plane->on_pointer_released(button, flx, fly, clicks);
        }
        this->end_update_sequence();
    }
}

void GYDM::Cosmos::on_mouse_move(uint32_t state, int x, int y, int dx, int dy) {
    if (this->recent_plane != nullptr) {
        this->begin_update_sequence();
        this->recent_plane->on_pointer_move(state, float(x), float(y), float(dx), float(dy));
        this->end_update_sequence();
    }
}

void GYDM::Cosmos::on_scroll(int horizon, int vertical, float hprecise, float vprecise) {
    if (this->recent_plane != nullptr) {
        this->begin_update_sequence();
        this->recent_plane->on_scroll(horizon, vertical, hprecise, vprecise);
        this->end_update_sequence();
    }
}

void GYDM::Cosmos::on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) {
    if (this->recent_plane != nullptr) {
        this->begin_update_sequence();
        this->recent_plane->on_char(key, modifiers, repeats, pressed);
        this->end_update_sequence();
    }
}

void GYDM::Cosmos::on_text(const char* text, size_t size, bool entire) {
    if (this->recent_plane != nullptr) {
        this->begin_update_sequence();
        this->recent_plane->on_text(text, size, entire);
        this->end_update_sequence();
    }
}

void GYDM::Cosmos::on_editing_text(const char* text, int pos, int span) {
    if (this->recent_plane != nullptr) {
        this->begin_update_sequence();
        this->recent_plane->on_editing_text(text, pos, span);
        this->end_update_sequence();
    }
}
            
void GYDM::Cosmos::on_save(const std::string& full_path, std::ofstream& dev_datout) {
    if (this->recent_plane != nullptr) {
        this->recent_plane->on_save(full_path, dev_datout);
    }
}

const char* GYDM::Cosmos::usrdata_extension() {
    if (this->recent_plane != nullptr) {
        return this->recent_plane->usrdata_extension();
    } else {
        return nullptr;
    }
}

/*************************************************************************************************/
const char* GYDM::Cosmos::plane_name(int idx) {
    const char* pname = nullptr;

	if (this->head_plane != nullptr) {
		IPlane* child = this->head_plane;
		int current_idx = 0;

		do {
			if (current_idx == idx) {
				pname = child->name();
                break;
			}

			current_idx += 1;
			child = PLANE_INFO(child)->next;
		} while (child != this->head_plane);
	}

    return pname;
}

int GYDM::Cosmos::plane_index(const std::string& name) {
	int to_idx = -1;
	
	if ((this->head_plane != nullptr) && (!name.empty())) {
		IPlane* child = this->head_plane;
		int idx = 0;

		do {
			LinkedPlaneInfo* info = PLANE_INFO(child);

			if (name.compare(child->name())) {
				to_idx = idx;
				break;
			}

			idx += 1;
			child = info->next;
		} while (child != this->head_plane);
	}

	return to_idx;
}

void GYDM::Cosmos::transfer(int delta_idx) {
	if ((this->recent_plane != nullptr) && (delta_idx != 0)) {
		if (this->from_plane == nullptr) {
			this->from_plane = this->recent_plane;
		}

		if (delta_idx > 0) {
            delta_idx = delta_idx % this->chunk_count;
            this->recent_plane_idx = (this->recent_plane_idx + delta_idx) % this->chunk_count;

			for (int i = 0; i < delta_idx; i++) {
				this->recent_plane = PLANE_INFO(this->recent_plane)->next;
			}
		} else {
            delta_idx = (-delta_idx) % this->chunk_count;
            this->recent_plane_idx = (this->recent_plane_idx + int(this->chunk_count) - delta_idx) % this->chunk_count;
            
            for (int i = 0; i < delta_idx; i++) {
				this->recent_plane = PLANE_INFO(this->recent_plane)->prev;
			}
		}

        if (!PLANE_INFO(this->recent_plane)->loaded) {
            float flwidth, flheight;

            this->feed_client_extent(&flwidth, &flheight);
            construct_plane(this->recent_plane, flwidth, flheight, true);
        }

        this->notify_transfer(this->from_plane, this->recent_plane);
        this->set_window_title("%s", this->recent_plane->name());

		this->from_plane = nullptr;
		this->notify_updated();
	}
}

void GYDM::Cosmos::transfer_to_plane(const std::string& name) {
	int to_idx = plane_index(name);
	
	if (to_idx >= 0) {
		this->transfer_to_plane(to_idx);
	}
}

void GYDM::Cosmos::transfer_to_plane(int to_idx) {
	this->on_navigate(this->recent_plane_idx, to_idx);
}

void GYDM::Cosmos::on_navigate(int from_idx, int to_idx) {
    int delta_idx = to_idx - from_idx;

    if (delta_idx != 0) {
        this->from_plane = this->recent_plane;
        this->transfer(delta_idx);
    }
}

void GYDM::Cosmos::notify_transfer(IPlane* from, IPlane* to) {
    if (from != nullptr) {
        from->on_leave(to);
    }

    if (to != nullptr) {
        to->on_enter(from);
    }
}
