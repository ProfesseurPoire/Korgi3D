#pragma once

#include <corgi/components/ColliderComponent.h>

namespace corgi
{
	class Entity;

// A collider needs transform component
// AABB stands for axis aligned bounding box
// Attach functions/lambdas to the exit, enter and collision callbacks

// The BoxCollider is actually building a new Mesh under the hood representing
// the actual Box. When the BoxCollider is destroyed, the associated mesh is 
// also automatically deleted
class BoxCollider : public ColliderComponent
{
	friend class CollisionSystem;
	friend class Renderer;
	friend class Physic;

public:

// Constructors

	BoxCollider();
	BoxCollider(int layer);
	BoxCollider(int layer, float width, float height, float depth);

//  Functions

	void enable();
	void disable();
	void dimensions(float width, float height, float depth);

	void width(float v);
	void height(float v);
	void depth(float v);

	// Contains the triangles that makes the collider
	void build_box();

	/*!
	 * @brief	Returns the width of the box collider
	 */
	[[nodiscard]] float width()const noexcept;
	[[nodiscard]] float height()const noexcept;
	[[nodiscard]] float depth()const noexcept;

	[[nodiscard]] bool is_enabled()const noexcept;

protected:

	float width_	{ 1.0f };
	float height_	{ 1.0f };
	float depth_	{ 1.0f };

	bool is_enabled_=true;
};

}