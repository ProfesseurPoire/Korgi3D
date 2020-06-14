#include "CollisionSystem.h"

#include <corgi/components/ColliderComponent.h>

#include <corgi/ecs/ComponentPools.h>
#include <corgi/ecs/ComponentPool.h>

#include <corgi/ecs/Scene.h>

#include <corgi/utils/Physic.h>
#include "corgi/components/BoxCollider.h"
#include <corgi/ecs/Entity.h>

#include <vector>

using namespace corgi;

CollisionSystem::CollisionSystem(Scene& scene)
		: AbstractSystem(scene)
	{

	}

// Checks if the container collisions has a collision that feature
// the two collider provided as argument
static bool contains(const Vector<Collision>& collisions, 
				const ColliderComponent* first_collider, 
				const ColliderComponent* second_collider)
{
	for (auto& collision : collisions)
	{
		if ( (collision.colliderA == first_collider) &&
			(collision.colliderB == second_collider))
		{
			return true;
		}
	}
	return false;
}

const Vector<Collision>& CollisionSystem::collisions()const
{
	return _collisions;
}

const Vector<Collision>& CollisionSystem::enter_collisions()const
{
	return _enter_collisions;
}

const Vector<Collision>& CollisionSystem::exit_collisions()const
{
	return _exit_collisions;
}

	// TODO : could also just send the layers
	// and the scene
	void CollisionSystem::update()
	{
		_exit_collisions.clear();
		_enter_collisions.clear();

		std::vector<Collision> newCollisions;

		auto& collider_pool = *scene_.pools().get<BoxCollider>();

		for(auto& collider : collider_pool)
		{
			collider.second.colliding = false;
		}

		auto m = collider_pool.get_map();

		//   TODO  :  There's probably a way to improve this 
		// like this was the loop before   : 
		// for (size_t i = 0; i < collider_pool.size(); ++i)
		// {
		// 	for (size_t j = i+1; j < collider_pool.size(); j++)
		// 	{

		for(auto p : m)
		{
			for (auto p2 : m )
			{
				if(p.first==p2.first)
				{
					continue;
				}

				BoxCollider& first_collider		= *static_cast<BoxCollider*>(collider_pool.at(p.second));
				Entity& e1 = *scene_.entities_[p.first];

				BoxCollider& second_collider	= *static_cast<BoxCollider*>(collider_pool.at(p2.second));
				Entity& e2 = *scene_.entities_[p2.first];

				//  so how do I get the entity back here ...

				
				if( !first_collider.is_enabled() || !second_collider.is_enabled()||
					!e1.is_enabled() || !e2.is_enabled())
				{
					continue;
				}
				
				// Skip the collision detection if they are not on a compatible layer
				if (!Physic::layer_colliding(first_collider.layer(), second_collider.layer()))
				{
					continue;
				}

				// First we check if the 2 collider are actually in contact
				if (first_collider.collide(&second_collider, e2))
				{
					first_collider.colliding  = true;
					second_collider.colliding = true;
					// If they are, we check if the collision has already
					// been registered previously
					if (!contains(_collisions, &first_collider, &second_collider))
					{
						// If not, we add the collision to our collision list
						// we run the on_enter_ callbacks 
						_enter_collisions.push_back({&first_collider,  &second_collider, &e1, &e2});

						first_collider.on_enter.invoke(first_collider, second_collider, e1, e2);
						second_collider.on_enter.invoke(second_collider, first_collider, e2, e1);
					}

					// We register all the collision that occurred during this frame
					newCollisions.push_back({&first_collider, &second_collider, &e1, &e2});

					first_collider.on_collision.invoke(first_collider, second_collider, e1, e2);
					second_collider.on_collision.invoke(second_collider, first_collider, e2, e1);
				}
			}
		}

		// Now we compare the collisions that were registered this frame
		// with the collisions that were registered during the previous frame
		for (auto& collision : _collisions)
		{
			// If the collision doesn't exist anymore, then we fire
			// the exit collision callbacks
			if (!contains(newCollisions, collision.colliderA, collision.colliderB))
			{
				_exit_collisions.push_back(collision);

				collision.colliderA->on_exit.invoke
				(
					*collision.colliderA, 
					*collision.colliderB,
					*collision.entityA,
					*collision.entityB
				);

				collision.colliderB->on_exit.invoke
				(
					*collision.colliderB,
					*collision.colliderA,
					*collision.entityB,
					*collision.entityA
				);
			}
		}
		_collisions = newCollisions;
	}
