#pragma once

#include <memory>
#include <corgi/rendering/texture.h>

namespace corgi
{
	// Goal here is to reuse this when I make animations and other stuff
	class Sprite
	{
	public:

		// Todo : Maybe rethink the Sprite class and use unsigned short here
		// instead of floats to gain some space (then later on it is converted
		// to float when sent to the gpu as texture coordinate?)

		// Width, height and offset's unit is pixels
		float width		{0.0f};
		float height	{0.0f};

		float offset_x	{ 0.0f };
		float offset_y	{ 0.0f };
		Texture* texture = nullptr;
	};
}