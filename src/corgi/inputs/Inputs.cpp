#include "Inputs.h"
#include "Keyboard.h"

namespace corgi
{
	std::vector<std::function<void(Mouse::DragEvent)>> Inputs::mouse_left_drag_events_;
	std::vector<std::function<void(Mouse::DragEvent)>> Inputs::mouse_right_drag_events_;
	std::vector<std::function<void(Mouse::DragEvent)>> Inputs::mouse_middle_drag_events_;

	bool Mouse::has_moved() const
	{
		return move_event_;
	}

	void Mouse::update()
	{
		move_event_ = false;

		for(int i=0; i< 3; ++i)
		{
			buttons_down_[i]	= false;
			buttons_up_[i]		= false;
		}
	}

	int Mouse::previous_x() const
	{
		return previous_x_;
	}

	int Mouse::previous_y() const
	{
		return previous_y_;
	}

	int Mouse::x() const
	{
		return x_;
	}

	int Mouse::y() const
	{
		return y_;
	}

	Vec2i Mouse::position() const noexcept
	{
		return Vec2i(x_, y_);
	}

	bool Mouse::is_button_down(Mouse::Button button) const
	{
		return buttons_down_[static_cast<int>(button)];
	}

	bool Mouse::is_button_pressed(Mouse::Button button) const
	{
		return buttons_[static_cast<int>(button)];
	}

	bool Mouse::is_button_up(Mouse::Button button) const
	{
		return buttons_[static_cast<int>(button)];
	}
						
	const Keyboard& Inputs::keyboard()
	{
		return keyboard_;
	}

	const Gamepad& Inputs::gamepad(int id)
	{
		return gamepads_[id];
	}

	void Inputs::finalize()
	{
		gamepads_.clear();
	}

	bool Inputs::is_gamepad_connected(unsigned int index)noexcept
	{
		return (index < gamepads_.size()&& index >=0);
	}

	const Mouse& Inputs::mouse()
	{
		return mouse_;
	}

	Mouse&	Inputs::non_const_mouse()
	{
		return mouse_;
	}

	Keyboard&	Inputs::non_const_keyboard()
	{
		return keyboard_;
	}

	void Inputs::update()
	{
		keyboard_.update();
		mouse_.update();

		for(auto& gamepad :  gamepads_)
		{
			gamepad.update();
		}
	}
}
