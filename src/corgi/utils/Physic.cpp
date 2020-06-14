#include "Physic.h"

#include <corgi/math/Ray.h>
#include <corgi/math/Collisions.h>

#include <corgi/components/BoxCollider.h>
#include <corgi/components/Transform.h>

#include <corgi/ecs/Entity.h>
#include <corgi/main/Game.h>

#include <limits>

namespace corgi
{
	RaycastResult::operator bool()const
	{
		return collision_occured;
	}

	void Physic::set_collision(int layer1, int layer2)
	{
		layers[layer1] |= int_64(1) << layer2;
		layers[layer2] |= int_64(1) << layer1;
	}

	bool Physic::layer_colliding(int layer1, int layer2)
	{
		return (layers[layer1] & ( int_64(1) << layer2)) != 0;
	}

	const RaycastResult Physic::raycast(const Ray& ray, int_64 layer_flag)
	{
		return raycast(ray.start, ray.direction, ray.length, layer_flag);
	}

	const RaycastResult Physic::raycast(const Vec3& start, const Vec3& direction, float length, int_64 layer)
	{
		RaycastResult result;
		Ray ray(start, direction, length);

		result.ray = ray;

		// Get the colliders
		Vec3 intersection_point;
		Vec3 intersection_normal;

		float min_length = std::numeric_limits<float>::max();
		
		for (auto& collider : *Game::scene().pools().get<BoxCollider>())
		{
			auto& entity  = *Game::scene().entities_[collider.first];

			if(! collider.second.is_enabled() || !entity.is_enabled() )
			{
				continue;
			}
			
			int_64 v = 1;

			if ((( v << (collider.second._current_layer))& layer) != 0)
			{
				auto mesh = collider.second._mesh;
				
				if (
					math::intersect_with_collider
					(
						0,
						3, 
						entity.get_component<Transform>().world_matrix(),
						entity.get_component<Transform>().world_matrix().inverse(),
						mesh->indexes(),
						mesh->vertices().data(),
						collider.second.normals,
						ray,
						intersection_point, intersection_normal
					)
				)
				{
					if ((intersection_point - start).length() < min_length)
					{
						min_length = (intersection_point - start).length();
						result.collision_occured	= true;
						result.intersection_point	= intersection_point;
						result.collider				= &collider.second;
						result.intersection_normal	= intersection_normal;
						result.entity				= &entity;
					}
				}
			}
		}
		return result;
	}
}