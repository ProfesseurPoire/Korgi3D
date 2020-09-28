#pragma once

#include <corgi/ecs/Scene.h>

#include <string>
#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <functional>

#include <corgi/logger/log.h>

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

		/*!
		 * @brief Creates a new empty entity
		 */
		explicit Entity(Scene& scene, const std::string& name = "Unnamed");
		explicit Entity(Scene& scene, Entity* parent, const std::string& name = "Unnamed");

		explicit Entity(const Entity& entity);
		

		~Entity() = default;

		/*Entity(Scene& scene, Entity&& entity)noexcept;
		Entity(Scene& scene, const Entity& entity);

		Entity& operator=(const Entity& entity);
		Entity& operator=(Entity&& entity)noexcept;*/

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
		std::optional<std::reference_wrapper<Entity>> find(const std::string& name)const;
		std::optional<std::reference_wrapper<Entity>> find(int id)const;

		/*!
		 * @brief	Should return a behavior attached to the entity
		 */
		//Behavior* behavior();

		/*!
		 * @brief	Update all the components owned by the entity and its children
		 */
		void update(Scene& scene);


		template<class T>
		void check_if_pool_exist()
		{
			if (!scene_.pools().contains<T>())
			{
				scene_.pools().add<T>();
			}
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
			check_if_pool_exist<T>();

			auto& e = component.entity();

			auto& pool = scene_.pools().get<T>()->get();
			
			if(pool.has_component(id_))
			{
				auto* comp = dynamic_cast<T*>(pool.at_id(id_));
				(*comp) = std::move(component);
				comp->_entity = this;
			}
			else
			{
				scene_.pools().get<T>()->add(id_, std::move(component));
			}
			e.remove_component<T>();
		}

		template<class T>
		void copy_component(const T& component)
		{
			check_if_pool_exist<T>();

			if (scene_.pools().get<T>()->has_component(id_))
			{
				auto* comp = dynamic_cast<T*>(scene_.pools().get<T>()->at_id(id_));
				(*comp) = component;
				comp->_entity = this;
			}
			else
			{
				scene_.pools().get<T>()->add(id_, component);
			}
		}

		template<class T, class ... Args>
		T& add_component(Args&& ... args)
		{
			if (!scene_.pools().contains<T>())
			{
				scene_.pools().add<T>();
			}

			auto& pool = scene_.pools().get<T>()->get();

			// We then check if the pool already has a component of type T 
			if (pool.has_component(id()))
			{
				log_warning("The entity already has a component of type T associated with it");
				return pool.get(id());
			}

			// Then we simply forward the parameters to the add_param function of our pool
			return pool.add_param(*this, std::forward<Args>(args)...);
		}

		template<class T>
		T& add_component(const T& copy)
		{
			check_if_pool_exist<T>();

			auto& component = scene_.pools().get<T>()->add(id_, copy);
			component._entity = this;
			return component;
		}

		template<class T>
		T& add_component(T&& copy)
		{
			check_if_pool_exist<T>();

			auto& entity = copy.entity();
			
			auto& component = scene_.pools().get<T>()->add(id_, std::move(copy));
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
		std::optional<std::reference_wrapper<T>> get_component()
		{
			if (!scene_.pools().contains<T>())
			{
				return std::nullopt;
			}
			auto& pool = scene_.pools().get<T>()->get();
			if (!pool.has_component(id_))
			{
				return std::nullopt;
			}
			return pool.get(id_);
		}

		// TODO : Maybe use a macro to be able to get the component name?
		//#define get_c(type_name) \
		//get_component<type_name>(#type_name)
		//
		/*template<class T>
		std::optional<std::reference_wrapper<T>> get_component(const std::string& type_name)
		{
			if (!scene_.pools().contains<T>())
			{
				log_warning("No component pool of type "+ type_name);
				return std::nullopt;
			}

			auto& pool = scene_.pools().get<T>()->get();

			if (!pool.has_component(id_))
			{
				return std::nullopt;
			}

			return pool.get(id_);
		}*/

		void remove_component(const std::type_info& component_type);

		template<class T>
		void remove_component()
		{
			remove_component(typeid(T));
		}

		[[nodiscard]] int depth()const
		{
			return depth_;
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

		int depth_ = 0;
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

		Scene& scene_;
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
