#pragma once

#include <corgi/components/RendererComponent.h>

namespace corgi
{
	class Texture;
	class Mesh;

    class MeshRenderer : public RendererComponent
	{
    public:

		MeshRenderer(Entity& entity);
	};
}