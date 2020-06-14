#pragma once

#include <vector>
#include <functional>

namespace corgi
{
	class Texture;

	enum class MouseButton : int
	{
		Left	= 0,
		Middle	= 1,
		Right	= 2
	};

	struct Frame
	{
		bool enabled = true;
		float width;
		float height;
		float x;
		float y;

		// rgb can only be 0-255
		int r;
		int g;
		int b;

		corgi::Texture* texture_ = nullptr;

		//Mesh* mesh_;

		private :

		//std::function<void(Mouse)> on_mouse_click;
		//std::function<void(Mouse)> on_mouse_move;
		//std::function<void(Mouse)> on_mouse_double_click;
		//std::function<void(Mouse)> on_mouse_drag;
	};

    /*struct Window
	{
		bool enabled = true;
		// So width and height can be set as relative stuff
		float width;
		float height;


    void set_relative_width(float percentage);
		void set_relative_height(float percentage);

		void set_relative_x(float percentage);
		void set_relative_y(float percentage);
		private:

    };*/

	/*!
	 * @brief	The canvas class is simply here to holds frames that are displayed by the 
	 *			engine in screen space coordinate
	 */
	struct Canvas
	{
		std::vector<Frame> frames;
	};
}
