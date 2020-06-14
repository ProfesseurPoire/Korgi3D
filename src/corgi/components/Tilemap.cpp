#include "Tilemap.h"

#include <corgi/rendering/texture.h>
#include <corgi/rendering/renderer.h>
#include <corgi/rendering/Mesh.h>

#include <corgi/utils/ResourcesLoader.h>

namespace corgi
{
	Tilemap::Tilemap()
		:RendererComponent(nullptr)
	{
		// getting the default material here
		material = *ResourcesLoader::material("unlit_texture.mat");
	}

	void Tilemap::generate_mesh(Renderer& renderer, std::vector<long long>& tiles)
	{
		this->tiles = tiles;

		// Tiled stores information about tile flipping on 3 bits of the id

		// TODO : could use 0b'1010'1010 syntax?
		const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
		const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
		const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;

		auto texture = material.textures_.begin()->second;
		const int  ratio			= (texture->width() / _tile_width);
		const float tex_tile_width	= static_cast<float>(_tile_width)  / static_cast<float>(texture->width());
		const float tex_tile_height	= static_cast<float>(_tile_height) / static_cast<float>(texture->height());

		float bottom = static_cast<float>(_y);

		_mesh = Mesh::new_standard_mesh();

		// Drawing the tilemap in the most naive way for now
		for (int i = 0; i < _height; ++i)
		{
			float left = static_cast<float>( _x);

			for (int j = 0; j < _width; ++j)
			{
				// Get the tile id
				long long tile_id = tiles[i * _width + j];

				const bool flipped_horizontally	= (tile_id & FLIPPED_HORIZONTALLY_FLAG);
				const bool flipped_vertically	= (tile_id & FLIPPED_VERTICALLY_FLAG);
				const bool flipped_diagonally	= (tile_id & FLIPPED_DIAGONALLY_FLAG);

				// Remove the flags from the id
				tile_id &= ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);

				long long tile_x = tile_id % ratio;
				long long tile_y = tile_id / ratio;

				// if the tile id is equal to zero, we just skip the thing
				if (tile_id == 0)
				{
					left += _tile_width;
					continue;
				}

				// I think ID starts at 1 in tiled 
				Vec2 coord = _tileset.coordinates[int(tile_id - 1)];

				int flip_h_1 = 0;
				int flip_h_2 = 1;

				int flip_v_1 = 0;
				int flip_v_2 = 1;

				if (flipped_horizontally)
				{
					flip_h_2 = 0;
					flip_h_1 = 1;
				}

				if (flipped_vertically)
				{
					flip_v_1 = 1;
					flip_v_2 = 0;
				}

				if (!flipped_diagonally)
				{
					float x_coord = coord.x + flip_h_1 * tex_tile_width;
					float width_coord = tex_tile_width * flip_h_2 - tex_tile_width * flip_h_1;

					float y_coord = coord.y - flip_v_1 * tex_tile_height;
					float height_coord = -tex_tile_height * flip_v_2 + tex_tile_height * flip_v_1;

					_mesh->add_vertex()
						.position(left, bottom, depth)
						.uv(x_coord, y_coord);

					_mesh->add_vertex()
						.position(left, bottom - _tile_height, depth)
						.uv(x_coord, y_coord + height_coord);

					_mesh->add_vertex()
						.position(left + _tile_width, bottom - _tile_height, depth)
						.uv(x_coord + width_coord, y_coord + height_coord);

					_mesh->add_vertex()
						.position(left + _tile_width, bottom, depth)
						.uv(x_coord + width_coord, y_coord);

					int in = _mesh->vertex_count() - 4;
					_mesh->add_triangle(in, in+1, in + 2);
					_mesh->add_triangle(in , in + 2, in + 3);
				}
				else
				{
					float coord_x = coord.x  + (flip_h_2 * tex_tile_width);
					float coord_x2 = coord.x + (flip_h_1 * tex_tile_width);

					float coord_y = coord.y - (flip_v_1 * tex_tile_height);
					float coord_y2 = coord.y - (flip_v_2 * tex_tile_height);

					_mesh->add_vertex()
						.position(left, bottom, depth)
						.uv(coord_x, coord_y);

					_mesh->add_vertex()
						.position(left, bottom - _tile_height, depth)
						.uv(coord_x2, coord_y);

					_mesh->add_vertex()
						.position(left + _tile_width, bottom - _tile_height, depth)
						.uv(coord_x2, coord_y2);

					_mesh->add_vertex()
						.position(left + _tile_width, bottom, depth)
						.uv(coord_x, coord_y2);

					int in = _mesh->vertex_count() - 4;
					_mesh->add_triangle(in, in + 1, in + 2);
					_mesh->add_triangle(in, in + 2, in + 3);
				}
				left += _tile_width;
			}
			bottom -= _tile_height;
		}
		_mesh->update_vertices();
	}

	void Tilemap::set_tileset(Texture* ts, int tile_width, int tile_height)
	{
		assert(ts);

		material.add_texture(ts, "main_texture");

		_tile_width = tile_width;
		_tile_height = tile_height;

		int rows = ts->height() / tile_height;
		int cols = ts->width()  / tile_width;

		// tile_width in [0.0; 1.0] range
		float tile_width_ratio	= (float)tile_width  / ts->width();
		float tile_height_ratio = (float)tile_height / ts->height();

		// The goal here is to stores the coordinates of the tileset's tiles
		// so I don't have to convert the tile id to row and columns
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				_tileset.coordinates.push_back
				(
					Vec2(j * tile_width_ratio, 1.0f -  i * tile_height_ratio)
				);
			}
		}
	}
}