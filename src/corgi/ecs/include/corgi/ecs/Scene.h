#pragma once

#include <corgi/ecs/System.h>
#include <corgi/ecs/ComponentPool.h>
#include <corgi/ecs/ComponentPools.h>

#include <vector>
#include <memory>
#include <string>

namespace corgi
{
	class Entity;
	
	class Scene
	{
	public:

	// Constructors

		Scene();
		~Scene();
		
	// Functions

		template<class T>
		[[nodiscard]] T* system()
		{
			for(auto* sys : systems_)
			{
				if( dynamic_cast<T*>(sys))
				{
					return dynamic_cast<T*>(sys);
				}
			}
			return nullptr;
		}

		void remove_entity(Entity& entity);

		/*Entity* find_by_name(std::string name);
		Entity* find_by_tag(std::string tag);*/

		void update(float elapsed_time);

		/*
		 * @brief	This enum can be used to specify additional information on how to create
		 *			a new entity 
		 */
		enum class EntityCreationFlag
		{
			Default,
			NoTransform
		};

		/*!
		 * @brief	By default a transform component is automatically added to the entity
		 *			Use the EntityCreationFlag::NoTransform to creates one without transform
		 */
		static Entity& new_entity(const std::string& name = "NewEntity", 
			EntityCreationFlag = EntityCreationFlag::Default);

		/*!
		 * @brief	Creates a copy of the given entity and attach it to the scene
		 */
		static Entity& new_entity(const Entity& entity);

		/*!
		 * @brief	Appends a new Entity to the scene and returns a new reference to it
		 */
		static Entity& append(const std::string& name = "NewEntity");

		static Scene& main();

		// TODO :	Since there can be more than 1 scene I'm not sure I should
		//			keep the static functions
		static Entity& append(Entity* entity);
		static Entity& append(Entity&& entity)noexcept;
		static Entity& append(const Entity& entity);

		ComponentPools& pools()
		{
			return pools_;
		}

		/*!
		 * @brief	Removes every entity attached to the scene
		 */
		static void clear();

		/*!
		 * @brief	Tries to find an entity called "name" inside the scene
		 *			Returns a pointer to the entity if founded, returns nullptr 
		 *			otherwise
		 */
		Entity* find(const std::string& name);

		float elapsed_time()
		{
			return _elapsed_time;
		}

		Entity* root()
		{
			return  root_.get();
		}

		std::vector<AbstractSystem*>& systems();

		// There will be a copy anyways 
		/*void add_canvas(const Canvas& canvas);
		void add_canvas(Canvas&& canvas);*/
		//Canvas& new_canvas();

		float _time_step;
		float _elapsed_time = 0.0f;

		//std::vector<std::unique_ptr<Canvas>> canvas_;

	private:

		std::vector<AbstractSystem*> systems_;
		ComponentPools pools_ {systems_};
		std::unique_ptr<Entity> root_;
	};
}
