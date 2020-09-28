#include <corgi/ecs/Entity.h>
#include <corgi/ecs/Scene.h>

#include <algorithm>

namespace corgi
{
	static int unique_id_ = 0;

// Lifecycle

	Entity::Entity(Scene& scene, Entity* parent, const std::string& name) :
	scene_(scene),parent_(parent),name_(name), id_(unique_id_++)
	{
		if(parent_)
		{
			depth_ = parent_->depth() + 1;
		}
		//scene_.entities().register_entity(id_, *this);
	}

	Entity::Entity(Scene& scene, const std::string& name)
	: scene_(scene), name_(name), id_(unique_id_++)
	{
		//scene_.entities().register_entity(id_, *this);
	}

	Entity::Entity(const Entity& entity):
		scene_(entity.scene_),
		name_(entity.name_),
		id_(unique_id_++)
	{
		
		// Probably some stuff to do with the children too
	}

// Functions
	
	Entity* Entity::child(const std::string& name) noexcept
	{
		for (auto& child : children_)
		{
			if (child->name() == name)
			{
				return child.get();
			}
		}
		return nullptr;
	}

	// TODO : THis is broken too lol
	void Entity::copy(const Entity& e)
	{
		// parent_ = e.parent_;
		// enabled_ = e.enabled_;
		// current_layer_ = e.current_layer_;
		// name_ = e.name_;
		// //transform_._entity = this;

		// // It's either that or have a virtual function in Components so :eyes:
		// /*for(auto& component : e._components)
		// {
		// 	_components.emplace(component.first, component.second->clone(*this));
		// }*/

		// for (auto& child : e.children_)
		// {
		// 	children_.push_back(std::unique_ptr<Entity>(new Entity(*child)));
		// }
	}

	// TODO : THis is totally broken mdr
	void Entity::move(Entity&& e)noexcept
	{
		// parent_ = e.parent_;
		// tags = e.tags;
		// //_components			= std::move(e._components);
		// children_ = std::move(e.children_);
		// name_ = e.name_;
		// current_layer_ = e.current_layer_;
		// enabled_ = e.enabled_;
		// //transform_._entity	= this;

		// /*for(auto& component : _components)
		// {
		// 	component.second.get()->_entity = this;
		// }*/

		// e.parent_ = nullptr;
	}

	int Entity::id()const noexcept
	{
		return id_;
	}

	// Todo : update Entity for std::find and algorithms
	std::optional<std::reference_wrapper<Entity>> Entity::find(const std::string& name)const
	{
		for (auto& child : children_)
		{
			if (child->name() == name)
			{
				return *child;
			}
			auto v = child->find(name);
			if(v!=std::nullopt)
			{
				return v;
			}
		}
		return std::nullopt;
	}

	std::optional<std::reference_wrapper<Entity>> Entity::find(int id)const
	{
		for (auto& child : children_)
		{
			if (child->id() == id)
			{
				return *child;
			}
			auto v = child->find(id);
			if (v != std::nullopt)
			{
				return v;
			}
		}
		return std::nullopt;
	}

	/*Behavior* Entity::behavior()
	{
		for (auto& pool : scene_->pools())fin
		{
			if (pool.second->has_component(id_))
			{
				auto* c = pool.second.get()->at_id(id_);
				auto* b = dynamic_cast<Behavior*>(c);

				if (b)
				{
					return b;
				}
			}
		}
		return nullptr;
	}*/

	void Entity::remove_child(Entity* e)
	{
		children_.erase
		(
			std::remove_if
			(
				children_.begin(),
				children_.end(),
				[&](std::unique_ptr<Entity>& p)
				{
					return p.get() == e;
				}
			),
			children_.end()
					);
	}

	

	[[nodiscard]] const std::string& Entity::name()const
	{
		return name_;
	}

	int Entity::current_layer()const
	{
		return current_layer_;
	}

	void Entity::name(const std::string& n)
	{
		name_ = n;
	}

	void Entity::current_layer(int cl)
	{
		current_layer_ = cl;
	}

	bool Entity::is_enabled()const noexcept
	{
		return enabled_;
	}

	void Entity::is_enabled(bool v)
	{
		enabled_ = v;
	}

	Entity* Entity::add_child(Entity* e) noexcept
	{
		return children_.emplace_back(e).get();
	}

	Entity& Entity::add_child(const Entity& entity)
	{
		children_.emplace_back(new Entity(entity)); // first we copy
		children_.back()->parent(this); // then we set the parent
		return *children_.back();		// then we return the thing
	}

	Entity& Entity::add_child(Entity&& entity) noexcept
	{
		children_.emplace_back(new Entity(std::move(entity)));
		children_.back()->parent(this);
		return *children_.back();
	}

	Entity& Entity::add_child(const std::string& name) noexcept
	{
		auto entity = new Entity(scene_, this, name);
		scene_.entities().register_entity(entity->id(), *entity);
		return *children_.emplace_back(entity);
	}

	void Entity::remove_component(const std::type_info& component_type)
	{
		if (!scene_.pools().contains(component_type))
		{
			return;
		}

		if (!has_component(component_type))
		{
			return;
		}

		scene_.pools().get(component_type)->remove(id_);
	}
	
	bool Entity::has_component(const std::type_info& component_type) const
	{
		// We first check if there's a pool that could hold the component 
		if (!scene_.pools().contains(component_type))
		{
			return false;
		}

		return scene_.pools().get(component_type)->has_component(id_);
	}

	/*const Transform& Entity::transform() const
	{
		return get_component<Transform>();
	}

	Transform& Entity::transform()
	{
		return get_component<Transform>();
	}

	const SpriteRenderer& Entity::sprite_renderer() const
	{
		return get_component<SpriteRenderer>();
	}

	SpriteRenderer& Entity::sprite_renderer()
	{
		return get_component<SpriteRenderer>();
	}*/

	// Detach the current entity from its parent's children list
	void Entity::detach_from_parent()
	{
		auto it = std::find_if(
			parent_->children_.begin(),
			parent_->children_.end(),
			[&](std::unique_ptr<Entity>& p) { return p.get() == this; });

		// We release the pointer
		it->release();
		// We delete the unique_ptr
		parent_->children_.erase(it);
	}

	Entity* Entity::parent() noexcept
	{
		return parent_;
	}

	void Entity::update(Scene& scene)
	{
		if (!enabled_)
			return;

		//for (auto& component : _components)
		//		if(component.second->enabled)
				//component.second->update(scene);

		for (auto& child : children_)
			child->update(scene);
	}

	const std::vector<std::unique_ptr<Entity>>& Entity::children()const noexcept
	{
		return children_;
	}

	std::vector<std::unique_ptr<Entity>>& Entity::children() noexcept
	{
		return children_;
	}

	Scene& Entity::scene() noexcept
	{
		return scene_;
	}

	void Entity::parent(Entity* new_parent)
	{
		if (new_parent)
		{
			if (new_parent != parent_)
			{
				if (parent_)
					detach_from_parent();

				new_parent->add_child(this);
				parent_ = new_parent;
			}
		}
		else
		{
			if (parent_)
			{
				detach_from_parent();
			}
			
			parent_ = &scene().entities().root();
		}
	}

	void Entity::parent(Entity& new_parent)
	{
		parent(&new_parent);
	}

}
