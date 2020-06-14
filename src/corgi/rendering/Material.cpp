#include "Material.h"

#include <corgi/logger/log.h>

namespace corgi
{
	void Material::add_texture(Texture* texture, const std::string& name)
	{
		textures_[name] = texture;
	}

	bool Material::operator==(const Material& m) const
	{
		if (shader_program != m.shader_program)
			return false;

		if (textures_ != m.textures_)
			return false;

		if (enable_depth_test != m.enable_depth_test)
			return false;

		if (depth_mask != m.depth_mask)
			return false;

		return true;
		// Todo : Etc
	}


	bool Material::operator<(const Material& m)const
	{	
		// is_transparent and render queue are not used
		// here because they are used before the sort by the renderer
		if (shader_program != m.shader_program)
			return shader_program < m.shader_program;

		// Not really sure how to sort on texture now ...
		if (textures_ != m.textures_)
			return textures_ < m.textures_;

		if (enable_depth_test != m.enable_depth_test)
			return enable_depth_test < m.enable_depth_test;

		if (enable_blend != m.enable_blend)
			return enable_blend < m.enable_blend;

		if (depth_mask != m.depth_mask)
			return depth_mask < m.depth_mask;

		if (mode != m.mode)
			return mode < m.mode;

		//if (!(ambient_color.equal(m.ambient_color)))
		//{
		//	if (ambient_color.x != m.ambient_color.x)
		//		return ambient_color.x < m.ambient_color.x;
		//	if (ambient_color.y != m.ambient_color.y)
		//		return ambient_color.y < m.ambient_color.y;
		//	if (ambient_color.z != m.ambient_color.z)
		//		return ambient_color.z < m.ambient_color.z;
		//}
/*
		if (!(color == m.color))
		{
			if (color.x != m.color.x)
				return color.x < m.color.x;
			if (color.y != m.color.y)
				return color.y < m.color.y;
			if (color.z != m.color.z)
				return color.z < m.color.z;
			if (color.w != m.color.w)
				return color.w;
		}
*/

		return false;
	}

	Material::Material()
		:name_("empty")
	{

	}

	Material::Material(const std::string& name)
		:name_(name)
	{
	}

	Material::~Material()
	{
	}

	void Material::remove_uniform(const std::string& name)
	{
		uniforms_.erase(name);
	}
}