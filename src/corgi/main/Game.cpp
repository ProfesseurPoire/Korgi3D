#include "Game.h"

#include <corgi/inputs/Inputs.h>

#include <corgi/utils/ResourcesLoader.h>
#include <corgi/utils/TimeHelper.h>

#include <corgi/main/Settings.h>

#include <corgi/main/AudioPlayer.h>

#include <glad/glad.h>

#include <sstream>
#include <iostream>

// handle errors

namespace corgi
{
	static Time		game_time;	// Time elapsed since the start of the game 

	// So there can be only one instance of Game
	static Game *instance_ {nullptr};

	void Game::initialize
	(	
		const std::string& project_resource_directory, 
		const std::string& library_resource_directory
	)
	{
		ResourcesLoader::add_directory(project_resource_directory);
		ResourcesLoader::add_directory(library_resource_directory);
		Settings::initialize(project_resource_directory + "/Settings.ini");
	}

	Game::Game()
	{
		instance_ = this;
	}

	Game::~Game()
	{
		
	}

	void Game::update_inputs()
	{
		Inputs::update();
		window().poll_events();
		
		/*if(Inputs::mouse_.has_moved())
		{
			if(Inputs::mouse_.is_button_pressed(Mouse::Button::Left))
			{
				for(auto& e : Inputs::mouse_left_drag_events_)
				{
					e(Mouse::DragEvent(
						Inputs::mouse().x() - Inputs::mouse().previous_x(),
						Inputs::mouse().y() - Inputs::mouse().previous_y(),
						Mouse::Button::Left
					));
				}
			}
			if(Inputs::mouse().is_button_pressed(Mouse::Button::Right))
			{
				for(auto& e : Inputs::mouse_right_drag_events_)
				{
					e(Mouse::DragEvent(
						Inputs::mouse().x() - Inputs::mouse().previous_x(),
						Inputs::mouse().y() - Inputs::mouse().previous_y(),
						Mouse::Button::Right
					));
				}
			}
			if(Inputs::mouse().is_button_pressed(Mouse::Button::Middle))
			{
				for(auto& e : Inputs::mouse_middle_drag_events_)
				{
					e(Mouse::DragEvent(
						Inputs::mouse().x() - Inputs::mouse().previous_x(),
						Inputs::mouse().y() - Inputs::mouse().previous_y(),
						Mouse::Button::Middle
					));
				}
			}
		}*/
	}

	float Game::time_step()
	{
		return time_step_;
	}

	void Game::time_step(float value)
	{
		time_step_ = value;
	}

	void Game::update_uis()
	{
		/*	for(auto& canvas : scene_->canvas_)
		{
			
		}*/
	}

	Renderer& Game::renderer()
	{
		return *renderer_;
	}

	void Game::initialize_window(const std::string& name,
		int x, int y, int width, int height, int fullscreen, bool vsync)
	{
		window_ = std::make_unique<Window>(name.c_str(), x, y, width, height, fullscreen, vsync);

		renderer_ = std::make_unique<Renderer>(*window_);
		AudioPlayer::initialize();

		// Only after the window and the GLADLoad thing we can start using opengl stuff
		// So now we actually load our resources. Resources often needs the renderer 
		ResourcesLoader::initialize(*renderer_);
		instance_->scene_ = std::make_unique<Scene>();
	}

	Scene& Game::scene()
	{
		return *instance_->scene_.get();
	}

	Window& Game::window()
	{
		return *window_;
	}

	std::string to_string(float value, int precision)
	{
		std::ostringstream out;
		out.precision(precision);
		out << std::fixed << value;
		return out.str();
	}

	// TODO: Profiler will probably needs to poke into the renderer for more information about
	//	what's happening during the game 
	struct Profiler
	{
		Counter update_counter_;
		Counter renderer_counter_;
		Counter loop_counter_;
		float refresh_rate_ = 0.35f;

		void reset()
		{
			loop_counter_.reset();
			renderer_counter_.reset();
			update_counter_.reset();
		}

		void update(Window& window)
		{
			// We only refresh the Profiler information every x seconds
			if (loop_counter_.elapsedTime() > refresh_rate_)
			{
				const auto update_time	= to_string(update_counter_.elapsedTime() * 1000.0f / renderer_counter_.ticks(), 2);
				const auto fps_value		= to_string(loop_counter_.tickPerSecond(), 2);
				const auto render_time	= to_string(
					renderer_counter_.elapsedTime() * 1000.0f / renderer_counter_.ticks(),	// This could also be done by the game no?
					2);

				reset();
				std::string fps = "FPS : " + fps_value + " Render Time " + render_time + " Update " + update_time;
				//window.title(fps.c_str());
			}
		}
	};

	void Game::finalize()
	{
		scene_.reset();
		Inputs::finalize();
		ResourcesLoader::finalize();
		AudioPlayer::finalize();
		renderer_.reset();
		window_.reset();
	}

	void Game::run()
	{
		Profiler profiler;

		game_time.timestep(time_step_);
		game_time.start();

		while (!window().is_closing())
		{
			profiler.loop_counter_.start();
			profiler.update(window());
			/*
			if (Inputs::keyboard().key_down(Key::T))
			{
				step_by_step_ = !step_by_step_;
			}

			if (Inputs::keyboard().key_down(Key::Y))
			{
				next_step_ = true;
			}*/
			
			
			// TODO : 
			// Might looks a bit weird if there's 2 fixed update in one turn 
			// Maybe I should move some stuff into a fixedUpdate and Update function?

			/*if (step_by_step_)
			{
				if (next_step_)
				{
					renderer().window_draw_list().clear();
					renderer().world_draw_list().clear();

					profiler.update_counter_.start();

					update_();
					scene().update(time_step_);

					profiler.update_counter_.tick();

					next_step_ = false;
				}
			}
			else
			{*/
			while (game_time.timestep_overrun())
			{
				renderer().window_draw_list().clear();
				renderer().world_draw_list().clear();

				profiler.update_counter_.start();

				update_inputs();
				scene().before_update(time_step_);
				update_();
				scene().update(time_step_);
				scene().after_update(time_step_);
				profiler.update_counter_.tick();
			}
			//}

			// Not sure if I should pack swap_buffer with it or not
			profiler.renderer_counter_.start();
			renderer().draw();
			profiler.renderer_counter_.tick();

			window().swap_buffers();
			profiler.loop_counter_.tick();
		}
	}

	void Game::refresh_settings()
	{
		Settings::refresh();
	}
}
