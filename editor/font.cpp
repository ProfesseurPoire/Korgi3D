#include "font.h"

#include <stdlib.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <corgi/utils/log.h>

#include "corgi/filesystem/filesystem.h"

namespace corgi
{
    bool FontLoader::freetype_initialized = false;
    void* FontLoader::freetype_library;

	bool FontLoader::initialize_library()
	{
		if (!FontLoader::freetype_initialized)
		{
			freetype_library = new FT_Library();

			if (FT_Init_FreeType((FT_Library*) freetype_library))
			{
				freetype_initialized = false;
				printf("Error : We could not initialize the freetype library");
			}
			freetype_initialized = true;
		}
		return FontLoader::freetype_initialized;
	}

	FontLoader::FontLoader(const char* filename, int police)
		: characters{}
	{
		pixel_height_ = police;
		_is_valid = load(filename);
	}

    float FontLoader::text_width(const char* str, FontLoader* fnt, float sx, float sy)
    {
        float x = 0;
        float y = 0;

        int size_text = strlen(str);

        for (int i = 0; i < size_text; i++)
        {
            char c = str[i];


            /* Advance the cursor to the start of the next character */
            x += fnt->characters[c].ax*sx;
            y += fnt->characters[c].ay*sy;
        }
        return x;
    }

    bool FontLoader::load(const char* file)
    {
        if (!initialize_library())
        {
            return false;
        }

        face = new FT_Face();

        // Loads the font
        if (FT_New_Face(*((FT_Library*) freetype_library), file, 0, (FT_Face*) face))
        {
            log_warning("Error : the font could not be loaded");
            return false;
        }

        build_atlas();
        return true;
    }

    void FontLoader::build_atlas()
    {
        // Set pixel size needs to be called after a face is loaded otherwise 
        // FT_Load_Char will fail
        FT_Error error = FT_Set_Pixel_Sizes(*(FT_Face*) face, 0, 60);

        if (error != 0)
        {
            log_warning("Error while setting the font pixel size");
        }

        FT_Face& f = *(FT_Face*) face;
        FT_GlyphSlot glyph = f->glyph;

        width	= 0;
        height	= 0;

        // Only scan for the max height 
        for (int i = 32; i < 128; i++)
        {
            if (FT_Load_Char(*(FT_Face*) face, (unsigned char) i, FT_LOAD_RENDER))
            {
                log_warning("Error while Loading character");
            }
            width += glyph->bitmap.width;

            if (glyph->bitmap.rows > height )
            {
                height = glyph->bitmap.rows;
            }
            // WARNING: Changed that line, maybe it breaks something
            //height = std::max(height, (int) glyph->bitmap.rows);
        }

        int x = 0;

        image = new unsigned char[width*height * 4];

        for (int i = 32; i < 128; i++)
        {
            if (FT_Load_Char(*static_cast<FT_Face*>(face), i, FT_LOAD_RENDER))
                continue;

            // Build the informations used to know where a character is
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
                    image[(((height-1)- j)*width + (x + k))*4]		= 255;
                    image[(((height-1) - j)*width + (x + k))*4 + 1]	= 255;
                    image[(((height-1) - j)*width + (x + k))*4 + 2]	= 255;
                    image[(((height-1) - j)*width + (x + k))*4 + 3]	= static_cast<unsigned char*>(glyph->bitmap.buffer)[j*glyph->bitmap.width + k];
                }
            }
            x += glyph->bitmap.width;
        }
    }

    bool FontLoader::is_valid()const
    {
        return _is_valid;
    }
}