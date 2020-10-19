#pragma once

#include <corgi/ecs/Component.h>

#include <corgi/rendering/Material.h>
#include <corgi/rendering/Mesh.h>
#include <memory>

namespace corgi
{
	class RendererComponent : public Component
	{
		friend class Renderer;
	public:

		static const std::type_info& type()
		{
			return typeid(RendererComponent);
		}

		RendererComponent(Entity& entity)
			: Component(entity){}

		RendererComponent(Entity* entity) : 
			Component(entity)
		{
			
		}

		RendererComponent(Entity& entity, Material* mat) :
			Component(entity),material(*mat){}

		RendererComponent(Entity* entity, Material* mat):
			Component(entity),material(*mat)
		{
			
		}


		Material material = Material("empty");

		// TODO : Not sure why I'm using a shared_ptr here
		std::shared_ptr<Mesh> _mesh;
	};
}