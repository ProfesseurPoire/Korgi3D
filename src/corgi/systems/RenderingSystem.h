#pragma once

#include <corgi/ecs/System.h>
#include <corgi/ecs/ComponentPool.h>
#include <corgi/components/RendererComponent.h>

#include <corgi/containers/Vector.h>

#include <vector>
#include <string>
#include <memory>
#include <string>
#include <typeindex>

namespace corgi
{
	class RenderingSystem : public AbstractSystem
	{
	public:

	// Lifecycle

		RenderingSystem(Scene& scene) :
			AbstractSystem(scene)
		{

		}
		
		Vector<std::pair<const RendererComponent*, Entity*>> renderer_components_;

		void add_pool(AbstractComponentPool* pool)
		{
			renderer_component_pools_.push_back(pool);
		}
		
	protected :

		// Really starting to wonder if this is actually a good idea, or if maybe
		// I should just add the components pools manually instead of trying
		// to trick the thing through this static crap 
		void on_new_component_pool(AbstractComponentPool* pool) override
		{
			// How could I use this here?
			//std::is_base_of<typeid(Component), Component>();
			//std::is_base_of(pool, pool);

			// Still a hack, but a less disturbing one at least 
			if (pool->type() == RendererComponent::type())
			{
				renderer_component_pools_.push_back(pool);
			}
		}

		void update() override
		{
			renderer_components_.clear();

			for(auto* pool : renderer_component_pools_)
			{
				std::map<int,int> m = pool->get_map();

				// TODO : still a slightly hacky way to get back the 
				// entity from a component in the pool but I think it works
				for(auto p : m)
				{
					int i =   p.second;

					auto& c = *pool->at(i);
					auto& e = *scene_.entities_[p.first];

					if( e.is_enabled())
					{
						RendererComponent* rc = static_cast<RendererComponent*>(&c);
						renderer_components_.push_back({rc,&e});
					}
				}
			}
		}

		Vector<AbstractComponentPool*> renderer_component_pools_;
	}; 
}