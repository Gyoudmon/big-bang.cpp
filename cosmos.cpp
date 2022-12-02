#include "cosmos.hpp"

#include "virtualization/screen/onionskin.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
#define PLANET_INFO(plane) (static_cast<LinkedPlaneInfo*>(plane->info))

namespace {
    struct LinkedPlaneInfo : public IPlaneInfo {
        LinkedPlaneInfo(IScreen* master) : IPlaneInfo(master) {};
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

static inline void construct_plane(IPlane* plane, float flwidth, float flheight) {
    plane->begin_update_sequence();

    plane->construct(flwidth, flheight);
    plane->load(flwidth, flheight);

    plane->end_update_sequence();
}

static inline void reflow_plane(IPlane* plane, float width, float height) {
    plane->reflow(width, height);
}

static inline void draw_plane(SDL_Renderer* renderer, IPlane* plane, float x, float y, float width, float height) {
    plane->draw(renderer, x, y, width, height);
}

/*************************************************************************************************/
WarGrey::STEM::Cosmos::Cosmos(int fps, uint32_t fgc, uint32_t bgc) : IUniverse(fps, fgc, bgc) {
    this->screen = new OnionSkin(this);
}

WarGrey::STEM::Cosmos::~Cosmos() {
    this->collapse();
    delete this->screen;
}

void WarGrey::STEM::Cosmos::push_plane(IPlane* plane) {
    // NOTE: this method is designed to be invoked in Cosmos::construct()

    if (plane->info == nullptr) {
        LinkedPlaneInfo* info = bind_plane_owership(this->screen, plane);
        
        if (this->head_plane == nullptr) {
            this->head_plane = plane;
            this->recent_plane = plane;
            info->prev = this->head_plane;
        } else { 
            LinkedPlaneInfo* head_info = PLANET_INFO(this->head_plane);
            LinkedPlaneInfo* prev_info = PLANET_INFO(head_info->prev);
             
            info->prev = head_info->prev;
            prev_info->next = plane;
            head_info->prev = plane;
        }

        info->next = this->head_plane;
    }
}

void WarGrey::STEM::Cosmos::collapse() {
    if (this->head_plane != nullptr) {
        IPlane* temp_head = this->head_plane;
        LinkedPlaneInfo* temp_info = PLANET_INFO(temp_head);
        LinkedPlaneInfo* prev_info = PLANET_INFO(temp_info->prev);

        this->head_plane = nullptr;
        this->recent_plane = nullptr;
        prev_info->next = nullptr;

        do {
            IPlane* child = temp_head;

            temp_head = PLANET_INFO(temp_head)->next;

            delete child; // plane's destructor will delete the associated info object
        } while (temp_head != nullptr);
    }
}

bool WarGrey::STEM::Cosmos::can_exit() {
    return (this->recent_plane != nullptr) && this->recent_plane->can_exit();
}

/*************************************************************************************************/
void WarGrey::STEM::Cosmos::on_big_bang(int width, int height) {
    if (this->head_plane != nullptr) {
        IPlane* child = this->head_plane;
        float flwidth = float(width);
        float flheight = float(height);

        do {
            LinkedPlaneInfo* info = PLANET_INFO(child);

            construct_plane(child, flwidth, flheight);
            child = info->next;
        } while (child != this->head_plane);

        this->set_window_title("%s", this->recent_plane->name());
    }
}

void WarGrey::STEM::Cosmos::reflow(float width, float height) {
    if ((width > 0.0F) && (height > 0.0F)) {
        if (this->head_plane != nullptr) {
            IPlane* child = this->head_plane;

            do {
                LinkedPlaneInfo* info = PLANET_INFO(child);

                reflow_plane(child, width, height);
                child = info->next;
            } while (child != this->head_plane);
        }
    }
}

void WarGrey::STEM::Cosmos::on_elapse(uint32_t count, uint32_t interval, uint32_t uptime) {
    this->begin_update_sequence();

    if (this->head_plane != nullptr) {
        IPlane* child = PLANET_INFO(this->recent_plane)->next;
        
        this->recent_plane->begin_update_sequence();
        this->recent_plane->on_elapse(count, interval, uptime);
        this->recent_plane->end_update_sequence();

        while (child != this->recent_plane) {
            child->on_elapse(count, interval, uptime);
            child = PLANET_INFO(child)->next;
        }
    }

    this->update(count, interval, uptime);

    this->end_update_sequence();
}

void WarGrey::STEM::Cosmos::draw(SDL_Renderer* renderer, int x, int y, int width, int height) {
    float flx = float(x);
    float fly = float(y);
    float flwidth = float(width);
    float flheight = float(height);

    // NOTE: only the current plane needs to be drawn

    if (this->recent_plane != nullptr) {
        draw_plane(renderer, this->recent_plane, flx, fly, flwidth, flheight);
    }
}

/*************************************************************************************************/
void WarGrey::STEM::Cosmos::on_mouse_event(SDL_MouseButtonEvent& m, bool pressed) {
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

void WarGrey::STEM::Cosmos::on_mouse_move(uint32_t state, int x, int y, int dx, int dy) {
    if (this->recent_plane != nullptr) {
        this->begin_update_sequence();
        this->recent_plane->on_pointer_move(state, float(x), float(y), float(dx), float(dy));
        this->end_update_sequence();
    }
}

void WarGrey::STEM::Cosmos::on_scroll(int horizon, int vertical, float hprecise, float vprecise) {
    if (this->recent_plane != nullptr) {
        this->begin_update_sequence();
        this->recent_plane->on_scroll(horizon, vertical, hprecise, vprecise);
        this->end_update_sequence();
    }
}

void WarGrey::STEM::Cosmos::on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) {
    if (this->recent_plane != nullptr) {
        this->begin_update_sequence();
        this->recent_plane->on_char(key, modifiers, repeats, pressed);
        this->end_update_sequence();
    }
}

void WarGrey::STEM::Cosmos::on_text(const char* text, size_t size, bool entire) {
    if (this->recent_plane != nullptr) {
        this->begin_update_sequence();
        this->recent_plane->on_text(text, size, entire);
        this->end_update_sequence();
    }
}

void WarGrey::STEM::Cosmos::on_editing_text(const char* text, int pos, int span) {
    if (this->recent_plane != nullptr) {
        this->begin_update_sequence();
        this->recent_plane->on_editing_text(text, pos, span);
        this->end_update_sequence();
    }
}
            
void WarGrey::STEM::Cosmos::on_save() {
    if (this->recent_plane != nullptr) {
        this->recent_plane->on_save();
    }
}

