#pragma once

#include <corgi/resources/Font.h>
#include <corgi/String.h>

namespace corgi
{
	class Text
	{
		public:

			Font* font{ nullptr };
			String str{ "Everything changed when the\nFire nation attacked!" };
			float r{ 1.0f }, g{ 1.0f }, b{ 1.0f }, a{ 1.0f };
	};
}