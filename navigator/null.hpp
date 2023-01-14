#pragma once

#include "../navigator.hpp"

namespace WarGrey::STEM {
	class NullNavigator : public WarGrey::STEM::INavigator {
		public:
			void insert(WarGrey::SCADA::IPlanet* planet) override {}
			void select(WarGrey::SCADA::IPlanet* planet) override {}
			int selected_index() override { return -1; }
	};
}
