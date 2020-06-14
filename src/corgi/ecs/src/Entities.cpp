#include <corgi/ecs/Entities.h>
#include <corgi/ecs/Entity.h>

using namespace corgi;

Entities::Entities(Scene& scene):
	scene_(scene),
    root_(std::make_unique<Entity>(scene))
{
	
}

Entity& Entities::emplace(const String& name)noexcept
{
    return root_->add_child(name);
}

Entity& Entities::emplace(Entity&& entity)noexcept
{
    return root_->add_child(std::move(entity));
}

Entity& Entities::emplace(const Entity& entity)noexcept
{
    return root_->add_child(entity);
}

Optional<Reference<Entity>> Entities::find(const String& name) noexcept
{
    return root_->find(name);
}

Optional<Reference<const Entity>> Entities::find(const String& name)const noexcept
{
    return root_->find(name);
}

Optional<Reference<const Entity>> Entities::operator[](int id) const noexcept
{
    if(entities_.contains(id))
    {
        return entities_.at(id);
    }
    return std::nullopt;
}

Optional<Reference<Entity>> Entities::operator[](int id) noexcept
{
    if(entities_.contains(id))
    {
        return entities_[id];
    }
    return std::nullopt;
}