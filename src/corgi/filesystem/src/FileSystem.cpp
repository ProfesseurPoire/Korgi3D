#include <corgi/filesystem/FileSystem.h>


#include <stdarg.h>
#include <filesystem>
#include <iostream>
#include <algorithm>

namespace corgi { namespace filesystem {

	std::vector<FileInfo> list_directory(const std::string& directory, bool recursive)
	{
		std::vector<FileInfo> files;

		for (auto& p : std::filesystem::directory_iterator(directory))
		{
			files.push_back(file_info(p.path().string()));

			// If the file is a directory, and the function was called with
			// recursive set to true, we simply call list_directory again
			// on that directory, and append what the function returns
			if (recursive && p.is_directory())
			{
				auto f = list_directory(p.path().string(), recursive);
				files.insert(files.end(), f.begin(), f.end());
			}
		}
		return files;
	}

	bool remove(const std::string& path)
	{
		return std::filesystem::remove(path);
	}

	bool create_directory(const std::string& path)
	{
		return std::filesystem::create_directory(path);
	}

	void remove_all(const std::string& path)
	{
		std::filesystem::remove_all(path);
	}

	bool exists(const std::string& path)
	{
		return std::filesystem::exists(path);
	}

	/*
	 * @brief  Find the last occurrence of character c in str
	 */
	int find_last(const std::string& str, char c...)
	{
		for (size_t i = str.size() - 1; i >= 0; i--)
		{
			va_list args;
			va_start(args, c);
			if (str[i] == c)
			{
				va_end(args);
				return i;
			}
			va_end(args);
		}
		return-1;
	}

	std::string directory(const std::string& path)
	{
		auto index_last = path.find_last_of('/');
		auto other 		= path.find_last_of('\\');
		
		if(other==std::string::npos && index_last==std::string::npos)
		{
			throw std::invalid_argument("No directory delimitator found");
		}

		if(index_last!=std::string::npos && other==std::string::npos)
		{
			return path.substr(0, index_last);
		}

		if(index_last==std::string::npos && other!=std::string::npos)
		{
			return path.substr(0, other);
		}

		if( index_last> other)
		{
			return path.substr(0, index_last);
		}

		if(other > index_last)
		{
			return path.substr(0, other);
		}	

		return "";	
	}

	std::string no_extension(const std::string& filepath)
	{
		auto character_index = filepath.find_last_of('.');

		// We throw an exception if we didn't find the character
		if(character_index==std::string::npos)	
		{
			throw std::invalid_argument(("No extension found in filepath : " + filepath).c_str());
		}

		// we throw an exception if the dot is the last char of the string
		if(character_index== filepath.size()-1)
		{
			throw std::invalid_argument(("No extension found after the dot character : "+filepath).c_str());
		}

		return filepath.substr(0, character_index);
	}

	bool rename(const std::string& path, const std::string& newPath)
	{
		std::error_code errorCode;
		std::filesystem::rename(path, newPath, errorCode);
		std::cout << errorCode.message() << std::endl;
		return true;
	}

	std::string path_without_name(const std::string& path)
	{
		// we look for the first "/" or "\" if the path from the end
		const int index = find_last(path, '/', '\\');

		// If index equals -1 it means there's nothing before the filename
		if (index == -1)
			return std::string();

		return path.substr(0, index + 1);
	}

	std::string filename(const std::string& path, bool with_extension)
	{
		std::string str;

		for (size_t i = path.size() - 1; i > 0; --i)
		{
			if (path[i] == '/' || path[i] == '\\')
			{
				str = path.substr(i + 1, std::string::npos);	//npos means until the end of the string
			}
		}

		if(!with_extension)
		{
			for(auto it = str.crbegin(); it != str.crend(); it++)
			{
				if(*it == '.')
				{
					str = str.substr(0, (it - str.crend()));
				}
			}
		}
		return str;
	}

	// Can be made in 1 line
	FileInfo file_info(const std::string& path)
	{
		FileInfo info;
		info.filepath_ = path;
		return info;
	}
	
	std::string extension(const std::string& filepath)
	{
		auto character_index = filepath.find_last_of('.');

		// We throw an exception if we didn't find the character
		if(character_index==std::string::npos)	
		{
			throw std::invalid_argument(("No extension found in filepath : " + filepath).c_str());
		}

		// we throw an exception if the dot is the last char of the string
		if(character_index== filepath.size()-1)
		{
			throw std::invalid_argument(("No extension found after the dot character : "+filepath).c_str());
		}

		return filepath.substr(character_index+1, std::string::npos);
	}

	bool has_extension(const std::string& filepath)
	{
		auto character_index = filepath.find_last_of('.');

		// If we don't find a . character, there's no extension
		if(character_index == std::string::npos)	
		{
			return false;
		}

		// if the . character is the last character of the string, there's still no character
		if(character_index == filepath.size()-1)
		{
			return false;
		}
		return true;
	}

	bool is_directory(const std::string& path)
	{
		return std::filesystem::is_directory(path);
	}

	bool FileInfo::is_valid()const
	{
		return is_valid_;
	}

	bool FileInfo::is_folder()const 
	{
		return is_directory(filepath_);
	}

	std::string FileInfo::name()const
	{
		return filename(filepath_);
	}

	const std::string& FileInfo::path()const
	{
		return filepath_;
	}
}}