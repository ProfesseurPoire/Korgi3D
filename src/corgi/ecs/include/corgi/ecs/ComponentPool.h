#pragma once

#include <vector>
#include <map>
#include <typeindex>
#include <algorithm>

namespace corgi
{
	class Component;
	
	class AbstractComponentPool
	{
	public:

		friend class ComponentPools;
		
		
		AbstractComponentPool() = default;
		virtual ~AbstractComponentPool() = default;

		virtual const Component* pop() const= 0;

		virtual void remove(int entity_id)=0;

		std::type_index type_ = typeid(int);	// default type

		// only here because I need a virtual function
		virtual void useless_crap_of_doom() {}

		virtual int size()const=0;
		virtual const Component* at(int index) const = 0;
		virtual Component* at_id(int entity_id) = 0;
		virtual Component* at(int index) = 0;
		virtual bool has_component(int entity_id)const = 0;
	};

	template<class T>
	class Ref;

	template<class T>
	class ComponentPool : public  AbstractComponentPool
	{
	public:

		friend class Entity;
		
		void remove(int entity) override
		{
			const int index = entity_id_to_components_[entity];

			components_[entity_id_to_components_[entity]]._entity = nullptr;
			components_.erase
			(
				components_.begin() +
				entity_id_to_components_[entity]
			);

			// Then we need to iterate on every EntityId-Index 
			// Every component that has an index superior to the one we removed
			// must be substracted by one
			for (auto& map_item : entity_id_to_components_)
			{
				if (map_item.second > index)
				{
					--map_item.second;
				}
			}

			entity_id_to_components_.erase(entity);
		}
		
		int size() const override
		{
			return components_.size();
		}
		
		const Component* at(int index) const override
		{
			return dynamic_cast<const Component*>(&components_[index]);
		};

		Component* at(int index) override
		{
			return dynamic_cast<Component*>(&components_[index]);
		};
		
		Component* at_id(int entity_id) override
		{
			return dynamic_cast<Component*>(&components_[ entity_id_to_components_.at(entity_id)]);
		}
		
		const Component* pop() const override
		{
			return new T();
		}

		ComponentPool(int size=0)
		{
			components_.reserve(size);
		}

		T& add(int id, T&& comp)
		{
			components_.emplace_back(std::move(comp));
			entity_id_to_components_.emplace(id, components_.size() - 1);
			type_ = typeid(T);
			return components_.back();
		}

		T& add(int id, const T& component)
		{
			components_.emplace_back(component);
			entity_id_to_components_.emplace(id, components_.size() - 1);
			type_ = typeid(T);
			return components_.back();
		}
		
		T& add(int id)
		{
			components_.emplace_back();
			entity_id_to_components_.emplace(id, components_.size() - 1);
			type_ = typeid(T);
			return components_.back();
		}

		template<class ... Args>
		T& add_param(int id, Args ...args)
		{
			components_.emplace_back(std::forward<Args>(args)...);
			entity_id_to_components_.emplace(id, components_.size() - 1);
			type_ = typeid(T);
			return components_.back();
		}

		T& get(int entity_id)
		{
			return components_[entity_id_to_components_.at(entity_id)];
		}
		
		Ref<T> get_ref(int entity_id)
		{
			return Ref<T>(*this, entity_id);
		}

		bool has_component(int entity_id)const override
		{
			return (entity_id_to_components_.count(entity_id) != 0);
		}

		auto begin()
		{
			return components_.begin();
		}

		auto end()
		{
			return components_.end();
		}

		[[nodiscard]] auto begin()const
		{
			return components_.cbegin();
		}

		[[nodiscard]] auto end()const
		{
			return components_.cend();
		}

		[[nodiscard]] const std::map<int,int>& entity_id_to_components()const noexcept
		{
			return entity_id_to_components_;
		}

		[[nodiscard]] const std::vector<T>& components()const noexcept
		{
			return components_;
		}
		

	private:

		std::map<int, int> entity_id_to_components_;
		std::vector<T> components_;

	};


	template<class T>
	class Ref
	{
	public:

		T* operator->()
		{
			return &pool_.get(entity_id_);
		}
		
	private:

		friend class ComponentPool<T>;

		Ref(ComponentPool<T>& pool, int entity_id)
			:pool_(pool),entity_id_(entity_id){}

		ComponentPool<T>& pool_;
		int entity_id_;
	};
}