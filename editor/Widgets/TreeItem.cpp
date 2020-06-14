#include "TreeItem.h"

#include <filesystem>
#include <corgi/filesystem/FileSystem.h>

namespace Editor
{
    std::vector<TreeItem>& TreeItem::children()
    {
        return _children;
    }

    const corgi::String TreeItem::extension()const
    {
        return corgi::filesystem::extension(_path);
    }

    const corgi::String& TreeItem::path()const
    {
        return _path;
    }

    const corgi::String TreeItem::name()const
    {
        return corgi::filesystem::filename(_path);
    }

    bool TreeItem::is_directory()const
    {
        return _isDirectory;
    }

    // TODO : Needs to be put in corgi::String
    static void switchCharacters(char* str, char old, char newChar)
    {
        int i = 0;

        while (str[i] != '\0')
        {
            if (str[i] == old)
            {
                str[i] = newChar;
            }
            ++i;
        }
    }

    void TreeItem::name(const corgi::String& name)
    {
        switchCharacters(_path.data(), '\\', '/');

        _path = corgi::filesystem::path_without_name(_path);
        _path += "/"+name;
    }
    
    TreeItem::TreeItem(corgi::String file)
    {
        _path= file;
        _isDirectory = corgi::filesystem::is_directory(_path);
    }

    void TreeItem::addChild(TreeItem item)
    {
        _children.push_back(item);
    }
}