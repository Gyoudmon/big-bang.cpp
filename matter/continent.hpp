#pragma once

#include "../matter.hpp"
#include "../virtualization/screen.hpp"
#include "../forward.hpp"

namespace WarGrey::STEM {
	class Continent : public WarGrey::STEM::IMatter {
	public:
		virtual ~Continent() noexcept;
		
		Continent(WarGrey::STEM::IPlane* planet, uint32_t background = 0U, double alpha = 0.0);
		Continent(WarGrey::STEM::IPlane* planet, float width, float height = 0.0F, uint32_t background = 0U, double alpha = 0.0);

		const char* name() override;

	public:
		void construct(SDL_Renderer* renderer) override;
		void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
		int update(uint64_t count, uint32_t interval, uint64_t uptime) override;
		void draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) override;

	public:
		void set_background_color(uint32_t color, double alpha = 1.0);
		void set_border_color(uint32_t color, double alpha = 1.0);

    public: // low-level events
        bool on_pointer_pressed(uint8_t button, float local_x, float local_y, uint8_t clicks) override;
        bool on_pointer_move(uint32_t state, float local_x, float local_y, float dx, float dy, bool bye) override;
        bool on_pointer_released(uint8_t button, float local_x, float local_y, uint8_t clicks) override;

	public:
		template<typename P> P* unsafe_plane() { return static_cast<P*>(this->plane); }

	private:
		WarGrey::STEM::IPlane* plane;

	private:
		WarGrey::STEM::IScreen* screen;
		uint32_t background;
		double bg_alpha;
		uint32_t border_color;
		double bd_alpha;
		float width;
		float height;
	};
}
