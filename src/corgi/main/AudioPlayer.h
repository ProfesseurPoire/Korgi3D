#pragma once

namespace corgi
{
	class Sound;
	
	class AudioPlayer
	{
	public:

		static void initialize();
		static void finalize();

		/*
		 * @brief Play a wav file
		 *
		 *@param channel	If channel equals to -1, the audio library will chose
		 *					the first channel available to play the sound
		 */
		static void play(const Sound& sound, int channel = -1);
	};
}