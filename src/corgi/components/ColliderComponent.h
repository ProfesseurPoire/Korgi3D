#pragma once

#include <corgi/ecs/Component.h>
#include <corgi/rendering/Mesh.h>

#include <memory>
#include <functional>
#include <vector>

namespace corgi
{
	class Entity;

	class ColliderComponent : public Component
	{
		friend class CollisionSystem;
		friend class Renderer;
		friend class Physic;

		class CollisionEvent
		{
		public:

			void operator+=(std::function<void(ColliderComponent&, ColliderComponent&, Entity&, Entity&)> callback)
			{
				_callbacks.push_back(callback);
			}

			void operator -=(int index)
			{
				_callbacks.erase(_callbacks.begin() + index);
			}

			void clear()
			{
				_callbacks.clear();
			}

			template<class ...T>
			void invoke(T&&... args)
			{
				for (auto& callback : _callbacks)
				{
					callback(args...);
				}
			}

			std::vector<std::function<void(ColliderComponent&, ColliderComponent&, Entity&, Entity&)>> _callbacks;
		};

public:

// Lifecycle

	ColliderComponent(Entity& entity);
	ColliderComponent(Entity* entity);

	ColliderComponent(std::vector<VertexAttribute> attributes):
		_mesh(std::make_shared<Mesh>(attributes))
		{
		}
		
	virtual ~ColliderComponent() = default;

// Functions
		// 5 

		bool colliding = false;			// 1
		char _current_layer	= 0;		// 1

		CollisionEvent on_enter;		// 16
		CollisionEvent on_exit;			// 16
		CollisionEvent on_collision;	// 16

		void layer(int layer_id);
		[[nodiscard]] int layer()const;

	protected :

		bool collide(ColliderComponent* col,Entity& a, Entity& entity);

		// TODO : I can probably just use a pointer here 
		std::shared_ptr<Mesh> _mesh;	// 8 	// So is this what fucks me up?

		//Store the normals of every triangle of the mesh
		std::vector<Vec3> normals;		// 16 
	};
}