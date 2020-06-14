#pragma once

#include <Widgets/Widget.h>
#include <Widgets/Canvas.h>

namespace Editor
{
    class Tileset;
    class TextureSelectionDialog;

    class TilesetCreationWidget : public Widgets::Widget
    {
    public:

    // Lifecycle

        TilesetCreationWidget();
        ~TilesetCreationWidget();


    // Virtual methods

        void init()override;

    // Methods

        void setTileset(Tileset* ts);

    protected:

        void show() override;

    private:

        Canvas * _canvas;
        bool _isOpen{ false };
        Tileset* _tileset = nullptr;
    };
}