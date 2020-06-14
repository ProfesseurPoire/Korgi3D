#include "SpriteRenderer.h"

#include <corgi/rendering/renderer.h>
#include <corgi/rendering/texture.h>

#include <corgi/utils/ResourcesLoader.h>

namespace corgi
{
	SpriteRenderer::SpriteRenderer()
		: RendererComponent(nullptr, ResourcesLoader::material("unlit_texture.mat"))
	{
		_mesh		= Mesh::new_standard_mesh();
		
		material.enable_depth_test	= true;
		material.is_transparent		= true;
		material.set_uniform("flat_color", Vec4(2.0f, 2.0f, 2.0f, 1.0f));
		material.set_uniform("use_flat_color", 0);

		_mesh->vertices().resize(8*4, 0.0f);
		
		_mesh->add_triangle(0, 1, 2);
		_mesh->add_triangle(0, 2, 3);

		_mesh->update_vertices();
	}

	// TODO : Would be nice to have a sort of "list" of materials, like I tried to do
	// for textures, even though it doesn't exactly works :joy:
	SpriteRenderer::SpriteRenderer(Pivot piv)
		: RendererComponent(nullptr, ResourcesLoader::material("unlit_texture.mat"))
	{
		_mesh = Mesh::new_standard_mesh();

		material.enable_depth_test = true;
		material.is_transparent = true;
		material.set_uniform("flat_color", Vec4(2.0f, 2.0f, 2.0f, 1.0f));
		material.set_uniform("use_flat_color", 0);
		pivot(piv);
		_mesh->vertices().resize(8 * 4, 0.0f);

		_mesh->add_triangle(0, 1, 2);
		_mesh->add_triangle(0, 2, 3);

		_mesh->update_vertices();
	}

	// TODO : would be nice to directly initialize the things inside the initializer list
	// so, add a noice constructor in component and RendererComponent one day
	SpriteRenderer::SpriteRenderer(SpriteRenderer&& sr) noexcept
		: RendererComponent(nullptr),	pivot_value(sr.pivot_value)
	{
		_mesh		= sr._mesh;
		material	= sr.material;

		_mesh->update_vertices();

		//entity_			= sr.entity_;
		_sprite			= sr._sprite;
	}

	SpriteRenderer& SpriteRenderer::operator=(SpriteRenderer&& sr) noexcept
	{
		_mesh		= sr._mesh;
		material	= sr.material;

		_mesh->update_vertices();

		pivot_value = sr.pivot_value;
		//entity_ = sr.entity_;
		_sprite = sr._sprite;

		return *this;
	}

	void SpriteRenderer::texture(Texture* texture)
	{
		_sprite.width	= static_cast<float>(texture->width());
		_sprite.height	= static_cast<float>(texture->height());
		_sprite.texture = texture;
		set_sprite(_sprite);
	}

	constexpr static Vec2 pivots_[] =
	{
		{Vec2(0.5f, 0.5f)},	// Pivot::Center
		{Vec2(0.5f, 0.0f)},	// Pivot::Down
		{Vec2(1.0f, 0.5f)}, // Pivot::Right
		{Vec2(0.0f, 0.5f)}, // Pivot::Left
		{Vec2(0.5f, 1.0f)},	// Pivot::Up
		{Vec2(1.0f, 1.0f)},	// Pivot::RightUp
		{Vec2(0.0f, 1.0f)}, // Pivot::LeftUp
		{Vec2(0.0f, 0.0f)}, // Pivot::LeftDown
		{Vec2(1.0f, 0.0f)}	// Pivot::RightDown
	};

	void SpriteRenderer::pivot(Pivot pivot)
	{
		pivot_value = pivots_[static_cast<int>(pivot)];
	}

	void SpriteRenderer::set_sprite(Sprite sprite)
	{
		material.add_texture(sprite.texture,"main_texture");
		_sprite = sprite;

		// Also maybe this doesn't need to be computed all the time? Maybe it could
		// be cached on the Sprite class
		float offset_texture_x		= sprite.offset_x	/ static_cast<float>(sprite.texture->width());
		float offset_texture_y		= sprite.offset_y	/ static_cast<float>(sprite.texture->height());
		float texture_width_coef	= sprite.width		/ static_cast<float>(sprite.texture->width());
		float texture_height_coef	= sprite.height		/ static_cast<float>(sprite.texture->height());

		if (flipX)
		{
			offset_texture_x	= +sprite.offset_x	/ +sprite.texture->width() + sprite.width / sprite.texture->width();
			texture_width_coef	= -sprite.width		/ +sprite.texture->width();
		}

		auto& vertices = _mesh->vertices();

		vertices[0]		= offset_x - (sprite.width * pivot_value.x);
		vertices[1]		= offset_y - (sprite.height * pivot_value.y);
		vertices[2]		= offset_z;
		vertices[3]		= offset_texture_x;
		vertices[4]		= offset_texture_y;
		vertices[5]		= 0.0f;
		vertices[6]		= 0.0f;
		vertices[7]		= 1.0f;

		vertices[8]		= offset_x + (sprite.width * (1.0f - pivot_value.x));
		vertices[9]		= offset_y - (sprite.height * pivot_value.y);
		vertices[10]	= offset_z;
		vertices[11]	= offset_texture_x + texture_width_coef;
		vertices[12]	= offset_texture_y;
		vertices[13]	= 0.0f;
		vertices[14]	= 0.0f;
		vertices[15]	= 1.0f;

		vertices[16]	= offset_x + (sprite.width * (1.0f - pivot_value.x));
		vertices[17]	= offset_y + (sprite.height * (1.0f - pivot_value.y));
		vertices[18]	= offset_z;
		vertices[19]	= offset_texture_x + texture_width_coef;
		vertices[20]	= offset_texture_y + texture_height_coef;
		vertices[21]	= 0.0f;
		vertices[22]	= 0.0f;
		vertices[23]	= 1.0f;

		vertices[24]	= offset_x - (sprite.width * pivot_value.x);
		vertices[25]	= offset_y + (sprite.height * (1.0f - pivot_value.y));
		vertices[26]	= offset_z;
		vertices[27]	= offset_texture_x;
		vertices[28]	= offset_texture_y + texture_height_coef;
		vertices[29]	= 0.0f;
		vertices[30]	= 0.0f;
		vertices[31]	= 1.0f;
			 
		_mesh->update_vertices_really();
	}

	void SpriteRenderer::flip_sprite(bool value)
	{
		flipX = value;
		set_sprite(_sprite);
	}
}
