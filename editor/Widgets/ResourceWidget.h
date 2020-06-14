#pragma once

#include <Widgets/Widget.h>
#include <corgi/filesystem/FileSystem.h>
#include <Widgets/TreeItem.h>
#include <corgi/String.h>
#include <set>

namespace Editor
{
    class ResourceManager;
    
    namespace Widgets
    {
        class Resource : public Widget
        {
        public:

            Resource(ResourceManager& manager);

        protected:

            void show() override;

        private:

            TreeItem buildTreeItems(const corgi::String& directory,
                TreeItem* parent = nullptr );

            void parseFiles(TreeItem& item);

            ResourceManager& _manager;
            int _currentItem = 0;
            int _tilesetCurrentItem = 0;

            bool _rightClicked=false;
            char _searchText[200]="";

            corgi::String _clickedFolderPath;
            corgi::String _dragData;
            corgi::String _itemEditedPath;

            bool _refreshManager    = false;
            bool _modalOpened       = false;
            bool _modalOkSelected   = false;

            bool isTabDown  = false;
            bool _isF2Down  = false;

            TreeItem _root;

            std::set<TreeItem*> _selectedItems;

            bool _editTreeNode=false;
        };
    }
}