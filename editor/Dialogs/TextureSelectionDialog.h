#pragma once

#include <corgi/containers/List.h>
#include <corgi/utils/ResourceManager.h>

namespace Editor
{
    class TextureSelectionDialog
    {
    public:

        TextureSelectionDialog(ResourceManager& resourceManager);

        void open();
        void show();

        corgi::TextureHandle* getSelectedTexture()const;

        bool isOpened()const;

    private:

        bool _opened = false;
        corgi::TextureHandle* _selectedTexture = nullptr;
        ResourceManager& _resourceManager;
    };
}