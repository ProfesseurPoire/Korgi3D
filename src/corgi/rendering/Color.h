#pragma once

namespace corgi
{
	struct Color
	{
		static const Color red;
		static const Color green;
		static const Color blue;
		static const Color white;

		float r{ 0.0f }, g{ 0.0f }, b{ 0.0f }, a{ 0.0f };
	};
}