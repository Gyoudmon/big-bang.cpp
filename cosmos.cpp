#include "cosmos.hpp"

#include "virtualization/screen/onionskip.hpp"

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
WarGrey::STEM::Cosmos::Cosmos(int fps, uint32_t fgc, uint32_t bgc)
    : IUniverse(fps, fgc, bgc), head_planet(nullptr), recent_planet(nullptr) {
    this->screen = new OnionSkip(this);
    this->set_cmdwin_height(24);
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

void WarGrey::STEM::Cosmos::reflow(int nwidth, int nheight) {
    if ((nwidth > 0.0F) && (nheight > 0.0F)) {
        float width = float(nwidth);
        float height = float(nheight);
        
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

