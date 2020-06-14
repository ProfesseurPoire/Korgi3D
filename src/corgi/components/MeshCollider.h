#pragma once

#include <corgi/components/ColliderComponent.h>

#include <corgi/rendering/Mesh.h>

#include <memory>

namespace corgi
{
	// The Mesh Collider Component use a convex mesh to check for collisions
	// The class will assume that there is a position attribute inside the mesh
	// which can be retrived and used by the algorithms to check for collisions
	//
	// Warning : The MeshCollider thing doesn't own the mesh. So if you build a mesh
	// specially for this mesh collider, make sure you delete the mesh afterwards
	class MeshCollider : public ColliderComponent
	{
	public:
		
		// TODO : Change that for just a pointer later on
		void mesh(std::shared_ptr<Mesh> m);
	};
}