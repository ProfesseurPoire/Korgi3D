#include <stdlib.h>
#include <ft2build.h>

#include FT_FREETYPE_H
#include <cstdio>

int main(int arc, char** argv)
{
	printf("Initializing Freetype library");
	
	// We start by initializing FT
	auto ee = new FT_Library();

	if (FT_Init_FreeType((FT_Library*)ee))
	{
		printf("Error : We could not initialize the freetype library");
		return 0;
	}
}
