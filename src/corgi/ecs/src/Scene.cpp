#include <corgi/ecs/Scene.h>
#include <corgi/ecs/Entity.h>

namespace corgi
{
	//std::vector<std::unique_ptr<Canvas>>& Scene::canvas()
	//{
	//	return canvas_;
	//}

	// void Scene::remove_entity(Entity& entity)
	// {
	// 	for(auto& component_pool : pools_)
	// 	{
	// 		component_pool.second->remove(entity.id_);
	// 	}
	// }

	void Scene::update(float elapsed_time)
	{
		for(auto& system : systems_)
		{
			system.second->update();
		}
		_elapsed_time = elapsed_time;
	}

	void Scene::before_update(float elapsed_time)
	{
		for(auto& system : systems_)
		{
			system.second->before_update();
		}
	}

	void Scene::after_update(float elapsed_time)
	{
		for(auto& system: systems_)
		{
			system.second->after_update();	
		}
	}

	/*Canvas& Scene::new_canvas()
	{
		return *canvas_.emplace_back(std::make_unique<Canvas>()).get();
	}

	void Scene::add_canvas(Canvas&& canvas)
	{
		canvas_.push_back(std::make_unique<Canvas>(canvas));
	}

	void Scene::add_canvas(const Canvas& canvas)
	{
		canvas_.push_back(std::make_unique<Canvas>(canvas));
	}*/
	
	

	Entity& Scene::new_entity(const std::string& name)
	{
		return root_->add_child(name);
	}
	
	Entity& Scene::new_entity(const Entity& entity)
	{
		auto e = new Entity(entity);
		root_->add_child(e);
		return *e;
	}

	void Scene::clear()
	{
		if(root_) 	// There might not be any root. Btw I wonder if maybe I should just have a std::vector<Entity>
					// instead of that. 
		{
			root_->clear();
		}
		
		pools().clear();
		systems().clear();
	}
}