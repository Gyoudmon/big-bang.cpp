#include "pasteboard.hpp"

#include "../../plane.hpp"
#include "../../matter.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
IDisplay* WarGrey::STEM::Pasteboard::display() {
	IPlane* plane = this->matter->master();
	IDisplay* display = nullptr;

	if (plane != nullptr) {
		display = plane->master()->display();
	}

	return display;
}

void WarGrey::STEM::Pasteboard::refresh() {
	IDisplay* display = this->display();
	
	if (display != nullptr) {
		display->refresh();
	}
}

void WarGrey::STEM::Pasteboard::feed_extent(float* width, float* height) {
	IDisplay* display = this->display();

	if (display != nullptr) {
		display->feed_extent(width, height);
	}
}

void WarGrey::STEM::Pasteboard::feed_client_extent(float* width, float* height) {
	IDisplay* display = this->display();

	if (display != nullptr) {
		display->feed_client_extent(width, height);
	}
}

uint32_t WarGrey::STEM::Pasteboard::frame_rate() {
	IDisplay* display = this->display();

	if (display != nullptr) {
		return display->frame_rate();
	} else {
		return 0;
	}
}

void WarGrey::STEM::Pasteboard::begin_update_sequence() {
	IDisplay* display = this->display();

	if (display != nullptr) {
		display->begin_update_sequence();
	}
}

bool WarGrey::STEM::Pasteboard::is_in_update_sequence() {
	IDisplay* display = this->display();

	if (display != nullptr) {
		return display->is_in_update_sequence();
	} else {
		return false;
	}
}

void WarGrey::STEM::Pasteboard::end_update_sequence() {
	IDisplay* display = this->display();

	if (display != nullptr) {
		display->end_update_sequence();
	}
}

bool WarGrey::STEM::Pasteboard::should_update() {
	IDisplay* display = this->display();

	if (display != nullptr) {
		return display->should_update();
	} else {
		return false;
	}
}

void WarGrey::STEM::Pasteboard::notify_updated() {
	IDisplay* display = this->display();

	if (display != nullptr) {
		display->notify_updated();
	}
}

void WarGrey::STEM::Pasteboard::log_message(int fgc, const std::string& message) {
	IDisplay* display = this->display();

	if (display != nullptr) {
		display->log_message(fgc, message);
	};
}

void WarGrey::STEM::Pasteboard::start_input_text(const std::string& prompt) {
	IDisplay* display = this->display();

	if (display != nullptr) {
		display->start_input_text(prompt);
	}
}
