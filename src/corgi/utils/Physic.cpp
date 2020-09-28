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

	// TODO : Return std::optional since the thing can actually fail
	const RaycastResult Physic::raycast(const Vec3& start, const Vec3& direction, float length, int_64 layer)
	{
		RaycastResult result;
		Ray ray(start, direction, length);

		result.ray = ray;

		// Get the colliders
		Vec3 intersection_point;
		Vec3 intersection_normal;

		float min_length = std::numeric_limits<float>::max();

		auto opt_collider_pool = Game::scene().pools().get<BoxCollider>();

		if(!opt_collider_pool)
		{
			log_error("No Box Collider Pool");
			return RaycastResult();
		}

		auto& collider_pool = opt_collider_pool->get();
		
		for (auto& collider_pair : collider_pool)
		{
			auto& collider_component = collider_pair.second;
			
			auto opt_entity  = Game::scene().entities()[collider_pair.first];

			if(!opt_entity)
			{
				log_error("Could not find an entity attached to the Collider component");
				continue;
			}

			auto& collider_entity = opt_entity->get();

			if(! collider_component.is_enabled() || !collider_entity.is_enabled() )
			{
				continue;
			}
			
			int_64 v = 1;

			if ((( v << (collider_component._current_layer))& layer) != 0)
			{
				// TODO : Shared_ptr? WTF?
				auto mesh = collider_component._mesh;

				auto opt_transform = collider_entity.get_component<Transform>();

				if (opt_transform)
				{

					// TODO : I actually can find the inverse matrix directly
					// inside the function btw KEK
					if (math::intersect_with_collider
					(
						0,
						3,
						opt_transform->get().world_matrix(),
									opt_transform->get().world_matrix().inverse(),
						mesh->indexes(),
						mesh->vertices().data(),
						collider_component.normals,
						ray,
						intersection_point, intersection_normal
					)
						)
					{
						if ((intersection_point - start).length() < min_length)
						{
							min_length = (intersection_point - start).length();
							result.collision_occured = true;
							result.intersection_point = intersection_point;
							result.collider = &collider_component;
							result.intersection_normal = intersection_normal;
							result.entity = &collider_entity;
						}
					}
				}
				else
				{
					log_error("You shit just add a transform");
				}
			}
		}
		return result;
	}
}