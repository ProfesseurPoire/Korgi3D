#pragma once

#include <corgi/filesystem/FileSystem.h>
#include <corgi/String.h>
#include <vector>

namespace Editor
{
    // The treeItem thing will probably be abstracted later on
    // so I can easily reuse it between the Scene and Resource widget
    // but for now I try to focus on making something that works
    class TreeItem
    {
    public:

        TreeItem(corgi::String path);

        void addChild(TreeItem item);

        bool is_directory()const;

        const corgi::String     name()const;
        const corgi::String&    path()const;
        const corgi::String     extension()const;

        void name(const corgi::String& str);

        std::vector<TreeItem>& children();
        bool opened=false;

    private:

        // Since a treeItem can be edited, it needs to have a cache
        // of sort. Maybe I could build it only when edition
        // is needed?
        bool _isDirectory = false;

        corgi::String _path;
        corgi::String _onlyPath;
        std::vector<TreeItem>  _children;
    };
}