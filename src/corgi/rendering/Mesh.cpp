#include "Mesh.h"

#include <corgi/rendering/RenderCommand.h>
#include <corgi/rendering/renderer.h>

#include <corgi/utils/Strings.h>

#include <cassert>
#include <algorithm>
#include <glad/glad.h>

namespace corgi
{
	Vertex::Vertex(float* data)
		:_data(data){}

	Vertex::Vertex(float* data, const std::vector<VertexAttribute>& attributes)
		:_data(data), _attributes(attributes){}

	// TODO : Maybe using a map for the vertex part makes sense
	// Update if I notice some slowdown on mesh construction
	Vertex& Vertex::set_attribute(const std::string& attribute_name, float x)
	{
		assert(has_attribute(attribute_name));
		assert(attribute(attribute_name)->size == 1);

		_data[attribute(attribute_name)->offset] = x;
		return *this;
	}

	Vertex& Vertex::set_attribute(const std::string& attribute_name, float x, float y)
	{
		assert(has_attribute(attribute_name));
		assert(attribute(attribute_name)->size == 2);

		_data[attribute(attribute_name)->offset + 0] = x;
		_data[attribute(attribute_name)->offset + 1] = y;
		return *this;
	}

	Vertex& Vertex::set_attribute(const std::string& attribute_name, float x, float y, float z)
	{
		assert(has_attribute(attribute_name));
		assert(attribute(attribute_name)->size == 3);

		_data[attribute(attribute_name)->offset + 0] = x;
		_data[attribute(attribute_name)->offset + 1] = y;
		_data[attribute(attribute_name)->offset + 2] = z;
		return *this;
	}
	
	Vertex& Vertex::set_attribute(const std::string& attribute_name, float x, float y, float z, float w)
	{
		assert(has_attribute(attribute_name));
		assert(attribute(attribute_name)->size == 4);

		_data[attribute(attribute_name)->offset + 0] = x;
		_data[attribute(attribute_name)->offset + 1] = y;
		_data[attribute(attribute_name)->offset + 2] = z;
		_data[attribute(attribute_name)->offset + 3] = w;
		return *this;
	}

	const VertexAttribute* Vertex::attribute(const std::string& name)const
	{
		for(auto& attribute : _attributes)
		{
			if (attribute.name() == name)
				return &attribute;
		}
		return nullptr;
	}

	Vertex& Vertex::position(float x, float y, float z)
	{
		assert(has_attribute("position"));
		return set_attribute("position", x, y, z);
	}

	Vertex& Vertex::position(const Vec3& pos)
	{
		return position(pos.x, pos.y, pos.z);
	}

	Vertex& Vertex::uv(float u, float v)
	{
		assert(has_attribute("texture_coordinates"));
		return set_attribute("texture_coordinates", u, v);
	}

	Vertex& Vertex::uv(const Vec2& uv)
	{
		return this->uv(uv.x, uv.y);
	}

	Vertex& Vertex::normal(const Vec3& n)
	{
		return normal(n.x, n.y, n.z);
	}

	Vertex& Vertex::normal(float x, float y, float z)
	{
		assert(has_attribute("normals"));
		return set_attribute("normals", x, y, z);
	}

	int Mesh::vertex_count()const
	{
		return static_cast<int>(vertices_.size()) / vertex_size();
	}

	int Mesh::vertex_size()const
	{
		int count = 0;
		for (auto& attribute : _attributes)
			count += attribute.size;
		return count;
	}

	std::shared_ptr<Mesh> Mesh::new_standard_mesh()
	{
		return std::shared_ptr<Mesh>(new Mesh({
			VertexAttribute("position", 0, 3),
			VertexAttribute("texture_coordinates", 3, 2),
			VertexAttribute("normals", 5, 3) }));
	}

	std::shared_ptr<Mesh> Mesh::new_standard_sphere(const float radius, const int discretisation)
	{
		// We need at least more than 1 slice
		assert(discretisation > 1);

		auto s = std::shared_ptr<Mesh>(new Mesh({
			VertexAttribute("position", 0, 3),
			VertexAttribute("normals", 3, 3) }));

		// We build the vertices first

		float step_height = (radius * 2.0f) / (discretisation + 1);
		float height = -radius;

		for (int i = 0; i < discretisation; ++i)
		{
			// We try to find the radius of the current sphere slice, which
			// is a circle.
			float slice_angle = math::asinf(math::absf(height) / (radius));
			float local_radius = math::cosf(slice_angle)*radius;

			for (int j = 0; j < discretisation; ++j)
			{
				float angle = 2.0f* math::pi_f / (discretisation);
				angle = angle * j;

				float x = math::cosf(angle)*local_radius;
				float z = math::sinf(angle) * local_radius;

				Vec3 position(x, height, z);
				Vec3 normal(position.normalized());

				s->add_vertex()
					.position(position)
					.normal(normal);
			}
			height += step_height;
		}

		// We build the triangles

		// Skipping the first slice
		for (int i = 1; i < discretisation; ++i)
		{
			for (int j = 0; j < discretisation; j++)
			{
				int C = i * discretisation + j;
				int D = C + 1;
				int A = C - discretisation;
				int B = A + 1;

				// 
				//  C----- D
				//	
				//  A ---  B

				s->add_triangle(A, B, D);
				s->add_triangle(A, D, C);
			}

			// 
			//  C----- D
			//
			//  A ---  B

			int C = (i + 1) * discretisation - 1;
			int A = C - discretisation;
			int B = (A + 1) - discretisation;
			int D = C + 1 - discretisation;

			s->add_triangle(A, B, D);
			s->add_triangle(A, D, C);
		}

		s->update_vertices();
		return s;
	}

	Mesh::Mesh(const std::vector<VertexAttribute>& attributes)
		:	_attributes(attributes),
			_vbo_index(RenderCommand::generate_buffer_object()),
			_ibo_index(RenderCommand::generate_buffer_object()),
			vao_id_(RenderCommand::generate_vao_buffer()),
			_primitive_type(PrimitiveType::Triangles)
		{
		RenderCommand::bind_vertex_array(vao_id_);
		RenderCommand::bind_vertex_buffer_object(_vbo_index);
		RenderCommand::bind_index_buffer_object(_ibo_index);
		RenderCommand::bind_vertex_array(0);
		RenderCommand::bind_vertex_buffer_object(0);
		RenderCommand::bind_index_buffer_object(0);
		

				// TODO : avoid using Game though, it would be better
				// if Renderer was a static class as well and if I could check
				// if it has been initialized or not 
				
				// If there's no renderer, there's no point to actually
				// send the mesh's data to the GPU
	}

	Mesh::Mesh(std::vector<VertexAttribute>&& attributes) : 
		_attributes(std::move(attributes)),
		vao_id_(RenderCommand::generate_vao_buffer()),
		_vbo_index(RenderCommand::generate_buffer_object()),
		_ibo_index(RenderCommand::generate_buffer_object()),
		_primitive_type(PrimitiveType::Triangles)
	{
		RenderCommand::bind_vertex_array(vao_id_);
		RenderCommand::bind_vertex_buffer_object(_vbo_index);
		RenderCommand::bind_index_buffer_object(_ibo_index);
		RenderCommand::bind_vertex_array(0);
		RenderCommand::bind_vertex_buffer_object(0);
		RenderCommand::bind_index_buffer_object(0);
	}

	Vertex Mesh::add_vertex()
	{
		vertices_.insert(vertices_.end(), vertex_size(),0.0f);
		return Vertex(&vertices_[vertices_.size() - vertex_size()], _attributes);
	}

	std::vector<Vertex> Mesh::add_vertices(int count)
	{
		std::vector<Vertex> vertices;

		vertices_.insert(vertices_.end(), vertex_size() * count, 0.0f);

		for (int i = 0; i < count; ++i)
			vertices.emplace_back(&vertices_[vertices_.size() - (count - i)*vertex_size()], _attributes);
		return vertices;
	}

	void Mesh::update_vertices()
	{
		RenderCommand::bind_vertex_array(vao_id_);

		

		RenderCommand::buffer_vertex_data(_vbo_index, vertices_.data(),
			static_cast<int>(sizeof(float) * vertices_.size()));

		RenderCommand::buffer_index_data(_ibo_index,
			indexes_.data(), static_cast<int>(indexes_.size() * sizeof(unsigned int)));


		if (has_attribute("position"))
		{
			RenderCommand::enable_vertex_attribute(0);
			RenderCommand::vertex_attribute_pointer(0, vertex_size() * sizeof(float),
				0, 3);
		}

		if (has_attribute("texture_coordinates"))
		{
			RenderCommand::enable_vertex_attribute(1);
			RenderCommand::vertex_attribute_pointer(1,
				vertex_size() * sizeof(float),
				3,
				2);
		}

		RenderCommand::bind_vertex_array(0);

		if (has_attribute("position"))
			RenderCommand::disable_vertex_attribute(0);

		if (has_attribute("texture_coordinates"))
			RenderCommand::disable_vertex_attribute(1);
	}

	void Mesh::clear()
	{
		vertices_.clear();
		indexes_.clear();

		// TODO : I don't think I need to delete and remake the VBO, I could probably use glBufferSubData or something no?
		RenderCommand::delete_vertex_buffer_object(_vbo_index);
		RenderCommand::delete_vertex_buffer_object(_ibo_index);
		RenderCommand::delete_vertex_array_object(vao_id_);

		_vbo_index  = RenderCommand::generate_buffer_object();
		_ibo_index  = RenderCommand::generate_buffer_object();
		vao_id_		= RenderCommand::generate_vao_buffer();

		RenderCommand::bind_vertex_array(vao_id_);
		RenderCommand::bind_vertex_buffer_object(_vbo_index);
		RenderCommand::bind_index_buffer_object(_ibo_index);
		RenderCommand::bind_vertex_array(0);
		RenderCommand::bind_vertex_buffer_object(0);
		RenderCommand::bind_index_buffer_object(0);
	}

	void Mesh::update_vertices_really()
	{
		RenderCommand::buffer_vertex_subdata(_vbo_index, vertices_.data(), vertices_.size()*sizeof(float));
	}

	std::vector<float>& Mesh::vertices()
	{
		return vertices_;
	}

	Mesh::~Mesh()
	{
		RenderCommand::delete_vertex_buffer_object(_vbo_index);
		RenderCommand::delete_vertex_buffer_object(_ibo_index);
		RenderCommand::delete_vertex_array_object(vao_id_);
		Renderer::unregister_mesh(this);
	}
	VertexAttribute::VertexAttribute(char* name, int offset, int size)
		:offset(offset), size(size), name_(name)
	{

	}

	VertexAttribute::VertexAttribute(const std::string& name, int offset, int size)
		: offset(offset), size(size)
	{
		name_ = convert_std_string(name);
	}

	VertexAttribute::VertexAttribute(std::string&& name, int offset, int size)
		: name_(convert_std_string(name)), offset(offset), size(size) {}

	const VertexAttribute* Mesh::attribute(const std::string& name)const
	{
		for(auto& attribute : _attributes)
			if (attribute.name() == name)
				return &attribute;
		return nullptr;
	}

	void Mesh::add_triangle(unsigned int index1, unsigned int index2, unsigned int index3)
	{
		// TODO : Keep an eye on this. I'm not too sure about insert performances
		indexes_.insert(indexes_.end(), { index1, index2, index3 });
	}

	const std::vector<float>& Mesh::vertices()const
	{
		return vertices_;
	}

	const std::vector<unsigned int>& Mesh::indexes()const
	{
		return indexes_;
	}

	PrimitiveType Mesh::primitive_type() const
	{
		return _primitive_type;
	}

	void Mesh::primitive_type(PrimitiveType typ)
	{
		_primitive_type = typ;
	}

	bool Mesh::has_attribute(const std::string& name)const
	{
		// TODO : As always, not too sure about the performances of this, 
		// especially since the vector is really small
		return std::find_if(_attributes.begin(), _attributes.end(), [&](const VertexAttribute& v)
		{
			return v.name() == name;
		}) != _attributes.end();
	}

	bool Vertex::has_attribute(const std::string& name) const
	{
		return std::find_if(_attributes.begin(), _attributes.end(), [&](const VertexAttribute& v)
		{
			return v.name() == name;
		}) != _attributes.end();
	}

	void Mesh::set_vertices(float* vertices, int count)
	{
		vertices_ = std::vector<float>(vertices, vertices + count);
	}

	std::vector<unsigned int>& Mesh::indexes()
	{
		return indexes_;
	}
}
