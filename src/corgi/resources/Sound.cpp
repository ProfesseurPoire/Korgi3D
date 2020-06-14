#include "Sound.h"

#include <SDL_mixer.h>

namespace corgi
{
	Sound::Sound(const std::string& file)
	{
		auto end = file.substr(file.size() - 3, 3);
		is_wav_ = end == "wav";

		if(is_wav_)
		{
			sound_file_ = Mix_LoadWAV(file.c_str());
		}
		else
		{
			sound_file_ = Mix_LoadMUS(file.c_str());
		}
	}

	Sound::~Sound()
	{
		if(is_wav_)
		{
			Mix_FreeChunk( (Mix_Chunk*) sound_file_);
		}
		else
		{
			Mix_FreeMusic(  (Mix_Music*)sound_file_);
		}
	}

	bool Sound::is_wav() const
	{
		return is_wav_;
	}
}
