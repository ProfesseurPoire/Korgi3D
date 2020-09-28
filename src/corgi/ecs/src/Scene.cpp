#include <corgi/ecs/Scene.h>

namespace corgi
{
	static Scene main_scene;

	Systems& Scene::systems() noexcept
	{
		return systems_;
	}

	const Systems& Scene::systems() const noexcept
	{
		return systems_;
	}

	void Scene::before_update(float elapsed_time)
	{
		_elapsed_time = elapsed_time;
		
		for(auto& system : systems_)
		{
			system->before_update();
		}
	}

	void Scene::update(float elapsed_time)
	{
		for(auto& system : systems_)
		{
			system->update();
		}
	}

	void Scene::after_update(float elapsed_time)
	{
		for(auto& system : systems_)
		{
			system->update();
		}
	}

	Scene& Scene::main()
	{
		return main_scene;
	}
}
