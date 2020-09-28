#pragma once

#include <typeindex>

namespace corgi
{
	class Entity;
	class Scene;

/*!
 * @brief Components are data object attached to an entity and updated by the systems
 */
class Component
{
	friend class Entity;

public:

// Lifecycle

	Component(Entity* entity = nullptr);
	Component(Entity& entity);
	virtual ~Component() = default;

// Functions


	// TODO :	this is hacky and basically only exist because I have a mechanism
	//			where systems are made aware of new component pool on the fly
	//			solution : manually add the component pools instead of having
	//			this?
	static const std::type_info& type()
	{
		return typeid(Component);
	}
};
}