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
    bool has_default_texture(const std::string& filepath)
    {
        return (corgi::filesystem::exists(corgi::filesystem::no_extension(filepath)+".tex"));
    }

    void create_default_texture(const std::string& source_file)
    {
        try
        {
            std::ofstream file;

            file.open(corgi::filesystem::no_extension(source_file)+".tex");

file<< R"({
    "min_filter"	: "nearest",
    "mag_filter"	: "nearest",
    "wrap_s"		: "repeat",
    "wrap_t"		: "repeat"
})";
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    void export_image(const std::string& source_file, const std::string& output_file)
    {
        try
        {
            int x, y, channels;

            // Images are horizontal on OpenGL otherwise
            stbi_set_flip_vertically_on_load(true);

            stbi_uc* imageData = stbi_load(source_file.c_str(), &x, &y, &channels
                , STBI_rgb_alpha);

            // Now we're simply going to write things into a file

            const auto dir = corgi::filesystem::directory(output_file);
            
            // ok so create directories works, nice
            std::filesystem::create_directories(dir);

            std::ofstream file;
        
            file.open(corgi::filesystem::no_extension(output_file)+".bin", std::ofstream::binary | std::ofstream::out);
            
            file.write((const char*)&x, sizeof(int));
            file.write((const char*)&y, sizeof(int));
            file.write((const char*)&channels, sizeof(int));
            file.write((const char*)imageData, x*y*channels);

            // I kinda wish to also have the texture data inside this crap  

            std::ifstream texture_file(corgi::filesystem::no_extension(source_file)+".tex", std::ifstream::in | std::ifstream::binary);

            int fileSize = 0;

            if(!texture_file.is_open())
            {
                throw("Could not open file for texture");
            }

            if (texture_file.is_open())
            {
                texture_file.seekg(0, std::ios::end);
                fileSize = int(texture_file.tellg());
                texture_file.close();
            }

            FILE* fp = fopen((corgi::filesystem::no_extension(source_file)+".tex").c_str(), "rb"); // non-Windows use "r"

            char* readBuffer = new char[fileSize];

            rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
            rapidjson::Document document;
            document.ParseStream(is);
 
            assert(document.HasMember("wrap_s"));
            assert(document.HasMember("wrap_t"));
            assert(document.HasMember("min_filter"));
            assert(document.HasMember("mag_filter"));

            // TODO : Use a code here instead of a string 
            file<< document["min_filter"].GetString()<<"\n";
            file<< document["mag_filter"].GetString()<<"\n";

            file<< document["wrap_s"].GetString();
            file<< document["wrap_t"].GetString();
            
            fclose(fp);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    void convert_png(const std::string& resource_folder, const std::string& filepath)
    {
        if (!filepath.empty())
        {
            try
            {
                if(!has_default_texture(resource_folder+"/"+filepath))
                {
                    create_default_texture(resource_folder+"/"+filepath);
                }
                export_image(resource_folder+"/"+filepath, resource_folder+"/bin/"+filepath);
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else
        {
            throw std::exception(("No files at filepath : "+filepath).c_str());
        }
    }

    void ResourcePackager::run()
    {
        clear();

        auto files = corgi::filesystem::list_directory(resource_folder_, true);
    
        for(auto file : files)
        {
            if(!file.is_folder())
            {
                convert(file.path().substr(resource_folder_.size()+1, std::string::npos));    
            }
        }
    }

    void ResourcePackager::clear()
    {
        std::filesystem::remove_all(resource_folder_+"/bin");
    }

    bool ResourcePackager::convert(const std::string& filepath)
    {
        const auto complete_file_path = filepath;

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