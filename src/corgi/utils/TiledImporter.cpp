#include "TiledImporter.h"

#include <corgi/rapidjson/document.h>
#include <corgi/rapidjson/filereadstream.h>
#include <corgi/rapidjson/istreamwrapper.h>

#include <corgi/components/Tilemap.h>
#include <corgi/components/Transform.h>

#include <corgi/rendering/texture.h>

#include <fstream>

#include <corgi/ecs/Scene.h>
#include <corgi/ecs/Entity.h>

namespace Editor
{
	std::vector<TiledImporter::Property*> load_properties(rapidjson::Value& value)
	{
		std::vector<TiledImporter::Property*> properties;

		if (value.HasMember("properties"))
		{
			for (auto& prop : value["properties"].GetArray())
			{
				std::string property_type = prop["type"].GetString();

				if (property_type == "float")
				{
					auto property = new TiledImporter::FloatProperty();

					property->name = prop["name"].GetString();
					property->value = prop["value"].GetFloat();
					property->type = "float";

					properties.emplace_back((TiledImporter::Property*) property);
				}

				if (property_type == "bool")
				{
					auto property = new TiledImporter::BoolProperty();

					property->name = prop["name"].GetString();
					property->value = prop["value"].GetBool();
					property->type = "bool";

					properties.emplace_back((TiledImporter::Property*) property);
				}
			}
		}
		return properties;
	}

TiledImporter::TilesetInfo load_tileset_info(rapidjson::Value& value)
{
	TiledImporter::TilesetInfo tsi;

	tsi.firstgid			= value["firstgid"].GetInt();
	tsi.columns				= value["columns"].GetInt();
	tsi.image				= value["image"].GetString();
	tsi.image_height		= value["imageheight"].GetInt();
	tsi.image_width			= value["imagewidth"].GetInt();
	tsi.margin				= value["margin"].GetInt();
	tsi.name				= value["name"].GetString();
	tsi.spacing				= value["spacing"].GetInt();
	tsi.tile_count			= value["tilecount"].GetInt();
	tsi.tileheight			= value["tileheight"].GetInt();
	tsi.tilewidth			= value["tilewidth"].GetInt();
	//tsi.transparent_color	= value["transparentcolor"].GetString();

	for (auto& v : value["terrains"].GetArray())
	{
		tsi.terrains.push_back({ v["name"].GetString() ,v["tile"].GetInt() });
	}

	if(value.HasMember("tiles"))
	{
		for (auto& v : value["tiles"].GetArray())
		{
			TiledImporter::TileInfo i;
			if (v.HasMember("probability"))
			{
				i.probablity = v["probability"].GetFloat();
			}

			i.id = v["id"].GetInt();

			if (v.HasMember("terrains"))
			{
				int iter = 0;
				for (auto& tv : v["terrain"].GetArray())
				{
					i.terrain[iter++] = tv.GetInt();
				}
			}
			tsi.tiles.push_back(i);
		}
	}
	return tsi;
}

TiledImporter::Object load_object(rapidjson::Value& value)
{
	TiledImporter::Object object;
		
	object.id 		= value["id"].GetInt();
	object.name 	= value["name"].GetString();
	object.height	= static_cast<int>(value["height"].GetFloat());
	object.rotation = value["rotation"].GetFloat();
	object.type		= value["type"].GetString();
	object.visible	= value["visible"].GetBool();
	object.width	= static_cast<int>(value["width"].GetFloat());
	object.x		= static_cast<int>(value["x"].GetFloat());
	object.y		= static_cast<int>(value["y"].GetFloat());

	if (value.HasMember("polyline"))
	{
		for (auto& v : value["polyline"].GetArray())
		{
			object.polyline.push_back(
				{
					v["x"].GetInt(),
					v["y"].GetInt()
				}
			);
		}
	}

	if (value.HasMember("polygon"))
	{
		for (auto& v : value["polygon"].GetArray())
		{
			object.polygon.push_back(
				{
					v["x"].GetInt(),
					v["y"].GetInt()
				}
			);
		}
	}

	object.properties = load_properties(value);
	return object;
}


TiledImporter::ObjectGroup load_object_group(rapidjson::Value& value)
{
	TiledImporter::ObjectGroup object_group;

	object_group.opacity	= value["opacity"].GetFloat();
	object_group.visible	= value["visible"].GetBool();
	object_group.name		= value["name"].GetString();
	object_group.id			= value["id"].GetInt();
	object_group.x			= value["x"].GetInt();
	object_group.y			= value["y"].GetInt();
	object_group.draw_order = value["draworder"].GetString();

	for (auto& obj : value["objects"].GetArray())
	{
		object_group.objects.push_back(load_object(obj));
	}
		

	object_group.properties = load_properties(value);

	return object_group;
}


TiledImporter::TileLayer load_tile_layer(rapidjson::Value& value)
{
	TiledImporter::TileLayer layer;

	layer.width		= value["width"].GetInt();
	layer.height	= value["height"].GetInt();
	layer.opacity	= value["opacity"].GetFloat();
	layer.visible	= value["visible"].GetBool();
	layer.name		= value["name"].GetString();
	layer.id		= value["id"].GetInt();
	layer.x			= value["x"].GetInt();
	layer.y			= value["y"].GetInt();

	for (auto& data : value["data"].GetArray())
		layer.data.emplace_back(data.GetInt64());

	layer.properties = load_properties(value);

	return layer;
}

TiledImporter::TiledImporter(const std::string& path)
	:_tiled_file_path(path)
{
	std::ifstream file(_tiled_file_path.c_str());
	rapidjson::Document d;
	auto f = rapidjson::IStreamWrapper(file);
	d.ParseStream(f);
	 
	_height         = d["width"].GetInt();
	_width          = d["width"].GetInt();
	_nextlayerid    = d["nextlayerid"].GetInt();
	_next_object_id = d["nextobjectid"].GetInt();
	_tile_height    = d["tileheight"].GetInt();
	_tile_width     = d["tilewidth"].GetInt();

	_version        = d["version"].GetFloat();

	_infinite       = d["infinite"].GetBool();

	_orientation    = d["orientation"].GetString();
	_render_order   = d["renderorder"].GetString();
	_tiled_version  = d["tiledversion"].GetString();
	_type           = d["type"].GetString();

	for (auto& value : d["tilesets"].GetArray())
		_tileset_infos.push_back(load_tileset_info(value));

	for (auto& layer : d["layers"].GetArray())
	{
		std::string layer_type = layer["type"].GetString();
		if (layer_type == "tilelayer")
			_tile_layers.push_back(load_tile_layer(layer));
		if (layer_type == "objectgroup")
			_object_groups.push_back(load_object_group(layer));
	}
}

TiledImporter::~TiledImporter()
{

}

void TiledImporter::generate(corgi::Scene& scene, corgi::Texture* tileset)
{
	int it = 0;
	// Generate a tilemap for every layer
	for (auto& tile_layer : _tile_layers)
	{
		corgi::Entity& tm = scene.root()->add_child();
		tm.name("tilemap");

		tm.add_component<corgi::Transform>();
		tm.add_component<corgi::Tilemap>();

		tm.get_component<corgi::Transform>().position(0.0f, 0.0f, -2.0f);
		auto& tilemap = tm.get_component<corgi::Tilemap>();

		tilemap.depth			= (it++) * 0.21f;
		tilemap._height			= tile_layer.height;
		tilemap._width			= tile_layer.width;

		tilemap.set_tileset(tileset,
			_tileset_infos[0].tilewidth,
			_tileset_infos[0].tileheight);

		tilemap._x				= 0;
		tilemap._y				= 0;
		tilemap.tiles			= tile_layer.data;
	}
}

}