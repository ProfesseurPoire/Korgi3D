#pragma once

namespace corgi
{
	class Texture;

	struct character_info
	{
		float ax{0.0f}, ay{0.0f}; // advance
		float bw{0.0f}, bh{0.0f}; // bitmap width and row
		float bl{0.0f}, bt{0.0f}; // bitmap_left and top
		float tx{0.0f}; // x offset of glyph in texture coordinates
	};

//! \brief  Loads a font using freetype
//!         FreeType returns a bitmap image with a 1 byte GetComponent ( corresponding to the alpha value)
//!         for each glyph. The trick is to parse every character and build one unique texture, called an atlas
class FontLoader
{
public:

	// This shall probably be send elsewhere 
	static bool     initialize_library();
	static void*    freetype_library;
	static bool     freetype_initialized;

	// Constructors

	FontLoader() = default;

	//! \brief  Builds a new font object
	//!
	//! \param file             File name to load font from  </param>
	//! \param pixel_height     Height size of the font in pixel </param>
	FontLoader(const char* file, int pixel_height);

	

	// Functions

	void build_atlas();
	bool load(const char* file);
	float text_width(const char* str, FontLoader* font, float sx, float sy);

	bool is_valid()const;


	int width{ 0 }, height{ 0 };
	unsigned char* image{ nullptr };

	float r{ 0.0f }, g{ 0.0f }, b{ 0.0f }, a{ 0.0f };
	int  pixel_height_ = 30;
	character_info characters[256]{};

	void* face{ nullptr }; // Correspond to a FT_Face (a font)
	
	char* filename{ nullptr };

private:

	bool _is_valid = false;
};
}