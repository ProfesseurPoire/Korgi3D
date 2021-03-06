#include "ResourcesLoader.h"

#include <corgi/filesystem/FileSystem.h>

#include <corgi/rendering/renderer.h>
#include <corgi/rendering/texture.h>

#include <corgi/resources/image.h>
#include <corgi/resources/Font.h>
#include <corgi/resources/Sound.h>

#include <corgi/utils/Utils.h>
#include <corgi/utils/AsepriteImporter.h>		

#include <corgi/logger/log.h>

#include <corgi/rapidjson/rapidjson.h>
#include <corgi/rapidjson/document.h>
#include <corgi/rapidjson/filereadstream.h>

#include <map>
#include <algorithm>

#include <iostream>

namespace corgi
{
	static std::map<std::string, Texture>	textures_;
	static std::map<std::string, Material>	materials_;
	static std::map<std::string, Font>		fonts_;
	static std::map<std::string, Sound>		sounds_;

	// There can be more than one resource directories. Most of the time
	// there will be one resource directory used for the most basic stuff
	// by the library, and another one for the project using the library
	static std::vector<std::string> directories_;

	static Renderer* renderer_ = nullptr;	// Only the renderer can build textures

	static std::map<std::string, Texture::Wrap> wraps = 
	{
		{"repeat", Texture::Wrap::Repeat},
		{"clamp_to_border", Texture::Wrap::ClampToBorder},
		{"clamp_to_edge", Texture::Wrap::ClampToEdge },
		{"mirrored_repeat", Texture::Wrap::MirroredRepeat},
		{"mirror_clamp_to_edge", Texture::Wrap::MirrorClampToEdge}
	};
	
	static Texture::Wrap load_wrap(const std::string& str)
	{
		return wraps.at(str);
	}

	static std::map<std::string, Texture::MinFilter> min_filters_ =
	{
		{"nearest", Texture::MinFilter::Nearest},
		{"linear", Texture::MinFilter::Linear},
		{"nearest_mipmap_nearest", Texture::MinFilter::NearestMipmapNearest},
		{"nearest_mipmap_linear", Texture::MinFilter::NearestMipmapLinear},
		{"linear_mipmap_linear", Texture::MinFilter::LinearMipmapLinear},
		{"linear_mipmap_nearest", Texture::MinFilter::LinearMipmapNearest}
	};

	static Texture::MinFilter parse_min_filter(const std::string& str)
	{
		return min_filters_.at(str);
	}

	static std::map<std::string, Texture::MagFilter> mag_filters =
	{
		{"nearest", Texture::MagFilter::Nearest},
		{"linear", Texture::MagFilter::Linear}
	};

	static Texture::MagFilter parse_mag_filter(const std::string& str)
	{
		return mag_filters.at(str);
	}

	static void load_texture(const std::string& path)
	{
		if (!path.empty())
		{
			// TODO : Put this inside an util class, cause I copy paste
			// this way too often
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

			assert(document.HasMember("wrap_s"));
			assert(document.HasMember("wrap_t"));
			assert(document.HasMember("min_filter"));
			assert(document.HasMember("mag_filter"));

			corgi::Image* image = Editor::Utils::LoadImageForReal(
				(path.substr(0, path.size() - 4) + ".png").c_str());

			auto name = filesystem::filename((path.substr(0, path.size() - 4) + ".png"));


			// TODO : probably needs better information about the loaded image
			if (image->channel() == 3)
			{
				textures_.try_emplace(name,  
					name,
					image->width(),
					image->height(),
					parse_min_filter(document["min_filter"].GetString()),
					parse_mag_filter(document["mag_filter"].GetString()),
					load_wrap(document["wrap_s"].GetString()),
					load_wrap(document["wrap_t"].GetString()),
					Texture::Format::RGB,
					Texture::InternalFormat::RGB,
					Texture::DataType::UnsignedByte,
					image->pixels());
			}

			if (image->channel() == 4)
			{
				if (textures_.count(name) == 0)
				{
					textures_.try_emplace( name, 
						name,
						image->width(),
						image->height(),
						parse_min_filter(document["min_filter"].GetString()),
						parse_mag_filter(document["mag_filter"].GetString()),
						load_wrap(document["wrap_s"].GetString()),
						load_wrap(document["wrap_t"].GetString()),
						Texture::Format::RGBA,
						Texture::InternalFormat::RGBA,
						Texture::DataType::UnsignedByte,
						image->pixels());
				}
			}

			delete image;
		}
	}

	std::string ResourcesLoader::find(const std::string& path)
	{
		for (std::string& d : directories_)
		{
			std::string fullpath = d + "/" + path;
			if (filesystem::file_exist(fullpath.c_str()))
				return fullpath;
		}
		log_error("The application could not find a file at \"" + path + "\" in its resource directories");
		return "";
	}

	void ResourcesLoader::add_directory(const std::string& directory)
	{
		directories_.push_back(directory);
	}

	/*void ResourcesLoader::parseFile(const corgi::String& directory)
	{
		auto files = corgi::filesystem::list_directory(directory);

		for (auto& file : files)
		{
			if (file.is_folder())
			{
				parseFile(file.path());
			}
			else
			{
				if (file.extension() == "png")
				{
					corgi::Image* image = Utils::LoadImageForReal(file.path());
					corgi::Texture* t = renderer_.generateTextureFromImage(*image);
					textures.add(TextureHandle(t, String(file.name())));
				}

				if (file.extension() == "json")
				{
					_json_files.push_back(file.path().cstr());
				}
			}
		}
	}*/

	// This should probably be used into the resource_manager too
	// since this is kinda it's job, being able to load stuff 
	std::vector<Animation> ResourcesLoader::load_animations(const std::string& path)
	{
		// Maybe just do all in one go?
		AsepriteImporter importer;
		importer.load(find(path + ".json"));
		return importer.animations( texture(path + ".png"));
	}

	// This function simply look for every file inside the resource directories
	std::vector<filesystem::FileInfo> get_all_files(const std::vector<std::string>& directories)
	{
		std::vector<filesystem::FileInfo> files;

		for (auto& directory : directories)
		{
			auto new_files = filesystem::list_directory(directory, true);
			files.insert(files.begin(), new_files.begin(), new_files.end());
		}
		return files;
	}

	void ResourcesLoader::initialize(Renderer& renderer)
	{
		renderer_ = &renderer;
		auto files = get_all_files(directories_);

		// First pass to make sure every .png has a .tex file associated
		for (auto file : get_all_files(directories_))
		{
			if (file.extension() == "png")
			{
				// We check if a .tex exist for the current png, creates a default
				// one otherwise

				if (!filesystem::file_exist(file.path().substr(0, file.path().size() - 4) + ".tex"))
				{
					//Creates a default .tex file

					std::ofstream f;
					f.open(file.path().substr(0, file.path().size() - 4) + ".tex");

					f<< R"({
	"min_filter"	: "nearest",
	"mag_filter"	: "nearest",
	"wrap_s"		: "repeat",
	"wrap_t"		: "repeat"
})";
				}
			}
		}

		for (auto file : get_all_files(directories_))
		{
			if (file.extension() == "mat")
			{
				materials_.emplace(filesystem::filename(file.path()), renderer_->load_material(file.path()));
			}

			if (file.extension() == "tex")
			{
				load_texture(file.path());
			}

			if(file.extension()=="fnt")
			{
				fonts_.try_emplace(filesystem::filename(file.path()), filesystem::filename(file.path()), file.path());
			}

			if(file.extension()=="wav")
			{
				sounds_.try_emplace(filesystem::filename(file.path()), file.path());
			}

			if(file.extension()=="mp3")
			{
				sounds_.try_emplace(filesystem::filename(file.path()), file.path());
			}
		}
	}

void ResourcesLoader::finalize()
{
	// Here I'm using log_warning just to change the color for this section so it's
	// easier to find it inside the console
	log_warning("Start cleaning up ResourcesLoader");
		
	log_message("Cleaning up textures");
	textures_.clear();

	log_message("Cleaning up materials");
	materials_.clear();

	log_message("Cleaning up directories");
	directories_.clear();

	log_message("Cleaning up fonts");
	fonts_.clear();
	log_warning("Finished cleaning up ResourcesLoader");
}

Material* ResourcesLoader::material(const std::string& name)
{
	if (!has_material(name))
	{
		log_error("The application could not load the material called \""+name+"\"");
		return nullptr;
	}
	return &materials_.at(name);
}

Font* ResourcesLoader::font(const std::string& name)
{
	if(!has_font(name))
	{
		log_error("The application could not load the font called \"" + name + "\"");
		return nullptr;
	}
	return &fonts_.at(name);
}

Sound* ResourcesLoader::sound(const std::string& name)
{
	return &sounds_.at(name);
}

	bool ResourcesLoader::has_texture(const std::string& name)noexcept
{
	return textures_.count(name) > 0;
}

bool ResourcesLoader::has_material(const std::string& name)noexcept
{
	return materials_.count(name) > 0;
}

bool ResourcesLoader::has_font(const std::string& name) noexcept
{
	return fonts_.count(name) > 0;
}

Texture* ResourcesLoader::texture(const std::string& name)
{
	if(!has_texture(name))
	{
		log_error_on("The application could not load the texture called \""+name+"\"", "ResourcesLoader");
		return nullptr;
	}
	return &textures_.at(name);
}

Texture* ResourcesLoader::new_texture(const std::string& name)
{
	if (has_texture(name))
	{
		log_error_on("The application could not create a new texture called \""+name+"\" : That texture already exist","ResourcesLoader");
		return nullptr;
	}

	return &textures_[name];
}

Material* ResourcesLoader::new_material(const std::string& name)
{
	if (has_material(name))
	{
		log_error_on("The application could not create a new material called \"" + name + "\" : That material already exist", "ResourcesLoader");
		return nullptr;
	}

	return &materials_[name];
}
}
