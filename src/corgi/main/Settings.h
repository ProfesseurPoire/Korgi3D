#pragma once

#include <corgi/Ini/Document.h>

#include <string>

namespace corgi
{
	/*!
	 * @brief	Loads properties from a Setting.ini file located inside the project's resource folder and
	 *			gives an interface to access them from anywhere in the game
	 *			
	 *			A property has a name and a value, and can be grouped into sections
	 */
	class Settings
	{
		friend class Game;
		
	public :

		/*!
		 * @brief	Get the value of the property called @a name 
		 */
		static const float& get_float(const std::string& name, const std::string& group = "no_section");
		static const int& get_int(const std::string& name, const std::string& group = "no_section");
		static const std::string& get_string(const std::string& name, const std::string& group = "no_section");


	private:

		//  Must and can only be initialized by the Game
		static void initialize(const std::string& path);
		static void refresh();
	};
}
