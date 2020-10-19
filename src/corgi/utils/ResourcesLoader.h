#pragma once

#include <corgi/containers/Vector.h>
#include <corgi/string/String.h>
#include <corgi/resources/Animation.h> 

namespace corgi
{
	class Renderer;
	class Texture;
	class Image;
	class Font;
	class Mesh;
	class Sound;
	class Material;

	// Load and cache ResourcesLoader
	// The Resource class only load the resource files inside the ResourcesLoader
	// folders. So if you create a texture or a mesh during runtime, it won't
	// be saved here

	// Note : I'm making this a class only to exploit the protected and friend keyword

	// TODO : Maybe later on it would be nice not to load everything inside the resource
	// folder, but instead, have a file that tag every ResourcesLoader used by a scene
	// and automatically only load and unload the corresponding ResourcesLoader
	// Also, it would be even nicer to load precompiled assets in binary form
	// instead of loading raw png, jpeg, json or whatever

	// The ResourcesLoader class owns and manages the lifecycle of the game's ResourcesLoader.
	// Its function only returns non owning pointers
	class ResourcesLoader
	{
		friend class Game;

	public:

		/*!
		 * @brief	Look if a texture called @a name is stored in one of the resource folders
		 *
		 * @param	name	The name of the texture we wish to know whether it is stored
		 *
		 * @return	Returns true if a texture called name is found, false otherwise
		 */
		static bool has_texture(const String& name) noexcept;

		/*!
		 * @brief	Look if a material called @a name is stored in one of the resource folders
		 *
		 * @param	name	The name of the material we wish to know whether it is stored
		 *
		 * @return	Returns true if a material called name is found, false otherwise
		 */
		static bool has_material(const String& name)noexcept;

		/*!
		 * @brief	Look if a font called @name is stored in at one of the resource folders
		 *
		 * @param	name	The name of the material we wish to know whether it is stored
		 *
		 * @return	Returns true if a font called name is found, false otherwise
		 */
		static bool has_font(const String& name)noexcept;
		
		/*!
		 * @brief	Tries to load a stored texture called @a name. 
		 * 
		 *			This function will first check if a texture called @a name is stored. If not,
		 *			an error is logged and the function will return nullptr. Otherwise, the function
		 *			returns a non owning pointer to the requested texture
		 *
		 * @param	name	The name of the requested texture
		 *
		 * @return	Returns a non owning pointer to the requested texture. If the requested
		 *			texture could not be found, returns nullptr.
		 */
		static Texture* texture(const String& name);

		/*!
		 * @brief	Tries to load a stored material called @a name. 
		 *			
		 *			If no material could be found,
		 *			the function will return a nullptr and log an error
		 *
		 * @param	name	The name of the requested material
		 *
		 * @return	Returns a non owning pointer to the requested material.
		 *			Returns nullptr if the material could not be found
		 */
		static Material* material(const String& name);


		/*!
		 * @brief	Tries to load a stored font called @a name
		 *
		 *			If no font could be found, the function returns nullptr and log an error
		 *
		 * @param	name	Name of the requested font
		 *
		 * @return	Returns a non owning pointer to the requested font.
		 *			Returns nullptr if the font could not be found
		 */
		static Font* font(const String& name);

		static Sound* sound(const String& name);

		/*!
		 * @brief	Tries to create and stores a new empty texture with the given @a name. 
		 *			If a texture with the given name is already stored, the texture creation 
		 *			is canceled, an error is logged and the function returns nullptr 
		 *
		 * @param	name	The name of the new texture
		 *
		 * @return	Returns a non owning pointer to the newly created and stored texture.
		 *			Returns nullptr if a texture with the given name already exist.
		 */
		static Texture* new_texture(const String& name);

		/*!
		 * @brief	Tries to create and store a new empty material with the given @a name
		 *			If a material with the given @a name is already stored, the material
		 *			creation is canceled, an error is logged and the function returns nullptr
		 *
		 * @param	name	The name of the new material
		 *
		 * @return	Returns a non owning pointer to the nwly created and stored material.
		 *			Returns nullptr if a material with the given name already exist
		 */
		static Material* new_material(const String& name);

		/*!
		 * @brief	Adds a new resource directory to scan for content
		 *
		 * @param	path	The path to a new resource directory 
		 */
		static void add_directory(const String& path);

		/*!
		 * @brief	Tries to find the full path of a file stored inside one of the
		 *			resource directories at @a path. @path being relative to the root of one
		 *			of the resource directories.
		 *			
		 *			In other words, for each resource directory, this function will
		 *			check for the existence of a file located at resource_directory+path.
		 *			
		 *			If the file exist, we return the full path.
		 *
		 * @param	path	The path to a file we wish to know whether it exist or not in
		 *					one of the resource directories. Path is relative to the
		 *					resource directory
		 *
		 *@return	If a file could be found in one of the resource directories, returns it's
		 *			full path. Otherwise, returns an empty string
		 */
		static String find(const String& path);

		/*
		 * // TODO : Probably check a little bit on this because that looks a bit yolo
		 * 			 Like maybe return std::unique_ptr or something? Idk
		 * @brief	Tries to load animations
		 */
		static Vector<Animation> load_animations(const String& path);

		
		/*
		 *  The initialize and finalize functions are called by the Game 
		 *	We need an initialized renderer to load the textures and material
		 */
		static void initialize(Renderer& renderer);

		/*
		 * Free every loaded resource. Called when the game's closes
		 */
		static void finalize();
	};
}