#pragma once

#include <functional>
#include <corgi/math/Vec2.h>
#include <memory>

struct GLFWwindow;

namespace corgi
{
	class Renderer;

    class Window
    {
    public:

	// Usings

		using ResizeWindowCallback	= std::function<void(int, int)>;
		using MoveCallback			= std::function<void(int, int)>;

	// Constructors

		Window(const char* title, int x, int y, int width, int height, int monitor, bool vsync);
        ~Window();

		// Just making sure you can't accidentally copy a window object
        Window(const Window&)				= delete;
        Window& operator=(const Window&)	= delete;

	// Functions

		/*!
		 * @brief	Returns the window's dimension (width and height)
		 */
		[[nodiscard]] Vec2i dimensions()const;

		/*!
		 * @brief	Returns the window's position
		 */
		[[nodiscard]] Vec2i position()const;

		/*!
		 * @brief	Returns the window's width
		 */
		[[nodiscard]] int width()const;

		/*!
		 * @brief	Returns the window's height
		 */
		[[nodiscard]] int height()const;

		/*!
		 * @brief	Returns the window's aspect ratio
		 */
		[[nodiscard]] float aspect_ratio()const;

		/*!
		 * @brief	Returns the window's top left x position relative to the screen
		 */
		[[nodiscard]] int x()const;

		/*!
		 * @brief	Returns the window's top left y position relative to the screen
		 */
		[[nodiscard]] int y()const;

		/*!
		 * @brief	Returns true if the window is currently using vsync 
		 *			V-Sync means opengl will only draw after a monitor refresh (avoids artefacts where the monitor
		 *			could be refreshed halfway while a frame was being constructed)
		 */
		[[nodiscard]] bool is_vsync()const;

		/*!
		 * @brief	Returns true when the window is currently in the process of closing, false otherwise
		 */
		[[nodiscard]] bool is_closing()const;

		/*!
		 * @brief	Returns a pointer to the actual GLFW window object, as a void*
		 *			TODO : Apparently it's something I'm using for Imgui. If I do use my own UI stuff, this might
		 *			get obsolote
		 */
		[[nodiscard]] void* underlying_window()const;

		/*!
		 * @brief	Sets the window's title
		 * @param title	String to display on the window's title
		 */
		void title(const char* title);

		/*!
		 * @brief	Turns on or off VSync
		 *			Vsync means opengl will only draw after a monitor refresh
		 */
		void vsync(bool value);

        void swap_buffers() const;
        void resize(int width, const int height);

        void add_resize_callback(std::shared_ptr<ResizeWindowCallback> delegate);
        void add_move_callback(std::shared_ptr<MoveCallback> delegate);

		void poll_events();

		/*!
		 * 	@brief 	Will close the window after being called, also destroying 
		 * 			whatever might
		 */
		void close();
		
	private:

	// Static functions

		/*static void on_resized_event(GLFWwindow*,	int, int);
		static void on_moved_event(GLFWwindow*,		int, int);
		static void on_keyboard_event(GLFWwindow*,	int key, int scancode, int action, int mods);
		static void on_mouse_move_event(GLFWwindow*, double x, double y);*/

		/*!
		 * @brief	Called by GLFW when a mouse button event occurs
		 * 
		 *			Will update Inputs::mouse_ object's state
		 */
		//static void on_mouse_button_event(GLFWwindow*, int button, int action, int mods);

	// Member Variables

		std::string	title_			{ "window" };
		int			x_				{ 0 };
		int			y_				{ 0 };
		int			width_			{ 0 };
		int			height_			{ 0 };
		int			monitor_count_	{ 0 };

		void*	window_				{ nullptr };

		bool	_running			{false};
		bool	vsync_				{false};	
		int		monitor_			{-1};		// if -1, it means the application is windowed
		
		// There is the option to share opengl context between windows but I'm not
		// sure how to actually do that so 
		bool	_shared			{false};

    	bool is_closing_ = false;

		std::vector<std::weak_ptr<std::function<void(int, int)>>>  _resize_callbacks;
		std::vector<std::weak_ptr<std::function<void(int, int)>>>  _move_callbacks;

	// Functions

		bool initialize_callbacks();
		bool create_window();

		friend class Game;
    };
}