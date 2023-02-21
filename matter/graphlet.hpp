#pragma once

#include "../matter.hpp"

#include "../datum/enum.hpp"
#include "../datum/flonum.hpp"

namespace WarGrey::STEM {
    class IGraphlet : public WarGrey::STEM::IMatter {
        // Yes, Meanwhile it's empty
    };

    template<typename T>
	class IValuelet : public virtual WarGrey::STEM::IGraphlet {
    public:
		T get_value() {
			return this->value;
		}
		
		void set_value(T value0, bool force_update = false) {
			this->set_value(value0, WarGrey::STEM::MatterAnchor::LT, force_update);
		}

		void set_value(T value0, WarGrey::STEM::MatterAnchor anchor, bool force_update = false) {
			T value = this->guarded_value(value0);

			if ((this->value != value) || force_update) {
				if (this->info == nullptr) {
					// NOTE: It is recommand that `set_value()`ing after the graphlet is inserted into a planet
					this->value = value;
				} else {
					this->moor(anchor);
					this->value = value;
					this->on_value_changed(this->master_renderer(), value);
					this->notify_updated();
				}
			}
		}
		
	protected:
		virtual void on_value_changed(SDL_Renderer* renderer, T value) {}

	protected:
		virtual T guarded_value(T value) { return flsafe(value, this->value); }

	private:
		T value = T();
	};

    template<typename T>
	class IRangelet : public virtual WarGrey::STEM::IValuelet<T> {
	public:
		IRangelet(T vmin, T vmax) {
			if (vmin <= vmax) {
				this->vmin = vmin;
				this->vmax = vmax;
			} else {
				this->vmin = vmax;
				this->vmax = vmin;
			}
		}

	public:
		void set_range(T vmin, T vmax, bool force_update = false) {
			this->set_range(vmin, vmax, WarGrey::STEM::MatterAnchor::LT, force_update);
		}

		void set_range(T vmin, T vmax, WarGrey::STEM::MatterAnchor anchor, bool force_update = false) {
			if (this->can_change_range()) {
				bool changed = false;

				if (vmin <= vmax) {
					if ((this->vmin != vmin) || (this->vmax != vmax)) {
						this->vmin = vmin;
						this->vmax = vmax;
						changed = true;
					}
				} else {
					if ((this->vmin != vmax) || (this->vmax != vmin)) {
						this->vmin = vmax;
						this->vmax = vmin;
						changed = true;
					}
				}

				if (force_update || changed) {
					this->moor(anchor);
					this->on_range_changed(this->master_renderer(), this->vmin, this->vmax);
					this->notify_updated();
				}
			}
		}

		virtual void on_range_changed(SDL_Renderer* ds, T vmin, T vmax) {}

	public:
		double get_percentage(T value) {
			double p = 1.0;

			if (this->vmin != this->vmax) {
				double flmin = double(this->vmin);
				
				p = ((double(value) - flmin) / (double(this->vmax) - flmin));
			}

			return p;
		}

		double get_percentage() {
			return this->get_percentage(this->get_value());
		}

	protected:
		virtual bool can_change_range() { return false; }

		T guarded_value(T v) override {
			return flsafe(v, this->vmin, this->vmax);
		}

	protected:
		T vmin;
		T vmax;
	};

	template<typename State, typename Style>
	class IStatelet : public virtual WarGrey::STEM::IGraphlet {
	public:
		IStatelet() : IStatelet(State::_) {}

		IStatelet(State state0) {
			this->default_state = ((state0 == State::_) ? 0 : _I(state0));
			this->current_state = this->default_state;

			for (unsigned int idx = 0; idx < _N(State); idx++) {
				this->style_ready[idx] = false;
			}
		}

	public:
		void construct(SDL_Renderer* renderer) override {
			this->update_state(renderer);
		}

	public:		
		void set_state(State state) {
			unsigned int new_state = ((state == State::_) ? this->default_state : _I(state));

			if (this->current_state != new_state) {
				this->current_state = new_state;
				this->update_state(this->master_renderer());
				this->notify_updated();
			}
		}

		void set_state(bool condition, State state) {
			if (condition) {
				this->set_state(state);
			}
		}

		void set_state(bool condition, State state_yes, State state_no) {
			if (condition) {
				this->set_state(state_yes);
			} else {
				this->set_state(state_no);
			}
		}

		State get_state() {
			return _E(State, this->current_state);
		}

		void set_style(State state, Style& style) {
			unsigned int idx = (state == State::_) ? this->current_state : _I(state);

			this->styles[idx] = style;
			this->style_ready[idx] = false;

			if (idx == this->current_state) {
				this->update_state(this->master_renderer());
				this->notify_updated();
			}
		}

		void set_style(Style& style) {
			for (unsigned int idx = 0; idx < _N(State); idx ++) {
				this->set_style(_E(State, idx), style);
			}
		}

		Style& get_style(State state = State::_) {
			unsigned int idx = (state == State::_) ? this->current_state : _I(state);

			if (!this->style_ready[idx]) {
				this->prepare_style(_E(State, idx), this->styles[idx]);
				this->style_ready[idx] = true;
			}

			return this->styles[idx];
		}

	protected:
		void update_state(SDL_Renderer* renderer) {
			this->apply_style(this->get_style(), renderer);
			this->on_state_changed(_E(State, this->current_state));
		}

	protected:
		virtual void prepare_style(State status, Style& style) = 0;
		virtual void on_state_changed(State status) {}
		virtual void apply_style(Style& style, SDL_Renderer* renderer) {}

	private:
		unsigned int default_state;
		unsigned int current_state;
		Style styles[_N(State)];
		bool style_ready[_N(State)];
	};
}
