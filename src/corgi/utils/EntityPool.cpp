#include "EntityPool.h"

#include <corgi/ecs/Entity.h>
#include <corgi/ecs/Scene.h>
#include <corgi/main/Game.h>

namespace corgi
{
	EntityPool::EntityPool(Entity* parent)
	{
		parent_ = &Game::scene().new_entity("EntityPool");
	}

	EntityPool::~EntityPool()
	{
		delete parent_;
	}

	Entity* EntityPool::pop()
	{
		if(stack_.empty())
		{
			return &parent_->add_child("EntityFromPool");
		}
		else
		{
			auto e = stack_.top();
			stack_.pop();
			return e;
		}
	}

	void EntityPool::push(Entity* e)
	{
		stack_.push(e);
	}
}
