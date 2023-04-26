#include "continent.hpp"

#include "../plane.hpp"
#include "../datum/box.hpp"
#include "../graphics/geometry.hpp"

#include "../virtualization/screen/pasteboard.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
namespace {
	class PlaceholderPlane : public Plane {
	public:
		virtual ~PlaceholderPlane() noexcept {}
		PlaceholderPlane() : Plane("_") {}
	};

	class PlaneInfo : public WarGrey::STEM::IPlaneInfo {
	public:
		PlaneInfo(IScreen* master) : IPlaneInfo(master) {};
	};
}

/*************************************************************************************************/
static inline PlaneInfo* bind_subplane_owership(IScreen* master, IPlane* plane) {
	auto info = new PlaneInfo(master);

	plane->info = info;

	return info;
}

static void construct_subplane(IPlane* plane, float width, float height) {
	plane->begin_update_sequence();

	plane->construct(width, height);
	plane->load(width, height);
	plane->reflow(width, height);
	
	plane->end_update_sequence();
}

/**************************************************************************************************/
WarGrey::STEM::Continent::Continent(IPlane* plane, uint32_t background, double alpha)
	: Continent(plane, 0.0F, 0.0F, background, alpha) {}

WarGrey::STEM::Continent::Continent(IPlane* plane, float width, float height, uint32_t background, double alpha)
	: plane(plane), background(background), bg_alpha(alpha), bd_alpha(0.0), width(width), height(height) {
	if (this->plane == nullptr) {
		this->plane = new PlaceholderPlane();
	}

	this->screen = new Pasteboard(this);
	this->enable_resize(false);

	// Client applications make their own decisions
	// this->enable_events(true, true);
}

WarGrey::STEM::Continent::~Continent() noexcept {
	delete this->plane;
	delete this->screen;
}

const char* WarGrey::STEM::Continent::name() {
	return this->plane->name();
}

/*************************************************************************************************/
void WarGrey::STEM::Continent::construct(SDL_Renderer* renderer) {
	bind_subplane_owership(this->screen, this->plane);
	construct_subplane(this->plane, this->width, this->height);
}

void WarGrey::STEM::Continent::feed_extent(float x, float y, float* width, float* height) {
	if ((this->width > 0.0F) && (this->height > 0.0F)) {
		SET_VALUES(width, this->width, height, this->height);
	} else {
		float subx, suby, subw, subh;
		float w = this->width;
		float h = this->height;

		this->plane->feed_matters_boundary(&subx, &suby, &subw, &subh);
		
		if (w <= 0.0F) w = subx + subw;
		if (h <= 0.0F) h = suby + subh;

		SET_VALUES(width, w, height, h);
	}
}

int WarGrey::STEM::Continent::update(uint64_t count, uint32_t interval, uint64_t uptime) {
	this->plane->begin_update_sequence();
	this->plane->on_elapse(count, interval, uptime);
	this->plane->end_update_sequence();

	return 0;
}

void WarGrey::STEM::Continent::draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) {
	if (this->bg_alpha > 0.0F) {
        game_fill_rect(renderer, x, y, Width, Height, this->background, this->bg_alpha);
    }

	if (this->bd_alpha > 0.0F) {
        game_draw_rect(renderer, x, y, Width, Height, this->border_color, this->bd_alpha);
    }

	this->plane->draw(renderer, x, y, Width, Height);
}

/**************************************************************************************************/
void WarGrey::STEM::Continent::set_background_color(uint32_t color, double alpha) {
	if ((this->background != color) || (this->bg_alpha != alpha)) {
		this->background = color;
		this->bg_alpha = alpha;
		this->notify_updated();
	} 
}

void WarGrey::STEM::Continent::set_border_color(uint32_t color, double alpha) {
	if ((this->border_color != color) || (this->bd_alpha != alpha)) {
		this->border_color = color;
		this->bd_alpha = alpha;
		this->notify_updated();
	} 
}

/**************************************************************************************************/
bool WarGrey::STEM::Continent::on_pointer_pressed(uint8_t button, float local_x, float local_y, uint8_t clicks) {
	return this->plane->on_pointer_pressed(button, local_x, local_y, clicks);
}

bool WarGrey::STEM::Continent::on_pointer_move(uint32_t state, float local_x, float local_y, float dx, float dy, bool bye) {
	return this->plane->on_pointer_move(state, local_x, local_y, dx, dy);
}

bool WarGrey::STEM::Continent::on_pointer_released(uint8_t button, float local_x, float local_y, uint8_t clicks) {
	return this->plane->on_pointer_released(button, local_x, local_y, clicks);
}
