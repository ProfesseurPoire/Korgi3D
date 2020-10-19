#include <corgi/filesystem/FileSystem.h>


#include <stdarg.h>
#include <filesystem>
#include <iostream>

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

	bool file_exist(const std::string& path)
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
		int index_last  = find_last(path, '/');
		int other       = find_last(path, '\\');

		int final_index = other;

		if (index_last > other)
		{
			final_index = index_last;
		}

		if (final_index == -1)
			return path;

		return path.substr(0, final_index);
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

	std::string filename(const std::string& path)
	{
		for (size_t i = path.size() - 1; i > 0; --i)
			if (path[i] == '/' || path[i] == '\\')
				return path.substr(i + 1, std::string::npos);	//npos means until the end of the string
		return std::string();
	}

	FileInfo file_info(const std::string& path)
	{
		FileInfo info;
		info._path = path;
		return info;
	}

	std::string extension(const std::string& path)
	{
		std::string str(path);
		std::string ext;
		bool start_ext = false;

		for (size_t i = 0; i < str.size(); ++i)
		{
			if (start_ext)
				ext += str[i];
			if (str[i] == '.')
				start_ext = true;
		}
		return ext;
	}

	bool is_directory(const std::string& path)
	{
		return std::filesystem::is_directory(path);
	}

	bool FileInfo::is_valid()const
	{
		return _is_valid;
	}

	bool FileInfo::is_folder()const 
	{
		return is_directory(_path);
	}

	std::string FileInfo::extension()const
	{
		return filesystem::extension(_path);
	}

	std::string FileInfo::name()const
	{
		return filename(_path);
	}

	const std::string& FileInfo::path()const
	{
		return _path;
	}
}}