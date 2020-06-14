#include "TextRenderer.h"

#include <corgi/rendering/texture.h>
#include <corgi/rendering/Material.h>
#include <corgi/utils/ResourcesLoader.h>

// TODO : 	I'm not sure it's a good thing to call the ResourceLoader
// 			just to get the "default" material. Maybe the default material
//			should be hard coded into the game?
corgi::TextRenderer::TextRenderer()
	: RendererComponent(nullptr, ResourcesLoader::material("unlit_texture.mat"))
{
	// TODO : Probably should have a material dedicated for
	// the Text renderer so I don't need to actually change things afterward
	material.enable_depth_test	= true;
	material.is_transparent		= true;
}

void corgi::TextRenderer::font(Font* font)
{
	font_ = font;
}

void corgi::TextRenderer::text(const String& text)
{
	text_ = text;

	if(font_!=nullptr)
	{
		material.add_texture(font_->texture);
		//material.add_texture(Resources::texture("Test.png"));

		int i = 0;
		_mesh = Mesh::new_standard_mesh();
		
		_mesh->vertices().resize(8 * 4 * (text.size()), 0.0f);
		
		for(auto c : text)
		{
			if(c!=' ')
			{
				_mesh->add_triangle(i, i + 1, i + 2);
				_mesh->add_triangle(i, i + 2, i + 3);
				i += 4;
			}
		}

		_mesh->update_vertices();

		i = 0;
		auto& vertices = _mesh->vertices();
		
		float offset = 0;
		
		for(auto c : text)
		{
			if(c ==' ')
			{
				offset += 20;
				i++;
				continue;
			}

			float v = (font_->texture->height() - font_->characters[c].bh) / font_->texture->height();
			float aa = ((float)font_->characters[c].bw / (float)font_->texture->width());

			const float glyph_x_offset	= font_->characters[c].bl * scaling_;
			const float glpyh_y_offset  = - (font_->characters[c].bh * scaling_ - font_->characters[c].bt * scaling_);
			const float glyph_width		= font_->characters[c].bw * scaling_;
			const float glyph_height	= font_->characters[c].bh * scaling_;
			
			vertices[i*32 + 0] = offset_.x +  offset + glyph_x_offset;
			vertices[i*32 + 1] = offset_.y + glpyh_y_offset;
			vertices[i*32 + 2] = offset_.z;
			vertices[i*32 + 3] = font_->characters[c].tx;
			vertices[i*32 + 4] = v;
			vertices[i*32 + 5] = 0.0f;
			vertices[i*32 + 6] = 0.0f;
			vertices[i*32 + 7] = 1.0f;
					  
			vertices[i*32 + 8]  = offset_.x + offset + glyph_x_offset + glyph_width;
			vertices[i*32 + 9]  = offset_.y + glpyh_y_offset;
			vertices[i*32 + 10] = offset_.z;
			vertices[i*32 + 11] = font_->characters[c].tx + aa;
			vertices[i*32 + 12] = v;
			vertices[i*32 + 13] = 0.0f;
			vertices[i*32 + 14] = 0.0f;
			vertices[i*32 + 15] = 1.0f;
					  
			vertices[i*32 + 16] = offset_.x + offset + glyph_x_offset + glyph_width;
			vertices[i*32 + 17] = offset_.y + glpyh_y_offset + glyph_height;
			vertices[i*32 + 18] = offset_.z;
			vertices[i*32 + 19] = font_->characters[c].tx + aa;
			vertices[i*32 + 20] = 1.0f;
			vertices[i*32 + 21] = 0.0f;
			vertices[i*32 + 22] = 0.0f;
			vertices[i*32 + 23] = 1.0f;
					  
			vertices[i*32 + 24] = offset_.x + offset + glyph_x_offset;
			vertices[i*32 + 25] = offset_.y + glpyh_y_offset + glyph_height;
			vertices[i*32 + 26] = offset_.z;
			vertices[i*32 + 27] = font_->characters[c].tx;
			vertices[i*32 + 28] = 1.0f;
			vertices[i*32 + 29] = 0.0f;
			vertices[i*32 + 30] = 0.0f;
			vertices[i*32 + 31] = 1.0f;
			
			offset += font_->characters[c].ax *scaling_;
			i++;
		}

		if(alignment_ == HorizontalAlignment::Centered)
		{
			for(size_t i=0; i< vertices.size()/8; ++i)
			{
				vertices[i * 8] -= offset / 2.0f;
			}
		}
		
		_mesh->update_vertices_really();
	}
}

void corgi::TextRenderer::scaling(float value)
{
	scaling_ = value;
}

void corgi::TextRenderer::offset(const Vec3& offset)
{
	offset_ = offset;
}

void corgi::TextRenderer::horizontal_alignment(HorizontalAlignment alignment)
{
	alignment_ = alignment;
}
