#include "Window.h"

#include  <SDL.h>

#include <glad/glad.h>
#include <iostream>
#include <algorithm>

#include "corgi/inputs/Inputs.h"

namespace corgi
{
	Window::~Window()
	{
		SDL_Quit();
	}
	
    Window::Window
	(
		const char* title,
		int x, int y,
		int width, int height,
		int monitor, 
		bool vsync
	)
        : x_(x), y_(y), width_(width), height_(height)
	{
        window_  =  nullptr;        // Declare a pointer

        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);              // Initialize SDL2
		
        // Create an application window with the following settings:
		window_ = SDL_CreateWindow
		(
            title,				// window title
            x,				// initial x position
            y,				// initial y position
            width,                       // width, in pixels
            height,                      // height, in pixels
            SDL_WINDOW_OPENGL            // flags - see below
        );

        // Check that the window was successfully created
        if (!window_)
        {
            printf("Could not create window: %s\n", SDL_GetError());
        }

		// CREATE THE OPENGL CONTEXT AND MAKE IT CURRENT:
		auto gl_context = SDL_GL_CreateContext((SDL_Window*)( window_));


		SDL_GL_MakeCurrent((SDL_Window*)(window_), gl_context);

		// INITIALIZE GLAD:
		
		gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);


		//SDL_Joystick
		
	}

	Vec2i Window::dimensions() const
	{
        return Vec2i(width_, height_);
	}

	int Window::height() const
	{
        return height_;
	}

	int Window::width()const
    {
        return width_;
    }

	float Window::aspect_ratio() const
	{
        return float(width_) / float(height_);
	}

	int Window::x() const
	{
        return x_;
	}

	int Window::y() const
	{
        return height_;
	}

    bool Window::is_vsync() const
    {
        return vsync_;
    }

	void Window::title(const char* title)
	{
        title_ = title;
		SDL_SetWindowTitle(static_cast<SDL_Window*>(window_), title);
	}

	void Window::vsync(bool value)
	{
        vsync_ = value;
	}

	bool Window::is_closing() const
	{
        return is_closing_;
	}

	Vec2i Window::position() const
	{
        return Vec2i(x_, y_);
	}

	void Window::close()
	{
		is_closing_=true;
	}

	void Window::swap_buffers() const
	{
		SDL_GL_SwapWindow(static_cast<SDL_Window*>(window_));
	}

	void* Window::underlying_window() const
	{
		return window_;
	}

	void Window::poll_events()
	{
		SDL_Event e;

		float val = 0.0f;

		while (SDL_PollEvent(&e)) 
		{
			switch (e.type)
			{
			case SDL_KEYDOWN:
				
				Inputs::keyboard_._keys_down[e.key.keysym.scancode]	= true;
				Inputs::keyboard_._keys[e.key.keysym.scancode]		= true;
				
				break;
				
			case SDL_KEYUP:

				Inputs::keyboard_._keys[e.key.keysym.scancode] = false;
				Inputs::keyboard_._keys_up[e.key.keysym.scancode] = true;
				
				break;

			case SDL_JOYDEVICEADDED:
				
				Inputs::gamepads_.emplace_back(e.jdevice.which);
				break;

			case SDL_JOYDEVICEREMOVED:

				Inputs::gamepads_.erase
				(
					std::remove_if
					(
						Inputs::gamepads_.begin(),
						Inputs::gamepads_.end(),
						[&](const Gamepad& gamepad)
						{
							return gamepad.id_ == e.jdevice.which;
						}
					)
				);
				break;
				
			case SDL_JOYAXISMOTION:

				if(e.jaxis.value>=0)
				{
					val = (float)e.jaxis.value / (float)32767;
				}
				else
				{
					val = (float)e.jaxis.value / (float)32768;
				}
				
				Inputs::gamepads_[e.jaxis.which].axes_[e.jaxis.axis] = val;

				break;

			case SDL_JOYBUTTONDOWN :

				Inputs::gamepads_[e.jbutton.which].buttons_[e.jbutton.button] = true;
				Inputs::gamepads_[e.jbutton.which].buttons_down_[e.jbutton.button] = true;
				
				break;

			case SDL_JOYBUTTONUP:

				Inputs::gamepads_[e.jbutton.which].buttons_up_[e.jbutton.button] = true;
				Inputs::gamepads_[e.jbutton.which].buttons_[e.jbutton.button] = false;

				break;

			case SDL_JOYHATMOTION:

				switch (e.jhat.value)
				{
					case SDL_HAT_LEFTUP:
						Inputs::gamepads_[e.jhat.which].hat_ = Gamepad::Hat::LeftUp;
						break;
					case SDL_HAT_UP :
						Inputs::gamepads_[e.jhat.which].hat_ = Gamepad::Hat::Up;
						break;
					case SDL_HAT_RIGHTUP:
						Inputs::gamepads_[e.jhat.which].hat_ = Gamepad::Hat::RightUp;
						break;
					case SDL_HAT_LEFT:
						Inputs::gamepads_[e.jhat.which].hat_ = Gamepad::Hat::Left;
						break;
					case SDL_HAT_CENTERED:
						Inputs::gamepads_[e.jhat.which].hat_ = Gamepad::Hat::Centered;
						break;
					case SDL_HAT_RIGHT:
						Inputs::gamepads_[e.jhat.which].hat_ = Gamepad::Hat::Right;
						break;
					case SDL_HAT_LEFTDOWN:
						Inputs::gamepads_[e.jhat.which].hat_ = Gamepad::Hat::LeftDown;
						break;
					case SDL_HAT_DOWN:
						Inputs::gamepads_[e.jhat.which].hat_ = Gamepad::Hat::Down;
						break;
					case SDL_HAT_RIGHTDOWN:
						Inputs::gamepads_[e.jhat.which].hat_ = Gamepad::Hat::RightDown;
						break;
				}

				break;
				
			case SDL_QUIT:

				is_closing_ = true;
				break;
			}
		}
	}
}
