#pragma once

#include "../matter.hpp"

#include "../datum/enum.hpp"
#include "../datum/flonum.hpp"

namespace GYDM {
    class __lambda__ IGraphlet : public GYDM::IMatter {
		// Yes, meanwhile it's empty
    };

    template<typename T>
	class __lambda__ IValuelet : public virtual GYDM::IGraphlet {
    public:
		T get_value() {
			return this->guarded_value();
		}

		void set_value(T value, bool update_now = false) {
			*(this->value) = value;

			if (update_now) {
				this->update_value_now();
			}
		}

		void bind_value(T& address) {
			this->value = &address;
		}
		
		void bind_value(T* address) {
			if (address != nullptr) {
				this->value = address;
			} else {
				this->value = &this->shadow;
			}
		}

		void set_value_port(GYDM::MatterPort port) {
			this->port = port;
		}

		GYDM::MatterPort get_value_port() {
			return this->port;
		}

	public:
		int update(uint64_t count, uint32_t interval, uint64_t uptime) override {
			this->update_value_now();
			return 0;
		}
		
	protected:
		virtual void on_value_changed(GYDM::dc_t* renderer, T value) {}
		virtual T guarded_value() { return *(this->value); }

	private:
		void update_value_now() {
			T cur_value = this->guarded_value();

			if (last_value != cur_value) {
				GYDM::dc_t* dc = this->drawing_context();

				last_value = cur_value;
				this->moor(this->port);
				if (dc != nullptr) this->on_value_changed(dc, cur_value);
				this->notify_updated();
			}
		}

	private:
		MatterPort port = GYDM::MatterPort::LT;
		T shadow = T();

	private:
		T last_value = T();
		T* value = &this->shadow;
	};

    template<typename T>
	class __lambda__ IRangelet : public virtual GYDM::IValuelet<T> {
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
		void set_range(T vmin, T vmax) {
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

				if (changed) {
					GYDM::dc_t* dc = this->drawing_context();

					this->moor(this->get_value_port());
					if (dc != nullptr) this->on_range_changed(dc, this->vmin, this->vmax);
					this->notify_updated();
				}
			}
		}

		virtual void on_range_changed(GYDM::dc_t* ds, T vmin, T vmax) {}

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

		T guarded_value() override {
			T cur_value = IValuelet<T>::guarded_value();

			if (cur_value < this->vmin) {
				cur_value = this->vmin;
			} else if (cur_value > this->vmax) {
				cur_value = this->vmax;
			}

			return cur_value;
		}

	protected:
		T vmin;
		T vmax;
	};

	template<typename State, typename Style>
	class __lambda__ IStatelet : public virtual GYDM::IGraphlet {
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
		void construct(GYDM::dc_t* dc) override {
			this->update_state(dc);
		}

	public:		
		void set_state(State state) {
			unsigned int new_state = ((state == State::_) ? this->default_state : _I(state));

			if (this->current_state != new_state) {
				GYDM::dc_t* dc = this->drawing_context();

				this->current_state = new_state;
				if (dc != nullptr) this->update_state(dc);
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
				GYDM::dc_t* dc = this->drawing_context();

				if (dc != nullptr) this->update_state(dc);
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
		void update_state(GYDM::dc_t* dc) {
			this->apply_style(this->get_style(), dc);
			this->on_state_changed(_E(State, this->current_state));
		}

	protected:
		virtual void prepare_style(State status, Style& style) = 0;
		virtual void on_state_changed(State status) {}
		virtual void apply_style(Style& style, GYDM::dc_t* dc) {}

	private:
		unsigned int default_state;
		unsigned int current_state;
		Style styles[_N(State)];
		bool style_ready[_N(State)];
	};
}
