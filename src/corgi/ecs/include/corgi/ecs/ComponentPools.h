#pragma once

#include <corgi/ecs/System.h>
#include <corgi/ecs/ComponentPool.h>

#include <map>
#include <vector>
#include <memory>
#include <typeindex>

namespace corgi
{
	class ComponentPools
	{
	public:

		ComponentPools(std::vector<AbstractSystem*>& sys)
			: systems_(sys){}

		/*!
		 * @brief	Returns a non owning pointer to the component pool of type T.
		 *			Returns nullptr if no pool of type T is stored inside the container
		 *	
		 */
		template<class T>
		[[nodiscard]] ComponentPool<T>* get()
		{
			if (!exists<T>())
			{
				return nullptr;
			}
			return dynamic_cast<ComponentPool<T>*>(pools_.at(typeid(T)).get());
		}

		[[nodiscard]] AbstractComponentPool* get(const std::type_info& component_type);

		/*!
		 * @brief	Will try to add a new component pool of the specified type.
		 *			If a component pool of the same type already exists, it won't
		 *			construct it 
		 */
		template<class T>
		void add(int size=0)
		{
			if(exists<T>())
			{
				return;
			}
			
			auto it = pools_.emplace(typeid(T), new ComponentPool<T>(size));

			// Register the new component_pool to systems
			// Not sure it's really needed though
			for (auto* system : systems_)
			{
				system->on_new_component_pool( it.first->second.get());
			}
		}

		template<class T>
		void remove(){remove(typeid(T));}

		template<class T>
		[[nodiscard]] bool exists() const { return exists(typeid(T)); }

		
		void remove(const std::type_info& component_type);
		[[nodiscard]] bool exists(const std::type_info& component_type)const;

		auto begin()
		{
			return pools_.begin();
		}

		auto end()
		{
			return pools_.end();
		}

		[[nodiscard]] int size() const noexcept;

	private:

		std::map<std::type_index, std::unique_ptr<AbstractComponentPool>> pools_;
		
		std::vector<AbstractSystem*>& systems_;
	};
}