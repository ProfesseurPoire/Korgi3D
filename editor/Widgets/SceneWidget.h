#pragma once

#include <Widgets/Widget.h>

#include <corgi/containers/List.h>

#include <vector>
#include <typeindex>
#include <typeinfo>

#include <Models/SceneModel.h>

namespace corgi
{
    class EntityPool;
    class Entity;
}

namespace Editor
{
    namespace Widgets
    {
        class Scene : public Widget
        {
        public:

            Scene(corgi::EntityPool& entityManager, SceneModel* model);

        private:

            SceneModel*  _model;

            void addEntity();

            void show() override;
            void entityNode(EntityModel& entity);

            int _currentEntityIndex = 0;

            corgi::EntityPool& _entityManager;

            bool _cancelRightClick = false;
            char  _searchText[200] = "";
        };
    }
}