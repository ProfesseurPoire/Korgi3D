#pragma once

#include <map>
#include <corgi/String.h>
#include <string>

#include <memory>

namespace corgi
{
    class Renderer;
    class Window;
}

namespace Editor
{
    class TextureSelectionDialog;
    class JsonSelectionDialog;
    class ResourceManager;

    namespace Widgets
    {
        class Widget;
    }

    class Application
    {
    public:

        Application();
        ~Application();

        void run();

        // Add a widget inside the widget map
        // Will use the widget's name as key
        void addWidget(Widgets::Widget* widget);

        TextureSelectionDialog& textureSelectionDialog();
        corgi::Renderer& renderer();

    private:

        ResourceManager& resource_manager();

        void Save();
        void Load();

        void main_menu();
        void import_menu();

        int windowWidth     = 800;
        int windowHeight    = 600;
        int windowPositionX = 0;
        int windowPositionY = 0;

        corgi::Window* _window;

        std::map<std::string, Widgets::Widget*> _widgets;

        std::unique_ptr<ResourceManager>        _resource_manager;
        std::unique_ptr<TextureSelectionDialog> _texture_selection_dialog;
        std::unique_ptr<JsonSelectionDialog>    _json_selection_dialog;
    };
}