#pragma once

namespace corgi
{
	struct Panel
	{
		Texture * texture;

		float r, g, b, a;
		float width, height;

		float offset;
		float textureOffset;
	};
}