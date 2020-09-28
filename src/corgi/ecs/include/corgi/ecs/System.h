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

// Lifecycle
	
	AbstractSystem(Scene& scene);
	virtual ~AbstractSystem() = default;

protected :

// Virtual functions

	/*!
	 * @brief Will be called every time a new component pool is created by the ECS
	 */
	virtual void on_new_component_pool(AbstractComponentPool* pool){}

	virtual void before_update() {}
	virtual void update() {}
	virtual void after_update(){}

	Scene& scene_;
};
	
}