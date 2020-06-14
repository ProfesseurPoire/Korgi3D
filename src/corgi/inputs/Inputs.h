#pragma once

#include <corgi/inputs/Keyboard.h>
#include <corgi/math/Vec2.h>

#include <vector>
#include <functional>
#include <iostream>

#include "Gamepad.h"

namespace corgi
{
	class Window;

	/*!
	 * @brief	I'm kinda wondering if maybe I should be using event instead of polling?
	 *			Though Unity don't really give much shit about that for instance, it just
	 *			assume you're going to detect move events yourself for instance
	 */
	class Mouse
	{
		friend class Inputs;
		friend class Window;

	public:
		
		enum class Button : int
		{
			Left	=  0,
			Middle	=  1,
			Right	=  2
		};

		struct DragEvent
		{
			DragEvent(int x, int y, Button button)
				: x(x),y(y),button(button){}
			int x;
			int y;
			Button button;
		};

		[[nodiscard]] int previous_x()const;

		[[nodiscard]] int previous_y()const;

		/*!
		 * @brief	Returns the mouse x position relative to the screen left top corner
		 */
		[[nodiscard]] int x()const;

		/*!
		 * @brief	Returns the mouse y position relative to the screen top left corner
		 */
		[[nodiscard]] int y()const;

		/*!
		 * @brief	Returns the mouse's position as a Vec2 object
		 */
		[[nodiscard]] Vec2i position()const noexcept;

		/*!
		 * @brief	Returns true if the mouse moved during this frame
		 */
		[[nodiscard]] bool has_moved()const;

		/*!
		 * @brief Returns true if the requested mouse button is held down
		 */
		[[nodiscard]] bool is_button_pressed(Button button) const;

		/*!
		 * @brief	Returns true if the requested mouse button has been pressed during the current frame
		 */
		[[nodiscard]] bool is_button_down(Button button)const;

		/*!
		 * @brief	Returns true if the requested mouse button has been released during the current frame
		 */
		[[nodiscard]] bool is_button_up(Button button)const;

	protected:

		void update();

	private:

		int x_;
		int y_;

		int previous_x_;
		int previous_y_;

		bool move_event_;
		bool button_event_;
		//  Should I send events or should I keep just watching for the 
		// state of the thing?
		// Like MouseButtonEvent -> Mouse Down -> Button 1
		// MouseMove event -> x and y difference
		// MouseDragEvent -> x and y diff + button down
		bool drag_event_;

		int buttons_[3];
		int buttons_down_[3];
		int buttons_up_[3];
	};
	
	class Inputs
	{
	public:

		friend class Window;
		friend class Game;

		static const Mouse& mouse();
		static const Keyboard& keyboard();
		static const Gamepad& gamepad(int id);

		/*!
		 * @brief	Returns true if a gamepad exist at the given ID
		 */
		static bool is_gamepad_connected(unsigned int id) noexcept;

		static void finalize();
		
		static void on_mouse_drag(Mouse::Button button, std::function<void(Mouse::DragEvent)> e)
		{
			switch (button)
			{
			case Mouse::Button::Left:
				mouse_left_drag_events_.push_back(e);
				break;

			case Mouse::Button::Right:
				mouse_right_drag_events_.push_back(e);
				break;

			case Mouse::Button::Middle:
				mouse_middle_drag_events_.push_back(e);
				break;
			}
		}

	protected:

		static inline Keyboard				keyboard_;
		static inline Mouse				mouse_;
		static inline std::vector<Gamepad>	gamepads_;

		static void update();

		static Mouse& non_const_mouse();
		static Keyboard& non_const_keyboard();

		static std::vector<std::function<void(Mouse::DragEvent)>> mouse_left_drag_events_;
		static std::vector<std::function<void(Mouse::DragEvent)>> mouse_right_drag_events_;
		static std::vector<std::function<void(Mouse::DragEvent)>> mouse_middle_drag_events_;
	};

	class Axis
	{
	public:

		Axis(Key positive, Key negative)
			: positive_(positive), negative_(negative){}

		void add_hat_values(Gamepad::Hat positive, Gamepad::Hat negative)
		{
			positive_hat_.push_back(positive);
			negative_hat_.push_back(negative);

			use_hat_ = true;
		}

		void set_axis(int id)
		{
			axe_id_ = id;
		}

		float value()
		{
			if(axe_id_!=-1)
			{
				const float threshold = 0.3f;
				if(Inputs::is_gamepad_connected(0))
				{
					if( (Inputs::gamepad(0).axis(axe_id_) > threshold) ||
						(Inputs::gamepad(0).axis(axe_id_) < -threshold)
						)
					{
						return Inputs::gamepad(0).axis(axe_id_);
					}
				}
			}
			
			if (Inputs::keyboard().key(positive_)  || ( use_hat_ &&is_positive_hat_triggered()))
			{
				return 1.0f;
			}

			if (Inputs::keyboard().key(negative_) ||(use_hat_ && is_negative_hat_triggered()))
			{
				return -1.0f;
			}
			return 0.0f;
		}

	private:

		bool is_hat_triggered(const std::vector<Gamepad::Hat>& hat_values)
		{
			if (!Inputs::is_gamepad_connected(0))
			{
				return false;
			}

			for (auto hat : hat_values)
			{
				if (hat == Inputs::gamepad(0).hat())
				{
					return true;
				}
			}
			return false;
		}
		
		bool is_positive_hat_triggered()
		{
			return is_hat_triggered(positive_hat_);
		}

		bool is_negative_hat_triggered()
		{
			return is_hat_triggered(negative_hat_);
		}

		int axe_id_ = -1;
		
		// The axis will take a positive value when the key is positive
		Key positive_;

		// the axis will take a negative value when the negative key is pressed
		Key negative_;

		bool use_hat_ = false;
		
		std::vector<Gamepad::Hat> positive_hat_;
		std::vector<Gamepad::Hat> negative_hat_;
	};
}