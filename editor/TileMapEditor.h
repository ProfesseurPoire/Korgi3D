#pragma once

#include <corgi/String.h>
#include <corgi/utils/Tileset.h>

namespace corgi
{
    struct TextureHandle;
}

namespace Editor
{
	class TextureSelectionDialog;

	class TileMapEditor
	{
	public:

		TileMapEditor(TextureSelectionDialog& dialog);
		void Show();
		bool showed = true;

	private:

		TextureSelectionDialog& textureSelectionDialog;
        Tileset _tileset;

        int _selectedGridX = 0;
        int _selectedGridY = 0;

        bool _tileSelected = false;
	};
}