#include "Font.h"

#include "config.h"


#include <corgi/resources/image.h>
#include <corgi/rendering/texture.h>
#include <corgi/logger/log.h>

#include <fstream>
#include <string>

namespace corgi
{
	Font::~Font()
	{
		delete texture;
	}

	Font::Font(const std::string& name, const std::string& file)
	{
		std::ifstream font_file(file, std::ifstream::binary);

		int width;
		int height;

		font_file.read((char*)&width, sizeof(width));
		font_file.read((char*)&height, sizeof(width));

		unsigned char* pixels = new unsigned char[width * height * 4];

		font_file.read((char*)pixels, width * height * 4);

		font_file.read((char*)characters, sizeof(characters) * 256);

		texture = new Texture(name, width, height, Texture::MinFilter::Nearest, Texture::MagFilter::Nearest,
			Texture::Wrap::Repeat, Texture::Wrap::Repeat, Texture::Format::RGBA, Texture::InternalFormat::RGBA,
			Texture::DataType::UnsignedByte, pixels);

		//char c = 'a';
		
		/*Sprite sprite;

		sprite.texture = texture;
		sprite.width = characters[c].bw;
		sprite.height = characters[c].bh;
		sprite.offset_x = characters[c].tx * texture->width();
		sprite.offset_y = texture->height() - characters[c].bh;

		_sprite_entity->get_component<SpriteRenderer>().set_sprite(sprite);*/
	}
}