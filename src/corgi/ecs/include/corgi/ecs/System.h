#pragma once

namespace corgi
{

	class AbstractComponentPool;
	
	/*!
	 * @brief	New Systems will have to inherit from this class and implement its
	 *			virtual functions
	 */
	class AbstractSystem
	{
	public:

		friend class Scene;
		friend class ComponentPools;

		AbstractSystem() = default;
		virtual ~AbstractSystem() {}

	protected :

		/*!
		 * @brief Will be called everytime a new component pool is created by the ECS
		 */
		virtual void on_new_component_pool(AbstractComponentPool* pool){}

		virtual void update() = 0;
	};
}