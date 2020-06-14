#pragma once

#include <corgi/components/RendererComponent.h>

#include <corgi/containers/Vector.h>

namespace corgi
{
	class Texture;
	class Renderer;

	// TODO Probably needs some love here
	class Tilemap : public RendererComponent
	{
	public:

		friend class Renderer;

		class SubTileset
		{
		public:

			// gets the coordinates of a thing inside the subTileset
			// what would actually be dope would be to share this
			// with the thing I do with the sprite sheet, but that'll do for now
			Vector<Vec2> coordinates;
		};

	// Constructors

		Tilemap();

	// Functions

		int _width;		// 4 
		int _height;	// 4 

		int _x;			// 4
		int _y;			// 4 

		int _tile_width;	// 4 bytes
		int _tile_height;	// 4 bytes

		float scaling	= 1.0f;	// 4 bytes
		float depth		= 0.0f;	// 4 bytes 

		float _opacity;				// 4 bytes
		unsigned int buffer = -1;	// 4 bytes

		int points = 0;				// 4 bytes


		// So the id of the tile should correspond to something in the tileset
		// And I should really build a tileset object thing
		Vector<long long> tiles;	// 16 bytes
		SubTileset _tileset;			// 16 bytes

		// tile_width and tile_height is set in pixel here

		// For now I send the renderer like that because I don't really know
		// how to do it otherwise
		void set_tileset(Texture* tileset, int tile_width, int tile_height);
		void generate_mesh(Renderer& renderer, Vector<long long>& tiles);
	};
}