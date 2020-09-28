#include <corgi/ecs/Entities.h>
#include <corgi/ecs/Entity.h>

using namespace corgi;

Entities::Entities(Scene& scene) :
    scene_(scene),
    root_(std::make_unique<Entity>(scene))
{

}

Entities::~Entities(){}


Entity& Entities::emplace(const String& name)noexcept
{
    auto& e =  root_->add_child(name);
    entities_.emplace(e.id(), e);
    return e;
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
    return root().find(name);
}

Optional<Reference<const Entity>> Entities::find(const String& name)const noexcept
{
	// TODO : Implement this
	// Preferably by using std::find, which means I need to set
	// the iterators for STL algorithms
    return std::nullopt;
}

Optional<Reference<const Entity>> Entities::operator[](int id) const noexcept
{
    return entities_.at(id);
}

Optional<Reference<Entity>> Entities::operator[](int id) noexcept
{
    return entities_.at(id);
}