#pragma once

#include <corgi/ecs/Component.h>
#include <corgi/ecs/Scene.h>
#include <corgi/ecs/Layer.h>

#include <corgi/containers/Vector.h>

#include <corgi/string/String.h>

#include <corgi/memory/UniquePtr.h>

#include <corgi/logger/log.h>

#include <typeindex>
#include <algorithm>
#include <optional>
#include <functional>

namespace corgi
{
/*!
 * @brief	An entity is an object that is used to modelize a behavior by 
 * 			combining basic building blocks called components. For instance, 
 * 			a character entity would need a Transform component, 
 * 			to know its position in world space, a SpriteRenderer component, 
 * 			to display the character's sprite, an Animator component, to call
 * 			and display 2D animations, a StateMachine component, to changed the 
 * 			state of the character depending on specific inputs and events, and 
 * 			a Character component, that could contain thing like health and mana
 * 			points.
 * 
 * 			Underneath, an entity is basically just an integer identifier that 
 * 			is used by the component pools to identifies which component is 
 * 			attached to which entity through a bimap 
 * 			(EntityId -> ComponentIndex, ComponentIndex -> EntityId)
 *		
 *			Entities can be organized in a tree fashion, so it also keeps 
 *			references to its children and parent objects (if any)
 *
 *			Since component pools are stored inside a scene object, you need to
 * 			give a reference to an existing one when creating an entity so it
 * 			"knows" where to store the components that you attach to it.
 */
class Entity
{
	friend class Scene;
	friend class Game;
	friend class Renderer;
	friend class Physic;

public:

	static inline int unique_id_ = 0;

// Lifecycle

	/*!
	 * @brief 	Constructs a new entity attached to @ascene that copies the content of @a entity
	 */
	Entity(Scene& scene, Entity& entity):
		scene_(scene),
		id_(unique_id_++)
	{
		// will need to copy @a entity components
	}

	/*!
	 * @brief 	Constructs a new entity attached to @a scene that move the content of @a entity
	 */
	Entity(Scene& scene, Entity&& entity):
		scene_(scene),
		id_(unique_id_++)
	{
		// Will need to move @a entity components
	}

	Entity(Scene& scene):
		id_(unique_id_++),
		scene_(scene)
	{
		//?
		//scene_->entities_.try_emplace(id_, this);
	}

	// TODO : An entity should always reference a scene. So I don't really like the following constructors
	// The problem lies in the fact that components vectors are initialized inside the scene object, and the entity needs to find them
	//
	// General idea being : If I need to load/initialize a new scene, I need my entity to be link to the scene they are going to be
	// a part of, so you should really not be able to just create a new entity without referencing the scene
	// that's also why you can do stuff like scene.new_entity("name");

	Entity(const String& name, Entity* parent = nullptr) : 
		parent_(parent), 
		name_(name), 
		id_(unique_id_++)
	{
		
	}
	Entity(Entity* parent, const String& name):
		parent_(parent),
		name_(name),
		id_(unique_id_++)
	{
		scene_ = parent->scene_;
		//scene_->entities_.try_emplace(id_, this);
	}

	Entity(Entity* parent, String&& name): parent_(parent), name_(name)
	{
		scene_ = parent->scene_;
		//scene_->entities_.try_emplace(id_, this);
	}


	Entity(Entity&& entity)noexcept
	{
		move(std::move(entity));
	}

	Entity(const Entity& entity)
	{
		copy(entity);
	}

	Entity& operator=(const Entity& entity)
	{
		copy(entity);
		return *this;
	}

	Entity& operator=(Entity&& entity)noexcept
	{
		move(std::move(entity));
		return *this;
	}

	~Entity()
	{
		//scene_->remove_entity(*this);
	}

// Functions

	/*!
		* @brief	Returns the entity's unique identifier
		*/
	[[nodiscard]] int id()const noexcept
	{
		return id_;
	}

	/*!
		* @brief	Returns the entity's parent. If the entity has no parent, returns nullptr instead
		*/
	[[nodiscard]] Entity* parent() noexcept
	{
		return parent_;
	}

	[[nodiscard]] const Entity* parent()const noexcept
	{
		return parent_;
	}

	[[nodiscard]] Scene& scene()noexcept
	{
		return *scene_;
	}

	void parent(Entity* new_parent)
	{
		if (!new_parent)
		{
			if (parent_)
			{
				detach_from_parent();
			}

			// TODO : Check this out, I'm hacking a bit here
			parent_ = scene().entities().
			scene().root()->add_child(this);
		}
		else
		{
			if (new_parent != parent_)
			{
				if (parent_)
					detach_from_parent();

				new_parent->add_child(this);
				parent_ = new_parent;
			}
		}
	}
	void parent(Entity& new_parent)
	{
		parent(&new_parent);
	}

	/*!
		* @brief	Tries to remove the entity passed as a parameter from the current entity children list
		*/
	void remove_child(Entity* e)
	{
		children_.erase
		(
			std::remove_if
			(
				children_.begin(),
				children_.end(),
				[&](UniquePtr<Entity>& p)
				{
					return p.get() == e;
				}
			),
			children_.end()
		);
	}

	/*!
		* @brief	Returns the entity's name
		*/
	[[nodiscard]] const String& name()const
	{
		return name_;
	}

	void name(const String& n)
	{
		name_ = n;
	}

	[[nodiscard]] Layer current_layer()const
	{
		return Layer(static_cast<int>(layer_));
	}

	void current_layer(Layer layer)
	{
		layer_ = static_cast<int>(layer.value());
	}

	/*!
		* @brief	Returns true if the entity is currently enabled
		*			A disabled entity will not update its components and ignore by the systems
		*/
	[[nodiscard]] bool is_enabled()const noexcept
	{
		return enabled_;
	}

	/*!
		* 	@brief 	Delete the entity's child and their components		
		*/
	void clear()
	{
		children_.clear();	
	}

	void is_enabled(bool v)
	{
		enabled_ = v;
	}

	/*!
		* @brief	Tries to find an entity called "name" in the entity's children. Non recursive
		*			Returns a pointer to requested entity. Returns nullptr if no children called "name" exists
		*/
	[[nodiscard]] Entity* child(const String& name) noexcept
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

	/*!
		* @brief	Returns the entity's children list as a const reference
		*/
	[[nodiscard]] const Vector<UniquePtr<Entity>>& children()const noexcept
	{
		return children_;
	}

	/*!
		* @brief	Returns the entity's children list as a reference
		*/
					
	[[nodiscard]] Vector<UniquePtr<Entity>>& children() noexcept
	{
		return children_;
	}

	/*!
		* @brief	Creates a new empty entity with the given name and add it to the entity's children list
		*			Returns a reference to the newly created entity
		*/
	Entity& add_child(const String& name = "Entity") noexcept
	{
		auto entity = new Entity(this, name);
		entity->depth_= depth_+1;
		entity->scene_ = this->scene_;
		return *children_.emplace_back(entity);
	}

	/*!
		* @brief	Makes a copy of the entity given as a parameter and process to add it as a child
		*			Returns a pointer to the newly created entity. If the given entity was equal to nullptr
		*			the function will return nullptr
		*/
	Entity* add_child(Entity* e) noexcept
	{
		return children_.emplace_back(e).get();
	}

	/*!
	 * @brief	Makes a copy of the entity given as a parameter and proceed to add it as a child
	 */
	Entity& add_child(const Entity& entity)
	{
		children_.emplace_back(new Entity(entity)); // first we copy
		children_.back()->parent(this); // then we set the parent
		return *children_.back();		// then we return the thing
	}

	/*!
		* @brief	Move the entity given as a parameter as one of the current entity children
		*/
	Entity& add_child(Entity&& entity)noexcept
	{
		children_.emplace_back(new Entity(std::move(entity)));
		children_.back()->parent(this);
		return *children_.back();
	}

	//std::optional<std::reference_wrapper<Entity>> find(const String& n);
	//Entity* find(const String& n)
	/*!
		* @brief	Recursively tries to find an entity with the same name as the parameter
		*/
	[[nodiscard]]std::optional<std::reference_wrapper<Entity>> find(const String& n) noexcept
	{
		for (auto& child : children_)
		{
			if (child->name() == n)
			{
				return *child.get();
			}

			const auto r = child->find(n);
			if (r)
			{
				return r;
			}
		}
		log_warning("Could not find a children called "+n+" in "+name());
		return std::nullopt;
	}

/*Behavior* Entity::behavior()
{
	for (auto& pool : scene_->pools())
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
	/*!
		* @brief	Should return a behavior attached to the entity
		*/
	//Behavior* behavior();

	// TODO : Delete that once everything uses a system

	/*!
		* @brief	Update all the components owned by the entity and its children
		*/
	void update(Scene& scene)
	{
		if (!enabled_)
			return;

		//for (auto& component : _components)
		//		if(component.second->enabled)
		//component.second->update(scene);

		for (auto& child : children_)
			child->update(scene);
	}

	

	template<class T>
	T& add_component(T&& copy)
	{
		if (!scene_->get().pools().contains<T>())
		{
			scene_->get().pools().add<T>();
		}
		return scene_->get().pools().get<T>()->add(id_, std::move(copy));
	}

	template<class T, class ...Args>
	T& add_component(Args&& ... args)
	{
		// TODO : Would there be a way to avoid making this check
		// everytime I add a component? Maybe also making it so we can add
		// multiple component at the same time?
		
		// We first check if a pool that can stores components of type T exist 
		if (!scene_->get().pools().contains<T>())
		{
			scene_->get().pools().add<T>();	// if not, we add a new pool to the scene
		}

		if(scene_->get().pools().get<T>()->has_component(id()))
		{
			log_warning("The entity already has a component of type T associated with it");
			return scene_->get().pools().get<T>()->get(id());
		}

		// Then we simply forward the parameters to the add_param function of our pool
		return scene_->get().pools().get<T>()->add_param(*this, std::forward<Args>(args)...);
	}

	/*
		* @brief	Checks if the entity stores a component of the given
		*			template parameter type
		*
		* @return	Returns true if the component is stored, false otherwise
		*/
	template<class T>
	[[nodiscard]] bool has_component() const{return has_component(typeid(T));}
	[[nodiscard]] bool has_component(const std::type_info& component_type)const
	{
		// We first check if there's a pool that could hold the component 
		if (!scene_->get().pools().contains(component_type))
		{
			return false;
		}
		return scene_->get().pools().get(component_type)->has_component(id_);
	}

	template<class T>
	T& get_component()  // Still const since technically we don't change the entity directly 
	{
		if (!scene_->get().pools().contains<T>())
		{
			//log_error("Boom");
			throw;
		}

		auto& pool = *scene_->get().pools().get<T>();

		if (!pool.has_component(id_))
		{
			//log_error("Boom");
			throw;
		}

		return pool.get(id_);
	}

	template<class T>
	const T& get_component()const
	{
		//check_template_argument<T>();

		if (!scene_->pools().contains<T>())
		{
			// Todo : maybe map the typeid to a string so I can easily 
			// log which component failed?
			//log_error("Could not get a Component Pool");
			throw;
		}

		auto& pool = *scene_->pools().get<T>();

		if (!pool.has_component(id_))
		{
			//log_error("Boom");
			throw;
		}

		return pool.get(id_);
	}

	void remove_component(const std::type_info& component_type)
	{
		if (!scene_->get().pools().contains(component_type))
		{
			return;
		}

		if (!has_component(component_type))
		{
			return;
		}

		scene_->get().pools().get(component_type)->remove(id_);
	}

	template<class T>
	void remove_component()
	{
		//check_template_argument<T>();
		remove_component(typeid(T));
	}

	void enable()
	{
		enabled_=true;
	}

	void disable()
	{
		enabled_=false;
	}


	[[nodiscard]] std::vector<std::string>& tags()noexcept
	{
		return tags_;
	}

	[[nodiscad]] const std::vector<std::string>& tags()const noexcept
	{
		return tags_;
	}

	/*!
		*	@brief	Returns the transform component attached to the entity.
		*			TODO : Probably return a * since this can not work
		*/
	/*const Transform& transform() const;
	Transform& transform();

	const SpriteRenderer& sprite_renderer()const;
	SpriteRenderer& sprite_renderer();*/

	[[nodiscard]] int depth()const
	{
		return depth_;
	}

private:

	// Detach the current entity from its parent's children list
	void detach_from_parent()
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

	template<class T>
	void check_template_argument()
	{
		static_assert(std::is_base_of<T, Component>(),"Template argument must inherit from Component.");
	}

	// Variables 

	int id_;
	int depth_=0;

	// This is optional in case I want/need to be able
	// to copy or move an entity
	// though if I copy an entity, this reference would probably
	// be problematic, since I wouldn't want to actually copy that
	// from, so, maybe this is wrong?
	// Actually, nonsensical, I can't add component or do
	// anything really if something hasn't already a scene
	// Maybe I could still have a sort of "prefab" scene
	// and then copy stuff from that?
	// I guess I should probably desactivate default
	// constructor then, as I don't want to be able to
	// copy construct an entity?
	std::optional<std::reference_wrapper<Scene>> scene_;	

	Entity* parent_ = nullptr;

	Vector<String> tags_;
	Vector<UniquePtr<Entity>> children_;

	String name_;

	char layer_ = 0;
	bool enabled_ = true;

	void copy(const Entity& e)
	{
		parent_ = e.parent_;
		enabled_ = e.enabled_;
		layer_ = e.layer_;
		name_ = e.name_;
	//transform_._entity = this;

	// It's either that or have a virtual function in Components so :eyes:
	/*for(auto& component : e._components)
	{
		_components.emplace(component.first, component.second->clone(*this));
	}*/

	for (auto& child : e.children_)
	{
		children_.push_back(UniquePtr<Entity>(new Entity(*child)));
	}
	}

	void move(Entity&& e)noexcept
	{
		parent_ = e.parent_;
		tags_ = e.tags_;
		//_components			= std::move(e._components);
		children_ = std::move(e.children_);
		name_ = e.name_;
		layer_ = e.layer_;
		enabled_ = e.enabled_;
		//transform_._entity	= this;

		/*for(auto& component : _components)
		{
			component.second.get()->_entity = this;
		}*/

		e.parent_ = nullptr;
	}
};
}
