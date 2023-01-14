#include "navigator.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
void INavigator::push_navigation_listener(INavigatorListener* listener) {
	this->listeners.push_back(listener);
}

void INavigator::navigate(int from_index, int to_index) {
	for (INavigatorListener* listener : this->listeners) {
		listener->on_navigate(from_index, to_index);
	}
}
