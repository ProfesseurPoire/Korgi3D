#pragma once

#include <stack>

namespace corgi
{
	class Entity;

	/*!
	 * @brief	The goal of the entity pool is to avoid unnecessary memory
	 *			allocation by creating a fixed number of Entity and reusing them once. This
	 *			is especially useful for things like particles or projectiles.
	 */
	class EntityPool
	{
	public:

	// Lifecycle

		/*!
		 * @brief	Entities created by the entity pool will inherit from the given parameter,
		 *			and the entity pool will create x stuff
		 */
		EntityPool(Entity* parent=nullptr);
		~EntityPool();

	//  Functions
		
		/*!
		 * @brief	Returns an entity from the pool. If no entity is available
		 *			a new one is created
		 */
		Entity* pop();
		void push(Entity* e);

	private:

		std::stack<Entity*> stack_;
		Entity* parent_;
	};
}