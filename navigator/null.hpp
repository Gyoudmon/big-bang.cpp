#pragma once

#include "../navigator.hpp"

namespace WarGrey::STEM {
	class NullNavigator : public WarGrey::STEM::INavigator {
	public:
		void insert(WarGrey::STEM::IPlane* plane) override {}
		void select(WarGrey::STEM::IPlane* plane) override {}
		int selected_index() override { return 0; }
	};
}
