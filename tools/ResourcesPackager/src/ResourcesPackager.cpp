#include "ResourcesPackager.h"

#include <corgi/filesystem/FileSystem.h>

#include <corgi/rapidjson/rapidjson.h>
#include <corgi/rapidjson/document.h>
#include <corgi/rapidjson/filereadstream.h>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <stdarg.h>
#include <vector>
#include <string>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

namespace corgi
{
    void export_image(const std::string& source_file, const std::string& output_file)
    {
        int x, y, channels;

        // Images are horizontal on OpenGL otherwise
        stbi_set_flip_vertically_on_load(true);

        stbi_uc* imageData = stbi_load(source_file.c_str(), &x, &y, &channels
            , STBI_rgb_alpha);

        // Now we're simply going to write things into a file

        const auto dir = corgi::filesystem::directory(output_file);

        

        std::filesystem::create_directory(dir);

        //std::ofstream file;

        //auto directory  = corgi::filesystem::directory(filepath);
        //auto filename   = corgi::filesystem::filename(filepath, true);

        //file.open(output_file);
    }

    void convert_png(const std::string& resource_folder, const std::string& filepath)
    {
        if (!filepath.empty())
        {
            export_image(resource_folder+"/"+filepath, resource_folder+"/bin/"+filepath);

            // // So apparently this is where I actually 
            // std::ifstream file(path.c_str(), std::ifstream::in | std::ifstream::binary);

            // int fileSize = 0;

            // if(!file.is_open())
            // {
            //     throw("Could not open file for texture");
            // }

            // if (file.is_open())
            // {
            //     file.seekg(0, std::ios::end);
            //     fileSize = int(file.tellg());
            //     file.close();
            // }

            // FILE* fp = fopen(path.c_str(), "rb"); // non-Windows use "r"

            // char* readBuffer = new char[fileSize];

            // rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
            // rapidjson::Document document;
            // document.ParseStream(is);

            // assert(document.HasMember("wrap_s"));
            // assert(document.HasMember("wrap_t"));
            // assert(document.HasMember("min_filter"));
            // assert(document.HasMember("mag_filter"));

            // corgi::Image* image = Editor::Utils::LoadImageForReal(
            //     (path.substr(0, path.size() - 4) + ".png").c_str());

            // name_ 	= filesystem::filename((path.substr(0, path.size() - 4) + ".png"));
            // width_ 	= image->width();
            // height_ = image->height();
            
            // min_filter_ = parse_min_filter(document["min_filter"].GetString());
            // mag_filter_ = parse_mag_filter(document["mag_filter"].GetString());

            // wrap_s_ 	= load_wrap(document["wrap_s"].GetString());
            // wrap_t_		= load_wrap(document["wrap_t"].GetString());

            // id_ 		= RenderCommand::generate_texture_object();

            // RenderCommand::bind_texture_object(id_);

            // if (image->channel() == 3)
            // {
            //     RenderCommand::initialize_texture_object
            //     (
            //         Format::RGB, InternalFormat::RGB,
            //         width_, height_,
            //         DataType::UnsignedByte,
            //         image->pixels()
            //     );
            // }

            // if (image->channel() == 4)
            // {
            //     RenderCommand::initialize_texture_object
            //     (
            //         Format::RGBA, InternalFormat::RGBA,
            //         width_, height_,
            //         DataType::UnsignedByte,
            //         image->pixels()
            //     );
            // }

            // RenderCommand::texture_parameter(min_filter_);
            // RenderCommand::texture_parameter(mag_filter_);
            // RenderCommand::texture_wrap_s(wrap_s_);
            // RenderCommand::texture_wrap_t(wrap_t_);
            // RenderCommand::end_texture();	

            // delete image;
        }
        else
        {
            throw("Could not construstruct the thing");
        }
    }

    bool ResourcePackager::convert(const std::string& filepath)
    {
        const auto complete_file_path = resource_folder_+filepath;

        if(corgi::filesystem::exists(complete_file_path))
        {
            return false;
        }

        try
        {
            const auto extension = corgi::filesystem::extension(complete_file_path);
            
            if(extension == "png")
            {
                convert_png(resource_folder_, filepath);
            }
        }
        catch(std::invalid_argument e) 
        {
            // The Resource folder shouldn't have extensionless files so we log it
            std::cout<<e.what()<<std::endl;
        }
        return true;
    }
}

// #include <corgi/utils/ResourcesLoader.h>
// #include <corgi\utils\AsepriteImporter.h>
// #include <corgi\components\Animatorh>

// class FileInfo
// {
// public:

// 	FileInfo() = default;

// 	bool _is_valid = false;
// 	bool _is_folder = false;

// 	std::string _path;
// };

// std::string filename(const std::string& path)
// {
// 	for (size_t i = path.size() - 1; i > 0; --i)
// 		if (path[i] == '/' || path[i] == '\\')
// 			return path.substr(i + 1, std::string::npos);	//npos means until the end of the string
// 	return std::string();
// }

// std::string extension(const std::string& path)
// {
// 	std::string str(path);
// 	std::string ext;
// 	bool start_ext = false;

// 	for (size_t i = 0; i < str.size(); ++i)
// 	{
// 		if (start_ext)
// 		{
// 			ext += str[i];
// 		}
// 		if (str[i] == '.')
// 		{
// 			start_ext = true;
// 		}
// 	}
// 	return ext;
// }

// FileInfo file_info(const std::string& path)
// {
// 	FileInfo info;
// 	info._path = path;
// 	return info;
// }

// std::vector<FileInfo> list_directory(const std::string& directory, bool recursive)
// {
// 	std::vector<FileInfo> files;

// 	for (auto& p : std::filesystem::directory_iterator(directory))
// 	{
// 		files.push_back(file_info(p.path().string()));

// 		// If the file is a directory, and the function was called with
// 		// recursive set to true, we simply call list_directory again
// 		// on that directory, and append what the function returns
// 		if (recursive && p.is_directory())
// 		{
// 			auto f = list_directory(p.path().string(), recursive);
// 			files.insert(files.end(), f.begin(), f.end());
// 		}
// 	}
// 	return files;
// }

// int main(int argc, char** argv)
// {
// 	// Needs a window
// 	corgi::Renderer	renderer();


// 	auto files = list_directory(argv[1], true);

// 	std::vector<std::string> textures;
// 	std::vector<std::string> animations;

// 	for (auto file : files)
// 	{
// 		if (extension(file._path) == "png")
// 		{
// 			textures.push_back(filename(file._path));
// 		}

// 		if (extension(file._path) == "json")
// 		{
// 			animations.push_back(file._path);
// 		}
// 	}

// 	std::string output_file = argv[2];
// 	output_file += "Resources.h";

// 	std::ofstream file(output_file);
// 	std::string str = R"(
// #pragma once

// #include <string>
// #include <vector>

// #include<corgi/utils/ResourcesLoader.h>

// namespace frog
// {
// 	namespace Resources
// 	{
// )";

// 	file << str;

// 	file << "\t\tnamespace Textures\n{\n";

// 	for (auto& texture : textures)
// 	{
// 		std::string filename_without_extension = texture.substr(0, texture.size() - 4);

// 		file << "\t\t\tstatic inline corgi::Texture* " << filename_without_extension << "(){return corgi::ResourcesLoader::texture(\"" << texture << "\");}\n";
// 	}
// 	file << "\t\t} \n";

// 	file << "\t\tnamespace Animations\n{\n";

// 	for (auto& animation : animations)
// 	{
// 		std::string filename_without_extension = filename(animation.substr(0, animation.size() - 5));

// 		corgi::AsepriteImporter importer;
// 		importer.load(animation);

// 		file << "\t\t\tnamespace " << filename_without_extension << "\n";
// 		file << "\t\t\t{\n";

// 		for (auto& tag : importer.meta.frame_tags)
// 		{
// 			file << "\t\t\t\tstatic inline corgi::Animation " << tag.name << "()\n";
// 			file << "\t\t\t\t{\n";
// 			file << "\t\t\t\t\tstatic auto aa = corgi::ResourcesLoader::load_animations(\""<<filename_without_extension<<"\");\n";
// 			file << "\t\t\t\t\treturn *std::find_if(aa.begin(), aa.end(), [&](const corgi::Animation& a) { return a.name() == \""<<tag.name<<"\"; });\n";
// 			file << "\t\t\t\t}\n";
// 		}
// 		file << "\t\t\t}\n";
// 	}
// 	file << "}}}";
// }