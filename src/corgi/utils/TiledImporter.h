#pragma once

#include <string>
#include <vector>
#include <memory>

namespace corgi
{
	class Scene;
	class Texture;
}

namespace Editor
{
class TiledImporter
{
public:

	/*
	 * @brief   Imports the tiled file located at the given parameter
	 */
	TiledImporter(const std::string& path);

	~TiledImporter();

	/*
	 * @brief	Creates an entity for each layer
	 */
	void generate(corgi::Scene& scene, corgi::Texture* tileset);

	struct Property
	{
		~Property() = default;
		std::string name;
		std::string type;
	};

	struct FloatProperty : public Property
	{
		float value;
	};

	struct BoolProperty : public Property
	{
		bool value;
	};

	struct Object
	{
		int height;
		int id;
		float rotation;
		std::string type;
		std::string name;
		bool visible;
		int width;
		int x;

		struct Point
		{
			int x;
			int y;
		};

		std::vector<Point> polyline;
		std::vector<Point> polygon;

		int y;

		std::vector<Property*> properties;
	};

	struct Layer
	{
		std::string name;
		int id;
		int x;
		int y;
		float opacity;
		bool visible;

		Property* property(const std::string& name);

		template<class T>
		T* property(const std::string& name)
		{
			for (auto pro : properties)
			{
				if (pro->name == name)
				{
					return (T*)(pro);
				}
			}
			return nullptr;
		}

		std::vector<Property*> properties;
	};

	// Could use inheritance here
	struct ObjectGroup : public Layer
	{
		std::string draw_order;
		std::vector<Object> objects;
	};

	struct TileLayer : public Layer
	{
		int height;
		int width;
		// Tiles are apparently stored as long long
		// though tiles can encode it differently or even 
		// compress it but that'll do for now
		std::vector<long long> data;
	};

	struct TerrainInfo
	{
		std::string name;
		int tile;
	};

	struct TileInfo
	{
		int id;
		float probablity;
		int terrain[4];
	};

	struct TilesetInfo
	{
		int firstgid;
		int columns;
		int image_height;
		int image_width;
		int margin;
		int tile_count;
		int tileheight;
		int tilewidth;
		int spacing;

		std::string image;
		std::string name;
		std::string transparent_color;

		std::vector<TerrainInfo> terrains;
		std::vector<TileInfo> tiles;
	};

	std::string _tiled_file_path;

	int _height;
	int _width;
	int _nextlayerid;
	int _next_object_id;
	int _tile_height;
	int _tile_width;

	float _version;

	std::string _orientation;
	std::string _render_order;
	std::string _tiled_version;
	std::string _type;

	bool _infinite;

	std::vector<TileLayer>      _tile_layers;
	std::vector<ObjectGroup>    _object_groups;
	std::vector<TilesetInfo>    _tileset_infos;
};
}