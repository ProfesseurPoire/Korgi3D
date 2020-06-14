#include "MeshRenderer.h"

#include <corgi/utils/ResourcesLoader.h>

namespace corgi 
{
	MeshRenderer::MeshRenderer(Entity& entity)
		: RendererComponent(entity)
	{
		material = *ResourcesLoader::material("unlit_color.mat");
		material.is_transparent = false;
	}
}