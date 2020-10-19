#include "ColliderComponent.h"

#include <corgi/math/Collisions.h>
#include <corgi/ecs/Entity.h>
#include <corgi/components/Transform.h>

#include <assert.h>

namespace corgi
{

	ColliderComponent::ColliderComponent(Entity& entity)
		: Component(entity) {}

	ColliderComponent::ColliderComponent(Entity* entity)
	{
	}

	void ColliderComponent::layer(int layer_id)
	{
		// Can't have a layer > 64, since I use a 64 bit variable
		// as my collision layer mask
		assert(layer_id < 64 && layer_id >= 0);
		_current_layer =  layer_id;
	}

	int ColliderComponent::layer() const
	{
		return _current_layer;
	}

	bool ColliderComponent::collide(ColliderComponent* col, Entity& entity)
	{
		return math::intersect_3D
		(
			
			entity.get_component<Transform>().world_matrix(),
			_mesh->indexes(),
			_mesh->vertices().data(),
			3,
			0,
			_mesh->vertex_count(),
			entity.get_component<Transform>().world_matrix(),
			col->_mesh->vertices().data(),
			col->_mesh->indexes(),
			3,
			0,
			col->_mesh->vertex_count()
		);
	}
}