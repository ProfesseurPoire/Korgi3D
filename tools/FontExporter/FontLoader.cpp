#include "FontLoader.h"

#include <stdlib.h>
#include <ft2build.h>
#include <fstream>

#include FT_FREETYPE_H
#include <cstdio>
#include "freetype/freetype.h"

static bool freetype_initialized_ = false;
static FT_Library* freetype_library = nullptr;

struct character_info
{
	float ax{ 0.0f }, ay{ 0.0f }; // advance
	float bw{ 0.0f }, bh{ 0.0f }; // bitmap width and row
	float bl{ 0.0f }, bt{ 0.0f }; // bitmap_left and top
	float tx{ 0.0f }; // x offset of glyph in texture coordinates
};

bool FontLoader::initialize()
{
	// We start by initializing FT
	freetype_library = new FT_Library();

	if (FT_Init_FreeType(freetype_library))
	{
		printf("Error : We could not initialize the freetype library");
		return freetype_initialized_ = false;
	}
	return freetype_initialized_ = true;
}

static void save_font(character_info* characters_info, unsigned char* image, int width, int height, const std::string& path)
{
	std::ofstream file (path, std::fstream::binary);
	// image uses 4 components, RGBA

	if(!file.is_open())
	{
		printf("Could not create file at %s", path.c_str());
	}

	file.write((char*)&width, sizeof(width));
	file.write((char*)&height, sizeof(height));
	file.write((char*)image, width * height * 4);

	file.write((char*)characters_info, 256 * sizeof(character_info));
}


void build_atlas(FT_Face* face, const std::string& output_path)
{
	character_info characters[256];
	
	FT_Error error = FT_Set_Pixel_Sizes(*face, 0, 60);

	if (error != 0)
	{
		printf("Error while setting the font pixel size");
	}

	FT_Face& f = *(FT_Face*)face;
	FT_GlyphSlot glyph = f->glyph;

	int width   = 0;
	int height  = 0;

	// Only scan for the max height 
	for (int i = 32; i < 128; i++)
	{
		if (FT_Load_Char(*face, (unsigned char)i, FT_LOAD_RENDER))
		{
			printf("Error while Loading character");
		}
		width += glyph->bitmap.width;

		if (glyph->bitmap.rows > height)
		{
			height = glyph->bitmap.rows;
		}
		// WARNING: Changed that line, maybe it breaks something
		//height = std::max(height, (int) glyph->bitmap.rows);
	}

	int x = 0;

	auto image = new unsigned char[width * height * 4];

	for (int i = 32; i < 128; i++)
	{
		if (FT_Load_Char(*face, i, FT_LOAD_RENDER))
			continue;

		// Build the information used to know where a character is
		// on the final image
		characters[i].ax = static_cast<float>(glyph->advance.x >> 6);
		characters[i].ay = static_cast<float>(glyph->advance.y >> 6);

		
		characters[i].bw = static_cast<float>(glyph->bitmap.width);
		characters[i].bh = static_cast<float>(glyph->bitmap.rows);

		characters[i].bl = static_cast<float>(glyph->bitmap_left);
		characters[i].bt = static_cast<float>(glyph->bitmap_top);

		characters[i].tx = static_cast<float>(x) / width;

		// Write the glyphs in the texture
		for (int j = 0; j < glyph->bitmap.rows; j++)
		{
			for (int k = 0; k < glyph->bitmap.width; k++)
			{
				image[(((height - 1) - j) * width + (x + k)) * 4] = 255;
				image[(((height - 1) - j) * width + (x + k)) * 4 + 1] = 255;
				image[(((height - 1) - j) * width + (x + k)) * 4 + 2] = 255;
				image[(((height - 1) - j) * width + (x + k)) * 4 + 3] = static_cast<unsigned char*>(glyph->bitmap.buffer)[j * glyph->bitmap.width + k];
			}
		}
		x += glyph->bitmap.width;
	}

	save_font(characters, image, width, height, output_path);
}

bool FontLoader::load(const std::string& path, const std::string& output_path)
{
	if(!freetype_initialized_)
	{
		initialize();
	}

	auto face = new FT_Face();

	if (FT_New_Face(*freetype_library, path.c_str(), 0, (FT_Face*)face))
	{
		printf("Error : the font could not be loaded");
		return false;
	}
	
	std::ifstream file(path);

	if(!file.is_open())
	{
		return false;
	}
	// for now I just try to open the file

	build_atlas(face, output_path);
	return true;
}
