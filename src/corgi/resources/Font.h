#pragma once

#include <string>

namespace corgi
{
	class Image;
	class Texture;

	struct CharacterInfo
	{
		float ax, ay; // advance
		float bw, bh; // bitmap width and row
		float bl, bt; // bitmap_left and top
		float tx; // x offset of glyph in texture coordinates
	};

	class Font
	{
	public:

		Font(const std::string& name, const std::string & font_file);
		~Font();

		Texture* texture = nullptr;
		CharacterInfo characters[256];
	private:

	};
}