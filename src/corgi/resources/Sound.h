#pragma once

#include <string>

namespace corgi
{
	
	class Sound
	{
		
	public:

		friend class AudioPlayer;

	// Constructors
		
		Sound(const std::string& file);
		~Sound();

	// Functions

		bool is_wav()const;

		

	private:

		bool is_wav_ = false;
		
		void* sound_file_ = nullptr;
	};
}
