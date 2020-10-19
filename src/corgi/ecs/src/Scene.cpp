#include <corgi/ecs/Scene.h>
#include <corgi/ecs/Entity.h>

namespace corgi
{
	static Scene main_scene;
	
	Scene::Scene()
	{
		root_.reset(new Entity(this));
		root_->scene_ = this;
	}

	Scene::~Scene()
	{
	}

	Entity* Scene::find(const std::string& name)
	{
		return root_->find(name);
	}

	std::vector<AbstractSystem*>& Scene::systems()
	{
		return systems_;
	}

	void Scene::remove_entity(Entity& entity)
	{
		for(auto& component_pool : pools_)
		{
			component_pool.second->remove(entity.id_);
		}
	}

	void Scene::update(float elapsed_time)
	{
		for(auto* system : systems_)
		{
			system->update();
		}
		_elapsed_time = elapsed_time;
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

	Scene& Scene::main()
	{
		return main_scene;
	}
	
	Entity& Scene::new_entity(const std::string& name, EntityCreationFlag flags)
	{
		return main_scene.root_->add_child(name);
	}
	
	Entity& Scene::new_entity(const Entity& entity)
	{
		auto e = new Entity(entity);
		main_scene.root_->add_child(e);
		return *e;
	}

	Entity& Scene::append(const std::string& name)
	{
		return main_scene.new_entity(name);
	}

	Entity& Scene::append(Entity* entity)
	{
		main_scene.root_->add_child(entity);
		return *entity;
	}

	Entity& Scene::append(Entity&& entity)noexcept
	{
		return main_scene.root_->add_child(std::move(entity));
	}

	Entity& Scene::append(const Entity& entity)
	{
		return main_scene.root_->add_child(entity);
	}

	void Scene::clear()
	{
		main_scene.root_->children().clear();
	}
}
