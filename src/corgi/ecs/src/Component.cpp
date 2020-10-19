#include <corgi/ecs/Component.h>
#include <corgi/ecs/Entity.h>

namespace corgi
{
	Component::Component(Entity* entity)
		: _entity(entity){}

	Component::~Component() {}

	const Entity& Component::entity()const
	{
		return *_entity;
	}

	bool Component::is_enabled() const
	{
		return is_enabled_;
	}

	void Component::is_enabled(bool value)
	{
		is_enabled_ = value;
	}

	int Component::id() const
	{
		return _entity->id();
	}

	Entity& Component::entity()
	{
		return *_entity;
	}
}
