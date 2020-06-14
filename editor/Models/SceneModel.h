#pragma once

#include <typeindex>

#include <corgi/containers/List.h>

namespace corgi
{
    class Entity;
}

namespace Editor
{
    struct EntityModel;

    struct ComponentModel
    {
        ComponentModel()
            :component(typeid(int)) {}

        ComponentModel(std::type_index i) : component(i) {}
        std::type_index component;
        EntityModel* parentEntity=nullptr;
        bool isSelected = false;
    };

    struct EntityModel
    {
        corgi::std::vector<ComponentModel> components;
        corgi::Entity* entity = nullptr;
        bool    isSelected = false;
    };

    class SceneModel
    {
    public:
        corgi::std::vector<EntityModel> _registeredEntities;

        EntityModel*    _selectedEntity = nullptr;
        ComponentModel* _selectedComponent = nullptr;
    };
}