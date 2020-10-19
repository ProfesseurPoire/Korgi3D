#include <filesystem>
#include <iostream>
#include <fstream>
#include <stdarg.h>
#include <vector>
#include <string>

// #include <corgi/utils/ResourcesLoader.h>
// #include <corgi\utils\AsepriteImporter.h>
// #include <corgi\components\Animator.h>

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