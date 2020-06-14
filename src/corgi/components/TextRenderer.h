#pragma once
#include <corgi/components/RendererComponent.h>

#include <corgi/resources/Font.h>
#include <corgi/string/String.h>

namespace corgi
{
	class Entity;

	class TextRenderer : public RendererComponent
	{
	public:

	// Lifecycle

		TextRenderer();

	//  Functions

		void font(Font* font);
		void text(const String& text);
		void scaling(float value);
		void offset(const Vec3& offset);

		enum class HorizontalAlignment
		{
			Left,
			Right,
			Centered
		};

		void horizontal_alignment(HorizontalAlignment alignment);

	private:

		String text_;
		Font* font_;
		float scaling_ = 1.0f;

		Vec3 offset_;
		HorizontalAlignment alignment_ = HorizontalAlignment::Left;
	};
}