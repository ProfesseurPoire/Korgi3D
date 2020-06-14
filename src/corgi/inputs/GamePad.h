#pragma once

#include <vector>
#include <map>
#include <string>

namespace corgi
{
	class Gamepad
	{
		friend class Window;
		
	public:

		enum class Hat
		{
			LeftUp,
			Up,
			RightUp,
			Left,
			Centered,
			Right,
			LeftDown,
			Down,
			RightDown
		};

		static inline std::map<Hat, std::string> hat_to_string
		{
			{Hat::LeftUp, "LeftUp"},
			{Hat::Up, "Up" },
			{Hat::RightUp, "RightUp" },
			{Hat::Left, "Left" },
			{Hat::Centered, "Centered" },
			{Hat::Right, "Right"},
			{Hat::LeftDown,"LeftDown"},
			{Hat::Down, "Down"},
			{Hat::RightDown, "RightDown"}
		};

	//   Constructors
		
		Gamepad(int id);
		~Gamepad();

	//  Functions

		void update();

		[[nodiscard]] bool button(int id)const;
		[[nodiscard]] bool button_down(int id)const;
		[[nodiscard]] bool button_up(int id)const;

		[[nodiscard]] int button_count()const;
		[[nodiscard]] float axis(int id)const;

		[[nodiscard]] Hat hat()const;

	private:

		std::vector<bool> buttons_;
		std::vector<bool> buttons_down_;
		std::vector<bool> buttons_up_;

		std::vector<float> axes_;

		Hat hat_ = Hat::Centered;
		
		int id_;

		void* joystick_impl_ = nullptr;
	};
}