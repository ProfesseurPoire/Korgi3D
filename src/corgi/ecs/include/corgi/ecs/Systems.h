#pragma once

#include <corgi/ecs/System.h>
#include <corgi/containers/TypeMap.h>
#include <concepts>

namespace corgi
{

template<typename T>
concept IsSystem = std::is_base_of<AbstractSystem, T>::value;

class ComponentPools;

class Systems
{
public:

// Lifecycle

    Systems(Scene& scene) : scene_(scene){}

// Methods

    template<class T, class ... Args>
    void emplace_back(Args&& ... params)
    {
        // This line here is basically the only reason I'm making this class
        systems_.add<T>(scene_, std::forward<Args>(params)...);
        sys_.push_back(systems_.get<T>());
    }

    template<class T>
    [[nodiscard]] T& get(){return *systems_.get<T>();}

    void clear() noexcept { systems_.clear();}

    template<class T>
    [[nodiscard]] constexpr bool contains() const noexcept
    {
        return systems_.contains(typeid(T));
    }

    auto begin(){return sys_.begin();}
    auto end(){return sys_.end();}

private:

    // Member variables

    Scene& scene_;
    containers::TypeMap<AbstractSystem> systems_;
	// I'm using this std::vector because the map will reorder things
	// and I need a specific order for system execution (I need transform to be updated
	// before the other systems for instance
    std::vector<AbstractSystem*> sys_;
};
}