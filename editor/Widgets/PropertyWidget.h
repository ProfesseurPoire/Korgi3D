#pragma once

#include <Widgets/Widget.h>
#include <Models/SceneModel.h>

namespace corgi
{
    class EntityPool;
    class MeshRenderer;
}

namespace Editor
{
    class TextureSelectionDialog;

    namespace Widgets
    {
        class Property : public Widget
        {
        public:

            Property(   corgi::EntityPool& entityManager,
                        TextureSelectionDialog& dialog,
                        SceneModel* model);

        private:

            void show()override;
            void showMesh(corgi::MeshRenderer& mesh);

            SceneModel* _model;
            corgi::EntityPool&   _entityManager;
            TextureSelectionDialog& _textureSelectionDialog;

            int _currentEntityIndex = 0;
        };
    }
}