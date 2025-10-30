#pragma once

#include "../matter.hpp"
#include "../physics/color/rgba.hpp"
#include "../physics/geometry/aabox.hpp"
#include "../virtualization/screen.hpp"
#include "../forward.hpp"

namespace Plteen {
	class __lambda__ Planelet : public Plteen::IMatter {
	public:
		virtual ~Planelet() noexcept;
		
		Planelet(Plteen::IPlane* planet, const Plteen::RGBA& background = transparent);
		Planelet(Plteen::IPlane* planet, float width, float height = 0.0F, const Plteen::RGBA& background = transparent);

		const char* name() override;

	public:
		void construct(Plteen::dc_t* dc) override;
		Plteen::Box get_bounding_box() override;
		int update(uint64_t count, uint32_t interval, uint64_t uptime) override;
		void draw(Plteen::dc_t* dc, float x, float y, float Width, float Height) override;

	public:
		void set_background_color(const Plteen::RGBA& color);
		void set_grid_color(const Plteen::RGBA& color);
		void set_border_color(const Plteen::RGBA& color);

    public: // low-level events
        bool on_pointer_pressed(uint8_t button, float local_x, float local_y, uint8_t clicks) override;
        bool on_pointer_move(uint32_t state, float local_x, float local_y, float dx, float dy, bool bye) override;
        bool on_pointer_released(uint8_t button, float local_x, float local_y, uint8_t clicks) override;

	public:
		template<typename P> P* unsafe_plane() { return static_cast<P*>(this->plane); }

	private:
		Plteen::IPlane* plane;

	private:
		Plteen::IScreen* screen;
		Plteen::RGBA background;
		Plteen::RGBA border;
		float width;
		float height;
	};
}
