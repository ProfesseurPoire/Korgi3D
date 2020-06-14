#pragma once

#include <corgi/ecs/System.h>
#include <corgi/ecs/ComponentPool.h>

#include <corgi/components/ParticleEmitter.h>

namespace corgi
{
	class ParticleEmitterSystem : public AbstractSystem
	{
	public:

		ParticleEmitterSystem(Scene& scene) : 
			AbstractSystem(scene)
		{

		}

		void update()override
		{
			/*for (auto& a : components_)
			{
				if (a.is_enabled())
				{
					a.update();
				}
			}*/
		}
	};
}