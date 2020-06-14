#include "Camera.h"

#include <corgi/components/Transform.h>
#include <corgi/rendering/FrameBuffer.h>

using namespace std;

namespace corgi
{
// Lifecycle

Camera::Camera(const Camera& copied) :
	is_dirty_(copied.is_dirty_),
	default_viewport_(copied.default_viewport_),
	type_(copied.type_),
	culling_layers_(copied.culling_layers_),
	projection_matrix_(copied.projection_matrix_),
	znear_(copied.znear_),
	zfar_(copied.zfar_),
	ratio_(copied.ratio_),
	orthographic_height_(copied.orthographic_height_),
	// making a framebuffer copy here
	framebuffer_(std::make_unique<FrameBuffer>(*copied.framebuffer_)),
	clear_color_(copied.clear_color_),
	viewport_(copied.viewport_),
	order_(copied.order_)
{

}

Camera& Camera::operator=(const Camera& copied)
{
	is_dirty_ = copied.is_dirty_;
	default_viewport_ = copied.default_viewport_;
	type_ = copied.type_;
	culling_layers_ = copied.culling_layers_;
	projection_matrix_ = copied.projection_matrix_;
	znear_ = copied.znear_;
	zfar_ = copied.zfar_;
	ratio_ = copied.ratio_;
	orthographic_height_ = copied.orthographic_height_;
	// making a framebuffer copy here
	framebuffer_ = std::make_unique<FrameBuffer>(*copied.framebuffer_);
	clear_color_ = copied.clear_color_;
	viewport_ = copied.viewport_;
	order_ = copied.order_;

	return *this;
}

	void Layers::enable_all()
	{
		layers_ = ~(int_64(0));
	}

	void Layers::disable_all()
	{
		layers_ = int_64(0);
	}

	int_64 Layers::layers() const
	{
		return layers_;
	}

	void Layers::enable_layer(const int layer_id)
	{
		layers_ |= (int_64(1) << layer_id);
	}

	void Layers::enable_one_layer(const int layer_id)
	{
		layers_ = (int_64(1) << layer_id);
	}

	void Layers::disable_layer(const int layer_id)
	{
		layers_ &= (~(int_64(1) << layer_id));
	}

	//width and height are window's dimensions
	Vec2 Camera::screen_to_world_point(Vec2 point, Matrix world_matrix, int width, int height)
	{
		point.y = float(height) - point.y;

		const float new_x = (float(point.x) / float(width)) * 2.0f -1.0f;
		const float new_y = (float(point.y) / float(height)) * 2.0f -1.0f;

		const auto vec3 = projection_matrix_.inverse() * Vec4(
			new_x, new_y, 0.0f, 1.0f);
		auto v = world_matrix * vec3;
		return Vec2(v.x, v.y);
	}

	//Vec2 Camera::screen_to_world_point(int x, int y, Transform& transform)
	//{
	//	return screen_to_world_point(Vec2(static_cast<float>(x),static_cast<float>(y)),transform);
	//}

	//Vec2 Camera::screen_to_world(const Transform& transform,
	//                             int              screeWidth, int screeHeight,
	//                             int              x,
	//                             int              y) const
	//{
	//	// The "real" way to do this would be to use the inverse of the projection Matrix4x4
	//	// and multiply it to the mouse coordinate on the -1;1 set

	//	// First thing is to invert the y

	//	y = screeHeight - y;

	//	// This is a bit hacky but it shall work nicely for everything using an ortho projection
	//	// on a 2D plane; I should make a test project to check if the Matrix4x4's inverse is correct 
	//	// though
	//	float new_x = (float)x / (float)screeWidth;
	//	float new_y = (float)y / (float)screeHeight;

	//	const float w = ratio_ * orthographic_height_ * 2.0f;
	//	const float h = orthographic_height_ * 2.0f;

	//	new_x = -transform.position().x + (-ratio_ * orthographic_height_) +
	//		new_x * w;
	//	new_y = -transform.position().y + (-orthographic_height_) + new_y * h;

	//	return Vec2(new_x, new_y);
	//}


	void Camera::ortho(const float height, const float ratio, const float znear, const float zfar)
	{
		type_ = Type::Ortho;

		znear_               = znear;
		zfar_                = zfar;
		orthographic_height_ = height;
		ratio_               = ratio;

		projection_matrix_ = Matrix::ortho(-ratio * height, ratio * height,
		                                   -height, height, znear, zfar);
	}

	void Camera::frustum(float l, float r, float b, float t, float n, float f)
	{
		type_  = Type::Perspective;
		znear_ = n;
		zfar_  = f;

		projection_matrix_ = Matrix::frustum(l, r, b, t, n, f);
		//_isOrthographic = false;
	}

	Color& Camera::clear_color()
	{
		return clear_color_;
	}

	void Camera::clear_color(float r, float g, float b, float a)
	{
		clear_color_.r = r;
		clear_color_.g = g;
		clear_color_.b = b;
		clear_color_.a = a;
	}

	void Camera::znear(float value)
	{
		znear_    = value;
		is_dirty_ = true;
	}

	void Camera::zfar(float value)
	{
		zfar_     = value;
		is_dirty_ = true;
	}

	void Camera::ratio(float value)
	{
		ratio_    = value;
		is_dirty_ = true;
	}

	void Camera::orthographic_height(float value)
	{
		orthographic_height_ = value;
		is_dirty_            = true;
	}

	float Camera::orthographic_height() const
	{
		return orthographic_height_;
	}

	float Camera::ratio() const
	{
		return ratio_;
	}

	float Camera::znear() const
	{
		return znear_;
	}

	float Camera::zfar() const
	{
		return zfar_;
	}

	bool Camera::is_orthographic() const
	{
		return type_ == Type::Ortho;
	}

	bool Camera::is_perspective() const
	{
		return type_ == Type::Perspective;
	}

	bool Camera::is_custom() const
	{
		return type_ == Type::Custom;
	}

	void Camera::projection_matrix(const Matrix& m)
	{
		type_              = Type::Custom;
		projection_matrix_ = m;
	}

	void Camera::viewport(int x, int y, int width, int height)
	{
		viewport_.x       = x;
		viewport_.y       = y;
		viewport_.width   = width;
		viewport_.height  = height;
		default_viewport_ = false;
	}

	void Camera::viewport(int x, int y, const Vec2i& dimensions)
	{
		viewport(x, y, dimensions.x, dimensions.y);
	}

	void Camera::viewport(const Vec2i& position, const Vec2i& dimensions)
	{
		viewport(position.x, position.y, dimensions.x, dimensions.y);
	}

	void Camera::use_default_viewport()
	{
		default_viewport_ = true;
	}

	Viewport& Camera::viewport()
	{
		return viewport_;
	}

	Matrix& Camera::projection_matrix()
	{
		return projection_matrix_;
	}

	const Matrix& Camera::projection_matrix() const
	{
		return projection_matrix_;
	}

	Layers& Camera::culling_layers()
	{
		return culling_layers_;
	}

	void Camera::move(Camera&& c) noexcept
	{
		is_dirty_            = c.is_dirty_;
		default_viewport_    = c.default_viewport_;
		type_                = c.type_;
		culling_layers_      = c.culling_layers_;
		projection_matrix_   = c.projection_matrix_;
		znear_               = c.znear_;
		zfar_                = c.zfar_;
		ratio_               = c.ratio_;
		orthographic_height_ = c.orthographic_height_;
		framebuffer_         = std::move(c.framebuffer_);
		clear_color_         = c.clear_color_;
		viewport_            = c.viewport_;
		order_               = c.order_;
	}

	void Camera::copy(const Camera& c)
	{
		
	}

	void Camera::framebuffer(int width, int height)
	{
		framebuffer_ = std::make_unique<FrameBuffer>(width, height);
	}

	FrameBuffer* Camera::framebuffer()
	{
		return framebuffer_.get();
	}

	bool Camera::operator<(const Camera& camera) const
	{
		return order_ < camera.order_;
	}

	int Camera::order() const
	{
		return order_;
	}

	void Camera::order(short v)
	{
		order_ = v;
	}

	Camera::Camera(Camera&& c) noexcept
	{
		move(std::move(c));
	}

	Camera& Camera::operator=(Camera&& c)noexcept
	{
		move(std::move(c));
		return *this;
	}

	


	const FrameBuffer* Camera::framebuffer() const
	{
		return framebuffer_.get();
	}
}
