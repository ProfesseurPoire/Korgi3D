#pragma once

#include <memory>
#include <string>

namespace corgi
{
	class Shader
	{
	public:

		enum class Type
		{
			Vertex,
			Fragment
		};

		unsigned int		id()const		{return _id;}
		const std::string&	source()const	{return _source;}
		Type				type()const		{return _type;}

	private:
		friend class Renderer;
		friend class ShaderProgram;



		Shader();
		Shader(const std::string& name, unsigned int id, const std::string& source, Type type)
			: _name(name), _id(id), _source(source), _type(type) {}

		~Shader() = default;

		unsigned int _id;
		const std::string _source;
		const std::string _name;
		Type _type;
	};

	// A shader Program is mainly a combination of shaders
	class ShaderProgram
	{
		friend class Renderer;

		ShaderProgram() = default;
		ShaderProgram(const std::string& name, 
			std::weak_ptr<Shader> vertex_shader,
			std::weak_ptr<Shader> fragment_shader,
			unsigned int id)
			:_name(name),_vertex_shader(vertex_shader),
		_fragment_shader(fragment_shader), _id(id){}

		~ShaderProgram() = default;

		std::string _name;
		unsigned int _id;

		// That way the Shader is only deleted once no more
		// program use it
		std::shared_ptr<Shader> _vertex_shader;
		std::shared_ptr<Shader> _fragment_shader;

	public:

		bool operator<(const ShaderProgram& p)const
		{
			return _id < p._id;
		}

		const std::string& name()const
		{
			return _name;
		}

		unsigned int id()const
		{
				return _id;
		}
	};
}