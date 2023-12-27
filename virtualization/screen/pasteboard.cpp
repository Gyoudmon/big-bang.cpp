#include "pasteboard.hpp"

#include "../../plane.hpp"
#include "../../matter.hpp"

using namespace GYDM;

/*************************************************************************************************/
IDisplay* GYDM::Pasteboard::display() {
	IPlane* plane = this->matter->master();
	IDisplay* display = nullptr;

	if (plane != nullptr) {
		display = plane->master()->display();
	}

	return display;
}

void GYDM::Pasteboard::refresh() {
	IDisplay* display = this->display();
	
	if (display != nullptr) {
		display->refresh();
	}
}

void GYDM::Pasteboard::feed_extent(float* width, float* height) {
	IDisplay* display = this->display();

	if (display != nullptr) {
		display->feed_extent(width, height);
	}
}

void GYDM::Pasteboard::feed_client_extent(float* width, float* height) {
	IDisplay* display = this->display();

	if (display != nullptr) {
		display->feed_client_extent(width, height);
	}
}

uint32_t GYDM::Pasteboard::frame_rate() {
	IDisplay* display = this->display();

	if (display != nullptr) {
		return display->frame_rate();
	} else {
		return 0;
	}
}

void GYDM::Pasteboard::begin_update_sequence() {
	IDisplay* display = this->display();

	if (display != nullptr) {
		display->begin_update_sequence();
	}
}

bool GYDM::Pasteboard::is_in_update_sequence() {
	IDisplay* display = this->display();

	if (display != nullptr) {
		return display->is_in_update_sequence();
	} else {
		return false;
	}
}

void GYDM::Pasteboard::end_update_sequence() {
	IDisplay* display = this->display();

	if (display != nullptr) {
		display->end_update_sequence();
	}
}

bool GYDM::Pasteboard::should_update() {
	IDisplay* display = this->display();

	if (display != nullptr) {
		return display->should_update();
	} else {
		return false;
	}
}

void GYDM::Pasteboard::notify_updated() {
	IDisplay* display = this->display();

	if (display != nullptr) {
		display->notify_updated();
	}
}

void GYDM::Pasteboard::log_message(Log level, const std::string& message) {
	IDisplay* display = this->display();

	if (display != nullptr) {
		display->log_message(level, message);
	};
}

void GYDM::Pasteboard::start_input_text(const std::string& prompt) {
	IDisplay* display = this->display();

	if (display != nullptr) {
		display->start_input_text(prompt);
	}
}
