#pragma once

#include <corgi/ecs/Component.h>
#include <corgi/ecs/Scene.h>

#include <string>
#include <vector>
#include <memory>
#include <string>
#include <typeindex>

namespace corgi
{
	/*
	 * @brief	An entity is basically just an ID that components attaches too
	 *			Whenever a new Entity is created, it is given a unique identifier (up to 2 billions)
	 */
	class Entity
	{
		friend class Scene;
		friend class Game;
		friend class Renderer;
		friend class Physic;

	public:

		// Lifecycle

		Entity(Scene* scene);

		// TODO : An entity should always reference a scene. So I don't really like the following constructors
		// The problem lies in the fact that components vectors are initialized inside the scene object, and the entity needs to find them
		//
		// General idea being : If I need to load/initialize a new scene, I need my entity to be link to the scene they are going to be
		// a part of, so you should really not be able to just create a new entity without referencing the scene
		// that's also why you can do stuff like scene.new_entity("name");

		Entity(const std::string& name, Entity* parent = nullptr);
		Entity(Entity* parent, const std::string& name);
		Entity(Entity* parent, std::string&& name);

		~Entity();

		//Copy/Move constructors/operators

		Entity(Entity&& entity)noexcept;
		Entity(const Entity& entity);

		Entity& operator=(const Entity& entity);
		Entity& operator=(Entity&& entity)noexcept;

		// Functions

			/*!
			 * @brief	Returns the entity's unique identifier
			 */
		[[nodiscard]] int id()const noexcept;

		/*!
		 * @brief	Returns the entity's parent. If the entity has no parent, returns nullptr instead
		 */
		[[nodiscard]] Entity* parent() noexcept;
		[[nodiscard]] const Entity* parent()const noexcept;


		[[nodiscard]] Scene& scene()noexcept;

		void parent(Entity* new_parent);
		void parent(Entity& new_parent);

		/*!
		 * @brief	Tries to remove the entity passed as a parameter from the current entity children list
		 */
		void remove_child(Entity* e);

		/*!
		 * @brief	Returns the entity's name
		 */
		[[nodiscard]] const std::string& name()const;
		void name(const std::string& n);

		// TODO : not sure this is actually really use atm, since it's mostly colliders that uses a layer
		[[nodiscard]] int current_layer()const;
		void current_layer(int cl);

		/*!
		 * @brief	Returns true if the entity is currently enabled
		 *			A disabled entity will not update its components and ignore by the systems
		 */
		[[nodiscard]] bool is_enabled()const noexcept;

		void is_enabled(bool v);

		/*!
		 * @brief	Tries to find an entity called "name" in the entity's children. Non recursive
		 *			Returns a pointer to requested entity. Returns nullptr if no children called "name" exists
		 */
		[[nodiscard]] Entity* child(const std::string& name) noexcept;

		/*!
		 * @brief	Returns the entity's children list as a const reference
		 */
		[[nodiscard]] const std::vector<std::unique_ptr<Entity>>& children()const noexcept;

		/*!
		 * @brief	Returns the entity's children list as a reference
		 */
		[[nodiscard]] std::vector<std::unique_ptr<Entity>>& children() noexcept;

		/*!
		 * @brief	Creates a new empty entity with the given name and add it to the entity's children list
		 *			Returns a reference to the newly created entity
		 */
		Entity& add_child(const std::string& name = "Entity") noexcept;

		/*!
		 * @brief	Makes a copy of the entity given as a parameter and process to add it as a child
		 *			Returns a pointer to the newly created entity. If the given entity was equal to nullptr
		 *			the function will return nullptr
		 */
		Entity* add_child(Entity* e) noexcept;

		/*!
		 * @brief	Makes a copy of the entity given as a parameter and proceed to add it as a child
		 */
		Entity& add_child(const Entity& entity);

		/*!
		 * @brief	Move the entity given as a parameter as one of the current entity children
		 */
		Entity& add_child(Entity&& entity)noexcept;

		//Recursively tries to find an entity with the same name as the parameter
		Entity* find(const std::string& name);

		/*!
		 * @brief	Should return a behavior attached to the entity
		 */
		//Behavior* behavior();

		/*!
		 * @brief	Update all the components owned by the entity and its children
		 */
		void update(Scene& scene);

		/*!
		 * @brief	Returns an "actual" reference to a component. Since the container storing
		 *			the components can reallocate memory, using the Ref object is the only way
		 *			to keep a reference to a specific component after reallocation
		 */
		template<class T>
		Ref<T> get_ref()
		{
			if (!scene_->pools().exists<T>())
			{
				//return false;
			}

			return scene_->pools().get<T>()->get_ref(id_);
		}

		/*!
		 * @brief	2 things can happen here.
		 *			1) Entity already has a component of the same type attached
		 *			If so, what happens is we move the component given as a parameter
		 *			and remove it from the pool it used to be a part of
		 *			2) Entity doesn't have a component of the same type attached
		 *			-> Calls the Move constructor 
		 */
		template<class T>
		void move_component(T&& component)
		{
			if(!scene_->pools().exists<T>())
			{
				scene_->pools().add<T>();
			}

			auto& e = component.entity();
			
			if(scene_->pools().get<T>()->has_component(id_))
			{
				auto* comp = dynamic_cast<T*>(scene_->pools().get<T>()->at_id(id_));
				(*comp) = std::move(component);
				comp->_entity = this;
			}
			else
			{
				scene_->pools().get<T>()->add(id_, std::move(component));
			}
			e.remove_component<T>();
		}

		template<class T>
		void copy_component(const T& component)
		{
			if (!scene_->pools().exists<T>())
			{
				scene_->pools().add<T>();
			}

			if (scene_->pools().get<T>()->has_component(id_))
			{
				auto* comp = dynamic_cast<T*>(scene_->pools().get<T>()->at_id(id_));
				(*comp) = component;
				comp->_entity = this;
			}
			else
			{
				scene_->pools().get<T>()->add(id_, component);
			}
		}

		template<class T, class ...Args>
		T& add_component(Args ... args)
		{
			// TODO :	This could be removed if we assume the user did initialized all the component pools
			//			when creating the scene 
			if (!scene_->pools().exists<T>())
			{
				scene_->pools().add<T>();
			}

			auto& component = scene_->pools().get<T>()->add_param(id_, std::forward<Args>(args)...);
			component._entity = this;
			return component;
		}
		
		template<class T>
		T& add_component()
		{
			// TODO :	This could be removed if we assume the user did initialized all the component pools
			//			when creating the scene 
			if (!scene_->pools().exists<T>())
			{
				scene_->pools().add<T>();
			}

			auto& component = scene_->pools().get<T>()->add(id_);
			component._entity = this;
			return component;
		}

		template<class T>
		T& add_component(const T& copy)
		{
			if (!scene_->pools().exists<T>())
			{
				scene_->pools().add<T>();
			}


			auto& component = scene_->pools().get<T>()->add(id_, copy);
			component._entity = this;
			return component;
		}

		template<class T>
		T& add_component(T&& copy)
		{
			if (!scene_->pools().exists<T>())
			{
				scene_->pools().add<T>();
			}

			auto& entity = copy.entity();
			
			auto& component = scene_->pools().get<T>()->add(id_, std::move(copy));
			component._entity = this;

			entity.remove_component<T>();
			return component;
		}

		/*
		 * @brief	Checks if the entity stores a component of the given
		 *			template parameter type
		 *
		 * @return	Returns true if the component is stored, false otherwise
		 */
		template<class T>
		[[nodiscard]] bool has_component() const{return has_component(typeid(T));}
		[[nodiscard]] bool has_component(const std::type_info& component_type)const;

		template<class T>
		T& get_component()
		{
			if (!scene_->pools().exists<T>())
			{
				//log_error("Boom");
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

		

		template<class T>
		const T& get_component()const
		{
			if (!scene_->pools().exists<T>())
			{
				//log_error("Boom");
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

		void remove_component(const std::type_info& component_type);

		template<class T>
		void remove_component()
		{
			remove_component(typeid(T));
		}

		/*!
		 *	@brief	Returns the transform component attached to the entity.
		 *			TODO : Probably return a * since this can not work
		 */
		/*const Transform& transform() const;
		Transform& transform();

		const SpriteRenderer& sprite_renderer()const;
		SpriteRenderer& sprite_renderer();*/

	private:

		// Detach the current entity from its parent's children list
		void detach_from_parent();

		template<class T>
		void check_template_argument()
		{
			/*static_assert(std::is_default_constructible<T>(),
						  "Template argument must be default constructible.");*/

						  /*static_assert(std::is_base_of<T, Component>(),
										"Template argument must inherit from Components.");*/
		}

		// Variables 

		int id_;

		Scene* scene_ = nullptr;	// We need the reference to the scene to actually delete the entity and its attached components
									// Although to be fair I could act as if there was only 1 scene and directly fetch it

		Entity* parent_ = nullptr;

		std::vector<std::string> tags;
		std::vector<std::unique_ptr<Entity>> children_;

		std::string name_;

		char current_layer_ = 0;
		bool enabled_ = true;

		void copy(const Entity& e);
		void move(Entity&& e)noexcept;
	};
}
