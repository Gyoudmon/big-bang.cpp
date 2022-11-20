#include "cosmos.hpp"

#include "virtualization/screen/onionskin.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
#define PLANET_INFO(planet) (static_cast<LinkedPlanetInfo*>(planet->info))

namespace {
    struct LinkedPlanetInfo : public IPlanetInfo {
        LinkedPlanetInfo(IScreen* master) : IPlanetInfo(master) {};
        IPlanet* next;
        IPlanet* prev;
    };
}

/*************************************************************************************************/
static inline LinkedPlanetInfo* bind_planet_owership(IScreen* master, IPlanet* planet) {
    auto info = new LinkedPlanetInfo(master);
    
    planet->info = info;

    return info;
}

static inline void construct_planet(IPlanet* planet, float flwidth, float flheight) {
    planet->begin_update_sequence();

    planet->construct(flwidth, flheight);
    planet->load(flwidth, flheight);

    planet->end_update_sequence();
}

static inline void reflow_planet(IPlanet* planet, float width, float height) {
    planet->reflow(width, height);
}

static inline void draw_planet(SDL_Renderer* renderer, IPlanet* planet, float x, float y, float width, float height) {
    planet->draw(renderer, x, y, width, height);
}

/*************************************************************************************************/
WarGrey::STEM::Cosmos::Cosmos(int fps, uint32_t fgc, uint32_t bgc) : IUniverse(fps, fgc, bgc) {
    this->screen = new OnionSkin(this);
}

WarGrey::STEM::Cosmos::~Cosmos() {
    this->collapse();
}

void WarGrey::STEM::Cosmos::push_planet(IPlanet* planet) {
    // NOTE: this method is designed to be invoked in Cosmos::construct()

    if (planet->info == nullptr) {
        LinkedPlanetInfo* info = bind_planet_owership(this->screen, planet);
        
        if (this->head_planet == nullptr) {
            this->head_planet = planet;
            this->recent_planet = planet;
            info->prev = this->head_planet;
        } else { 
            LinkedPlanetInfo* head_info = PLANET_INFO(this->head_planet);
            LinkedPlanetInfo* prev_info = PLANET_INFO(head_info->prev);
             
            info->prev = head_info->prev;
            prev_info->next = planet;
            head_info->prev = planet;
        }

        info->next = this->head_planet;
    }
}

void WarGrey::STEM::Cosmos::collapse() {
    if (this->head_planet != nullptr) {
        IPlanet* temp_head = this->head_planet;
        LinkedPlanetInfo* temp_info = PLANET_INFO(temp_head);
        LinkedPlanetInfo* prev_info = PLANET_INFO(temp_info->prev);

        this->head_planet = nullptr;
        this->recent_planet = nullptr;
        prev_info->next = nullptr;

        do {
            IPlanet* child = temp_head;

            temp_head = PLANET_INFO(temp_head)->next;

            delete child; // planet's destructor will delete the associated info object
        } while (temp_head != nullptr);
    }
}

bool WarGrey::STEM::Cosmos::can_exit() {
    return (this->recent_planet != nullptr) && this->recent_planet->can_exit();
}

/*************************************************************************************************/
void WarGrey::STEM::Cosmos::on_big_bang(int width, int height) {
    if (this->head_planet != nullptr) {
        IPlanet* child = this->head_planet;
        float flwidth = float(width);
        float flheight = float(height);

        do {
            LinkedPlanetInfo* info = PLANET_INFO(child);

            construct_planet(child, flwidth, flheight);
            child = info->next;
        } while (child != this->head_planet);

        this->set_window_title("%s", this->recent_planet->name());
    }
}

void WarGrey::STEM::Cosmos::reflow(float width, float height) {
    if ((width > 0.0F) && (height > 0.0F)) {
        if (this->head_planet != nullptr) {
            IPlanet* child = this->head_planet;

            do {
                LinkedPlanetInfo* info = PLANET_INFO(child);

                reflow_planet(child, width, height);
                child = info->next;
            } while (child != this->head_planet);
        }
    }
}

void WarGrey::STEM::Cosmos::on_elapse(uint32_t count, uint32_t interval, uint32_t uptime) {
    this->begin_update_sequence();

    if (this->head_planet != nullptr) {
        IPlanet* child = PLANET_INFO(this->recent_planet)->next;
        
        this->recent_planet->begin_update_sequence();
        this->recent_planet->on_elapse(count, interval, uptime);
        this->recent_planet->end_update_sequence();

        while (child != this->recent_planet) {
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

    // NOTE: only the current planet needs to be drawn

    if (this->recent_planet != nullptr) {
        draw_planet(renderer, this->recent_planet, flx, fly, flwidth, flheight);
    }
}

/*************************************************************************************************/
void WarGrey::STEM::Cosmos::on_mouse_event(SDL_MouseButtonEvent& m, bool pressed) {
    if (this->recent_planet != nullptr) {
        uint8_t button = m.button;
        float flx = float(m.x);
        float fly = float(m.y);
        uint8_t clicks = m.clicks;

        this->begin_update_sequence();
        if (pressed) {
            this->recent_planet->on_pointer_pressed(button, flx, fly, clicks, false);
        } else {
            this->recent_planet->on_pointer_released(button, flx, fly, clicks, false);
        }
        this->end_update_sequence();
    }
}

void WarGrey::STEM::Cosmos::on_mouse_move(uint32_t state, int x, int y, int dx, int dy) {
    if (this->recent_planet != nullptr) {
        this->begin_update_sequence();
        this->recent_planet->on_pointer_move(state, float(x), float(y), float(dx), float(dy), false);
        this->end_update_sequence();
    }
}

void WarGrey::STEM::Cosmos::on_scroll(int horizon, int vertical, float hprecise, float vprecise) {
    if (this->recent_planet != nullptr) {
        this->begin_update_sequence();
        this->recent_planet->on_scroll(horizon, vertical, hprecise, vprecise);
        this->end_update_sequence();
    }
}

void WarGrey::STEM::Cosmos::on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) {
    if (this->recent_planet != nullptr) {
        this->begin_update_sequence();
        this->recent_planet->on_char(key, modifiers, repeats, pressed);
        this->end_update_sequence();
    }
}

void WarGrey::STEM::Cosmos::on_text(const char* text, size_t size, bool entire) {
    if (this->recent_planet != nullptr) {
        this->begin_update_sequence();
        this->recent_planet->on_text(text, size, entire);
        this->end_update_sequence();
    }
}

void WarGrey::STEM::Cosmos::on_editing_text(const char* text, int pos, int span) {
    if (this->recent_planet != nullptr) {
        this->begin_update_sequence();
        this->recent_planet->on_editing_text(text, pos, span);
        this->end_update_sequence();
    }
}
            
void WarGrey::STEM::Cosmos::on_save() {
    if (this->recent_planet != nullptr) {
        this->recent_planet->on_save();
    }
}

