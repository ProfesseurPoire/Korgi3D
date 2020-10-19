#pragma once

#include <corgi/components/RendererComponent.h>
#include <corgi/rendering/Sprite.h>
#include <corgi/math/Vec2.h>

namespace corgi
{
	class Texture;

	enum class Pivot : int
	{
		Center		= 0,
		Down		= 1,
		Right		= 2,
		Left		= 3,
		Up			= 4,
		RightUp		= 5,
		LeftUp		= 6,
		LeftDown	= 7,
		RightDown	= 8
	};

	class SpriteRenderer : public RendererComponent
	{
		friend class Renderer;

	public:

	//  Lifecycle

		/*!
		 * @brief	Creates a sprite renderer and attach it to the given entity
		 */
		SpriteRenderer();

		SpriteRenderer(Pivot pivot);

		SpriteRenderer(SpriteRenderer&& sr) noexcept;
		SpriteRenderer& operator=(SpriteRenderer&& sr)noexcept;

	//  Functions

		// Uses a texture as a sprite
		void texture(Texture* texture);

		// The pivot thing tells the renderer how he should actually 
		void pivot(Pivot pivot);

		// I'm not really supposed to change the sprite value after the sprite
		// renderer has been assigned one. Though be wary that the underlying mesh
		// used by the SpriteRenderer is only updated when you call 
		// the set_sprite function
		void set_sprite(Sprite sprite);

		void flip_sprite(bool value);

		bool flipX		{ false };
		bool flipY		{ false };
		float alpha		{ 1.0f };

		// I'm not sure if I should keep the offset thing or not, just add
		// complexity for something that can be done in the transform component
		float offset_x	{ 0.0f };
		float offset_y	{ 0.0f };
		float offset_z	{ 0.0f };

		Vec2 pivot_value;

		void enable()
		{
			is_enabled_=true;
		}
		
		void disable()
		{
			is_enabled_=false;
		}

		bool is_enabled()const;

	private:

		bool is_enabled_=false;

		Sprite _sprite;
	};
}