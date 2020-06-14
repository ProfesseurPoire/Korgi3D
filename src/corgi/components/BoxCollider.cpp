#include "BoxCollider.h"

#include <corgi/rendering/renderer.h>

#include <assert.h>



namespace corgi
{
	BoxCollider::BoxCollider() :
		ColliderComponent({ {VertexAttribute("position", 0, 3)} })
	{
	}

	BoxCollider::BoxCollider(int layer) : 
		ColliderComponent({ {VertexAttribute("position", 0, 3)} })
	{
		// TODO :  Don't use an int for the  layer thing, though I'm
		// not exactly sure how to make a type that locks values
		// between 0 and 64

		// Can't have a layer > 64, since I use a 64 bit variable
		// as my collision layer mask
		assert(layer < 64 && layer >= 0);
		_current_layer = layer;
	}

	BoxCollider::BoxCollider(int layer, float width, float height, float depth):
		ColliderComponent(nullptr),
		width_(width),
		height_(height),
		depth_(depth)
	{
		std::vector<VertexAttribute> attributes = { {VertexAttribute("position", 0, 3)} };
		_mesh = std::make_shared<Mesh>(attributes);

		// Can't have a layer > 64, since I use a 64 bit variable
		// as my collision layer mask
		assert(layer < 64 && layer >= 0);
		_current_layer = layer;

		build_box();
	}

	void BoxCollider::dimensions(float width, float height, float depth)
	{
		width_ = width;
		height_ = height;
		depth_ = depth;
		build_box();
	}

	float BoxCollider::width()const noexcept
	{
		return width_;
	}

	float BoxCollider::height()const noexcept
	{
		return height_;
	}

	float BoxCollider::depth()const noexcept
	{
		return depth_;
	}

	void BoxCollider::width(float v)
	{
		width_ = v;
	}

	void BoxCollider::height(float v)
	{
		height_ = v;
	}

	void BoxCollider::depth(float v)
	{
		depth_ = v;
	}

	void BoxCollider::build_box()
	{
		const float mdepth	= depth()  / 2.0f;
		const float mwidth	= width()  / 2.0f;
		const float mheight	= height() / 2.0f;

		auto vertices = _mesh->add_vertices(18);

		vertices[0].position(-mwidth, -mheight, +mdepth);
		vertices[1].position(+mwidth, -mheight, +mdepth);
		vertices[2].position(-mwidth, +mheight, +mdepth);
		vertices[3].position(+mwidth, +mheight, +mdepth);

		vertices[4].position(-mwidth, -mheight, -mdepth);
		vertices[5].position(+mwidth, -mheight, -mdepth);
		vertices[6].position(-mwidth, +mheight, -mdepth);
		vertices[7].position(+mwidth, +mheight, -mdepth);
		
		//Front

		_mesh->add_triangle(0, 1, 3);
		_mesh->add_triangle(0, 3, 2);

		normals.push_back(Vec3::forward);
		normals.push_back(Vec3::forward);

		//// Back

		_mesh->add_triangle(4, 5, 7);
		_mesh->add_triangle(4, 7, 6);

		normals.push_back(Vec3::backward);
		normals.push_back(Vec3::backward);

		// Left

		_mesh->add_triangle(4, 0, 2);
		_mesh->add_triangle(4, 2, 6);

		normals.push_back(Vec3::left);
		normals.push_back(Vec3::left);

		// Right 

		_mesh->add_triangle(1, 5, 7);
		_mesh->add_triangle(1, 7, 3);

		normals.push_back(Vec3::right);
		normals.push_back(Vec3::right);

		// Top 

		_mesh->add_triangle(2, 3, 6);
		_mesh->add_triangle(3, 7, 6);

		normals.push_back(Vec3::up);
		normals.push_back(Vec3::up);

		// Down

		_mesh->add_triangle(0,1,4);
		_mesh->add_triangle(1,5,4);

		normals.push_back(Vec3::down);
		normals.push_back(Vec3::down);

		_mesh->update_vertices();
	}

	bool BoxCollider::is_enabled()const noexcept
	{
		return is_enabled_;
	}

	void BoxCollider::enable()
	{
		is_enabled_=true;
	}

	void BoxCollider::disable()
	{
		is_enabled_=false;
	}
}