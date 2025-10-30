#include "planet.hpp"

#include "../plane.hpp"
#include "../datum/box.hpp"
#include "../graphics/misc.hpp"

#include "../virtualization/screen/pasteboard.hpp"

using namespace Plteen;

/*************************************************************************************************/
namespace {
	class PlaceholderPlane : public Plane {
	public:
		virtual ~PlaceholderPlane() noexcept {}
		PlaceholderPlane() : Plane("_") {}
	};

	class PlaneInfo : public Plteen::IPlaneInfo {
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
Plteen::Planelet::Planelet(IPlane* plane, const Plteen::RGBA& background)
	: Planelet(plane, 0.0F, 0.0F, background) {}

Plteen::Planelet::Planelet(IPlane* plane, float width, float height, const Plteen::RGBA& background)
		: plane(plane), background(background), border(transparent), width(width), height(height) {
	if (this->plane == nullptr) {
		this->plane = new PlaceholderPlane();
	}

	this->screen = new Pasteboard(this);
	this->enable_resize(false);

	// Client applications make their own decisions
	// this->enable_events(true, true);
}

Plteen::Planelet::~Planelet() noexcept {
	delete this->plane;
	delete this->screen;
}

const char* Plteen::Planelet::name() {
	return this->plane->name();
}

/*************************************************************************************************/
void Plteen::Planelet::construct(Plteen::dc_t* dc) {
	bind_subplane_owership(this->screen, this->plane);
	construct_subplane(this->plane, this->width, this->height);
}

Box Plteen::Planelet::get_bounding_box() {
	Box box;

	if ((this->width > 0.0F) && (this->height > 0.0F)) {
		box = { this->width, this->height };
	} else {
		Box sub = this->plane->get_bounding_box();
		float w = (this->width > 0.0F) ?  this->width  : sub.rx();
		float h = (this->height > 0.0F) ? this->height : sub.by();

		box = { w, h };
	}

	return box;
}

int Plteen::Planelet::update(uint64_t count, uint32_t interval, uint64_t uptime) {
	this->plane->begin_update_sequence();
	this->plane->on_elapse(count, interval, uptime);
	this->plane->end_update_sequence();

	return 0;
}

void Plteen::Planelet::draw(Plteen::dc_t* dc, float x, float y, float Width, float Height) {
	if (this->background.is_opacity()) {
        dc->fill_rect(x, y, Width, Height, this->background);
    }

	if (this->border.is_opacity()) {
        dc->draw_rect(x, y, Width, Height, this->border);
    }

	this->plane->draw(dc, x, y, Width, Height);
}

/**************************************************************************************************/
void Plteen::Planelet::set_background_color(const RGBA& color) {
	if (this->background != color) {
		this->background = color;
		this->notify_updated();
	}
}

void Plteen::Planelet::set_grid_color(const RGBA& color) {
	if (this->background != color) {
		this->background = color;
		this->notify_updated();
	}
}

void Plteen::Planelet::set_border_color(const RGBA& color) {
	if (this->border != color) {
		this->border = color;
		this->notify_updated();
	}
}

/**************************************************************************************************/
bool Plteen::Planelet::on_pointer_pressed(uint8_t button, float local_x, float local_y, uint8_t clicks) {
	return this->plane->on_pointer_pressed(button, local_x, local_y, clicks);
}

bool Plteen::Planelet::on_pointer_move(uint32_t state, float local_x, float local_y, float dx, float dy, bool bye) {
	return this->plane->on_pointer_move(state, local_x, local_y, dx, dy);
}

bool Plteen::Planelet::on_pointer_released(uint8_t button, float local_x, float local_y, uint8_t clicks) {
	return this->plane->on_pointer_released(button, local_x, local_y, clicks);
}
