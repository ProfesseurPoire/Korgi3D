#pragma once

#include <corgi/math/Vec2.h>
#include <corgi/math/Vec3.h>
#include <corgi/math/Vec4.h>

#include <string>
#include <map>
#include <variant>
#include <memory>

namespace corgi
{
	class Texture;
	class ShaderProgram;

	enum class DepthTest : char
	{
		Always,
		Never,
		Less,
		Equal,
		LEqual,
		Greater,
		NotEqual,
		GEqual
	};

	enum class StencilTest : char
	{
		Always,
		NotEqual
	};

	enum class StencilOp : char
	{
		Replace,
		Keep
	};

	// Material is a class that contains additional information on how a 
	// mesh should be rendered. Also, the renderer will sort the renderer
	// component based on their material to reduce draw calls when the 
	// material's parameters are the same
	class Material
	{
	public:

		enum class PolygonMode : char
		{
			Line,
			Fill
		};

	// Lifecycle

		Material();
		Material(const std::string& name);
		~Material();

	// Functions

		template<class T>
		void set_uniform(const std::string& name, T v)
		{
			uniforms_[name] = v;
		}

		// Look for an uniform called "name" and remove it 
		void remove_uniform(const std::string& name);

		// Renderer Components are sorted by material to reduce 
		// draw calls
		bool operator<(const Material& m)const;

		bool operator==(const Material& m)const;
		

		//I use main_texture as a default name for texture in shader
		void add_texture(Texture* texture, const std::string& name = "main_texture");

		// TODO : Set all variables privates

	// Variables

		// If true, standard light information will be sent to the shader
		bool is_lit = true;		// 1 bytes

		// Objects are put in different draw list depending
		// on if they are transparent or opaque
		bool is_transparent = true;	// 1 bytes

		// Disable or enable depth test entirely
		bool enable_depth_test		= true;		// 1 bytes
		bool enable_stencil_test	= false;	// 1 bytes

		// Disable or enable blending entirely
		bool enable_blend = true;		// 1 byte

		// When false, the depth buffer is only in read-only mode
		bool depth_mask		= true;	// 1 byte

		bool stencil_mask	= false;	// 1 byte

		// When false, the object won't write in the color buffer
		bool render_color	= true;	// 1 byte

		StencilOp stencil_op		= StencilOp::Replace; // 1 byte (can make it 1)

		// When the depth passes. If current fragment z value
		// is "less" than the one stored, then it passes and writes
		DepthTest depth_test		= DepthTest::LEqual;		// 1 bytes

		StencilTest stencil_test	= StencilTest::NotEqual;	// 1 bytes

		// How the renderer should renders the mesh
		PolygonMode mode = { PolygonMode::Fill };				// 1 bytes

		// 12 bytes

		std::map<std::string, Texture*> textures_;		// 12 bytes

		// Shader Program linked to the current material
		ShaderProgram* shader_program = nullptr;			// 4 bytes

		std::map<std::string, std::variant<float, int, Vec2, Vec3, Vec4>>	uniforms_;

		// Used by the renderer to know what needs to be drawn before what
		short render_queue = 0;	// 2 bytes

		std::string name_;
	};
}
