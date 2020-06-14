#pragma once

#include <corgi/containers/Vector.h>

#include <corgi/ecs/System.h>
#include <corgi/ecs/ComponentPool.h>

namespace corgi
{
class ColliderComponent;
class BoxCollider;
class Scene;

struct Collision
{
	ColliderComponent* colliderA;
	ColliderComponent* colliderB;
	Entity* entityA;
	Entity* entityB;
};

// Check the collisions for every collider in the game, and fire their callbacks
// functions
class CollisionSystem : public AbstractSystem
{
public:

	CollisionSystem(Scene& scene);

	// Scene to get back the colliders, Physic to get back
	// the collision layers
	void update() override;

	const Vector<Collision>& collisions()const;
	const Vector<Collision>& enter_collisions()const;
	const Vector<Collision>& exit_collisions()const;

private:

	Vector<Collision> _collisions;
	Vector<Collision> _enter_collisions;
	Vector<Collision> _exit_collisions;
};
}