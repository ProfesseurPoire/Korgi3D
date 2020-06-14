#pragma once

#include <corgi/containers/List.h>
#include <corgi/utils/ResourceManager.h>

#include <functional>
namespace Editor
{

//TODO : 
// Maybe have a single File dialog? With some filters? 
class JsonSelectionDialog
{
public:

    JsonSelectionDialog(ResourceManager& resourceManager);

    void open();
    void show();

    std::string selection()const;
    bool is_opened()const;

    std::function<void(const std::string&)> callback;

private:

    bool _opened = false;
    bool _closed = false;

    std::string _selection;
    ResourceManager& _resourceManager;

};
}