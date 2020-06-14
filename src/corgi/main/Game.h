#pragma once

#include <corgi/ecs/Scene.h>
#include <corgi/main/Window.h>

#include <corgi/rendering/renderer.h>

#include <memory>
#include <string>
#include <functional>

namespace corgi
{
	class Scene;
	class ResourcesLoader;
	class Window;

	// Since there can only be 1 game running at the time, this is sort of the "master" class
	// that owns everything. I'm making everything static so it's a bit easier to access stuff
	// from the outside (though it should be kept minimal, prefer dependency injection whenever possible)

	// You need to create an initialize the game before using anything in this framework.
	// The Game::initialize() function will initialize everything needed for the game
	// like the Resources loading mechanism 
	class Game
	{
	public:

	// Lifecycle
		
		// I'm actually considering making Game an object so the construction
		// thing is automatic. Would probably feels better? Like you just
		// start your main by creating a game object, and once it goes out
		// of scope, it will gets deleted

		//TODO : I'm not really hyped about this way of handling the 
		// 		resource directory
		// This exists mostly because I fetch both theses strings by using
		// a custom .h ... but I'm not really sure about this mecanism
		// and it does feel a bit weird to have a game that is a bit 
		// heavy on the modern cpp features, while still having some really
		// C features. Like I'm totally fine with free functions, but when
		// it comes down to having "initialize" and "finalize" functions,
		// this should ring a bell and tell me not to do it that way
		// so well, how can I make this slightly less worse?
		
		// TODO : 	This should actually be removed once I have a real resource
		// 			manage that convert every existing resource inside our resource
		//			folder into 1 big or multiple binary files close to the
		//			executable
		// 			I even wonder if maybe this could be directly be embeded inside
		//			the executable. Like by having every resource inside a stream
		//			built in a .h file? 
		static void initialize
		(
			const std::string& project_resource_directory,
			const std::string& library_resource_directory
		);


		Game::Game();
		Game::~Game();

		void finalize();

		static void initialize_window(const std::string& name,
			int x, int y, int width, int height, int fullscreen, bool vsync);

		static void update_uis();
		static void update_inputs();

		[[nodiscard]] static float time_step();
		static void time_step(float value);

		static inline void(*update_)();

		[[nodiscard]] static Scene&		scene();
		[[nodiscard]] static Window&	window();
		[[nodiscard]] static Renderer&	renderer();

		// Means this function will lock the thread executing it
		[[noreturn]] static void run();

		static void refresh_settings();

	private:

		static inline bool step_by_step_ = false;
		static inline bool next_step_		= false;
		static inline float time_step_ = 1.0f/60.0f;

		static inline std::unique_ptr<Window>	window_;
		static inline std::unique_ptr<Renderer>	renderer_;
		
		std::unique_ptr<Scene>	scene_;
	};
}