#pragma once

#include "plane.hpp"

#include <vector>

namespace WarGrey::STEM {
	class INavigatorListener {
	public:
		virtual void on_navigate(int from_index, int to_index) = 0;
	};

	class INavigator {
	public:
		virtual void insert(WarGrey::STEM::IPlane* planet) = 0;
		virtual void select(WarGrey::STEM::IPlane* planet) = 0;
		virtual int selected_index() = 0;

	public:
		void push_navigation_listener(WarGrey::STEM::INavigatorListener* actor);
		void navigate(int from_index, int to_index);

	private:
		std::vector<WarGrey::STEM::INavigatorListener*> listeners;
	};
}
