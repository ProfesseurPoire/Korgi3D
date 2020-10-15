#pragma once

#include <vector>
#include <string>
#include <optional>
#include <exception>

/*
 *  The filesystem namespace contains functions and structures meant
 *  to help with file operations. At the moment it's mostly a wrapper for 
 *  std::filesystem. But if a platform doesn't support C++ 17, I'll "just"
 *  have to add a compilation flag and use system calls for this specific
 *  platform
 */
namespace corgi::filesystem 
{
	class FileInfo;

	/*!
	 * @brief   Checks if the file at the given path is a directory
	 *
	 * @param   path Path to the file we want to check
	 *
	 * @return  Returns true if the file is a directory, false otherwise
	 */
	[[nodiscard]] bool is_directory(const std::string& filepath);

	/*!
	 *  @brief  Creates and returns a FileInfo object for a given file
	 *
	 *  @param  path    Path to the file or directory we want to retrieve 
	 *                  information
	 *
	 *  @return Returns a @ref File object storing information about the file
	 */
	[[nodiscard]] FileInfo file_info(const std::string& filepath);

	/*!
	 * @brief   List the files inside the given directory.
	 *          
	 *          Creates a @ref FileInfo object for each file that the function
	 *          finds and adds it to a std::vector that will be returned. 
	 *              
	 * @param   directory   Path to the directory to be scanned
	 *
	 * @return  Returns a vector of FileInfo object. Each file found by
	 *          the function will add a new FileInfo object to the vector.
	 *          Empty if the directory doesn't exist or can't be opened
	 */
	[[nodiscard]] std::vector<FileInfo> list_directory(const std::string& directory, bool recursive=false);

	/*!
	 * @brief   Renames or moves a file or directory 
	 *
	 *          The function will move or rename the file or directory located
	 *          at @ref oldPath to @ref newPath.
	 *          The function will fail if newPath already exist
	 *
	 * @param   oldPath     Path to the file or folder that will be renamed
	 *                      or move
	 * @param   newPath     Path where the file or directory located at oldPath 
	 *                      will be move, or its new name
	 *
	 * @return  Returns true if the operation succeeded 
	 */
	bool rename(const std::string& oldPath, const std::string& newPath);

	/*!
	 * @brief   Returns the file's extension @a filepath
	 * 
	 *          extension("textures/frog.png"); will returns "png". 
	 * 
	 * 			Throw an invalid_argument extension if no extension could be found
	 *
	 * @param   filepath Path to the file we want to extract the extension
	 * 
	 * @return  Returns a std::optional that contains the extension 
	 */
	[[nodiscard]] std::string extension(const std::string& filepath);

	/*!
	 * @brief 	Returns true if @a filepath contains an extension
	 */
	[[nodiscard]] bool has_extension(const std::string& filepath);

	/*!
	 * @brief   Returns the filename part of the given path
	 * 
	 *          filename("textures/frog.png"); will returns "frog.png"
	 *
	 * @param   path    Path to the file we want to extract the filename
	 * @param 	with_extension 	True by default, if set to false, the function will
	 * 			return the filename without the extension
	 *
	 * @return  Returns a std::string with the file's name
	 */
	[[nodiscard]] std::string filename(const std::string& path, bool with_extension=true);

	/*!
	 * @brief   Returns the directory part of the given path
	 *
	 *          directory("textures/frog.png"); will return "textures"
	 *
	 * @param   path    Path to be worked on
	 *
	 * @return  Returns a std::string with he directory path
	 */
	[[nodiscard]] std::string directory(const std::string& path);

	/*!
	 * @brief   Returns the path without the file's name
	 * 
	 *          pathWithoutName("textures/frog.png"); will returns "textures/"
	 *
	 * @param   path    Path to the file we want to extract the data
	 *
	 * @return  Returns a std::string with the path without the name
	 */
	[[nodiscard]] std::string path_without_name(const std::string& path);

	/*!
	 * @brief   Remove the file or empty directory located at the given path
	 *
	 *          If the directory isn't empty, the operation will fail. You
	 *          should use removeAll instead.
	 *
	 * @param   path    Path to the file or empty directory to be removed
	 *
	 * @return  Returns true the operation succeeded, false otherwise
	 */
	bool remove(const std::string& path);

	/*!
	 * @brief   Remove the file or directory located at the given path
	 *
	 *          removeAll will work even if the file isn't an empty
	 *          directory
	 *
	 * @param   path    Path to the file or directory to be removed
	 *
	 * @return  Returns true if the operation succeeded, false otherwise
	 */ 
	void remove_all(const std::string& path);

	/*!
	 * @brief  Check if a file exist a given path
	 * 
	 *  @param      path    Path to the file to be checked for existence
	 *
	 *  @return     Returns true if the file exist, false otherwise
	 */
	[[nodiscard]] bool exists(const std::string& path);

	/*!
	 * @brief 	Returns the filepath without the extension (if any)
	 * 
	 * 			For instance, "foo/bar/stuff.png" will return "foo/bar/stuff"
	 */
	[[nodiscard]] std::string no_extension(const std::string& filepath);

	/*!
	 * @brief   Creates a new directory a given path
	 * 
	 * @param   filepath    Path where the new directory must be created
	 *
	 * @return  Returns true if a directory was created, false otherwise
	 */
	bool create_directory(const std::string& filepath);

	// TODO : Not exactly sure this is really used somewhere

	/*!
	 * @brief   Stores information about a File
	 *          
	 *          A FileInfo object is only a snapshot of a file at a given
	 *          moment. It won't track any changes made to the file it refers
	 *          to.
	 *
	 *          The file is also read only. And can only be created by 
	 *          filesystem functions
	 */
	class FileInfo
	{
	public:

		friend FileInfo file_info(const std::string& path);
		friend std::vector<FileInfo> list_directory(const std::string& directory);

		[[nodiscard]] bool is_valid()const;
		[[nodiscard]] bool is_folder()const;

		[[nodiscard]] std::optional<std::string> extension() const;
		[[nodiscard]] std::string name()const;

		[[nodiscard]] const std::string& path()const;

	protected:

		FileInfo() = default;

		bool is_valid_  = false;
		bool is_folder_ = false;

		std::string filepath_;
	};
}