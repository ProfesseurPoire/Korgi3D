#include "AsepriteImporter.h"

#include <corgi/rapidjson/document.h>
#include <corgi/rapidjson/filereadstream.h>

#include <corgi/components/Animator.h>
#include <corgi/rendering/texture.h>

#include <fstream>

#include <corgi/utils/Strings.h>

namespace corgi
{
	std::vector<Animation> AsepriteImporter::animations(Texture* texture)
	{
		std::vector<Animation> animations;

		for (auto& tag : meta.frame_tags)
		{
			auto& animation = animations.emplace_back();
			animation.name_ = convert_std_string(tag.name);
			// An animation loops if I can find the "loop" keyword inside the tag's name
			animation.looping = (tag.name.find("loop") != std::string::npos);

			for (int i = tag.from; i <= tag.to; i++)
			{
				animation.frames.push_back
				(
					{
						{
							static_cast<float>(frames[i].frame.w),
							static_cast<float>(frames[i].frame.h),
							static_cast<float>(frames[i].frame.x),
							static_cast<float>(texture->height() - frames[i].frame.y) - static_cast<float>(frames[i].frame.h),
							texture
						},
						static_cast<float>(frames[i].duration) / 1000.0f
					}
				);
			}
		}
		return animations;
	}

    AsepriteImporter::Meta load_meta(rapidjson::Value& value)
    {
        AsepriteImporter::Meta meta;

        meta.app        = value["app"].GetString();
        meta.version    = value["version"].GetString();
        meta.image      = value["image"].GetString();
        meta.format     = value["format"].GetString();

        meta.w = value["size"]["w"].GetInt();
        meta.h = value["size"]["h"].GetInt();

        meta.scale = value["scale"].GetString();
        
        for (auto& f : value["frameTags"].GetArray())
        {
            
            meta.frame_tags.emplace_back();

            meta.frame_tags.back().direction = f["direction"].GetString();
            meta.frame_tags.back().name      = f["name"].GetString();
            meta.frame_tags.back().from      = f["from"].GetInt();
            meta.frame_tags.back().to        = f["to"].GetInt();
        }

        for (auto& l : value["layers"].GetArray())
        {
            AsepriteImporter::Layer layer;

            layer.name = l["name"].GetString();
            layer.opacity = l["opacity"].GetInt();
            layer.blend_mode = l["blendMode"].GetString();

            meta.layers.push_back(layer);
        }
        return meta;
    }

    AsepriteImporter::AsepriteFrame load_frame(rapidjson::Value& value)
    {
        AsepriteImporter::AsepriteFrame frame;

        frame.rotated   = value["rotated"].GetBool();
        frame.trimmed   = value["trimmed"].GetBool();
        frame.duration  = value["duration"].GetInt();

        frame.source_size.w = value["sourceSize"]["w"].GetInt();
        frame.source_size.h = value["sourceSize"]["h"].GetInt();

        frame.sprite_source_size.x = value["spriteSourceSize"]["x"].GetInt();
        frame.sprite_source_size.y = value["spriteSourceSize"]["y"].GetInt();
        frame.sprite_source_size.w = value["spriteSourceSize"]["w"].GetInt();
        frame.sprite_source_size.h = value["spriteSourceSize"]["h"].GetInt();

        frame.frame.x = value["frame"]["x"].GetInt();
        frame.frame.y = value["frame"]["y"].GetInt();
        frame.frame.w = value["frame"]["w"].GetInt();
        frame.frame.h = value["frame"]["h"].GetInt();

        return frame;
    }

    void AsepriteImporter::load(const std::string& path)
    {
		//TODO : This bit of code is copy paste all around the place, maybe one day it would
		// be nice to actually make a corgi/utils/Files thing and add a function for this
        std::ifstream file(path.c_str(), std::ifstream::in | std::ifstream::binary);

        int fileSize = 0;

        if (file.is_open())
        {
            file.seekg(0, std::ios::end);
            fileSize = int(file.tellg());
            file.close();
        }

        FILE* fp = fopen(path.c_str(), "rb"); // non-Windows use "r"

        char* readBuffer = new char[fileSize];

        rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
        rapidjson::Document document;
        document.ParseStream(is);

        for (auto it = document["frames"].MemberBegin();
             it < document["frames"].MemberEnd(); ++it)
        {
            frames.push_back(load_frame(it->value));
        }

        meta = load_meta(document["meta"]);
        
        delete[] readBuffer;
    }
}