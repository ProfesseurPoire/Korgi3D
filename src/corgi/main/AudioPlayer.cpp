#include "AudioPlayer.h"

#include <SDL.h>
#include <SDL_mixer.h>

#include <corgi/logger/log.h>
#include <corgi/resources/Sound.h>

#include <iostream>

void corgi::AudioPlayer::initialize()
{
	//Initialize SDL_mixer 
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	{
		log_error("Could not initialize SDL mixer");
	}
}

void corgi::AudioPlayer::finalize()
{
	Mix_CloseAudio();
}

void corgi::AudioPlayer::play(const Sound& sound, int channel)
{
	if(sound.is_wav())
	{
		if(Mix_PlayChannel(channel,static_cast<Mix_Chunk*>(sound.sound_file_), 0) == -1)
		{
			log_error("Could not load wav file");
		}
	}
	else
	{
		if (Mix_PlayMusic(static_cast<Mix_Music*>(sound.sound_file_), -1) == -1)
		{
			log_error("Could not play music file");
		}
	}
}
