#pragma once

#include "../matter.hpp"
#include "../graphics/color.hpp"
#include "../virtualization/screen.hpp"
#include "../forward.hpp"

namespace WarGrey::STEM {
	class __lambda__ Continent : public WarGrey::STEM::IMatter {
	public:
		virtual ~Continent() noexcept;
		
		Continent(WarGrey::STEM::IPlane* planet, const WarGrey::STEM::RGBA& background = transparent);
		Continent(WarGrey::STEM::IPlane* planet, float width, float height = 0.0F, const WarGrey::STEM::RGBA& background = transparent);

		const char* name() override;

	public:
		void construct(SDL_Renderer* renderer) override;
		void feed_extent(float x, float y, float* width = nullptr, float* height = nullptr) override;
		int update(uint64_t count, uint32_t interval, uint64_t uptime) override;
		void draw(SDL_Renderer* renderer, float x, float y, float Width, float Height) override;

	public:
		void set_background_color(const WarGrey::STEM::RGBA& color);
		void set_border_color(const WarGrey::STEM::RGBA& color);

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
		WarGrey::STEM::RGBA background;
		WarGrey::STEM::RGBA border;
		float width;
		float height;
	};
}
