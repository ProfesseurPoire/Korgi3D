#pragma once

#include <corgi/math/Matrix.h>
#include <corgi/math/Vec2.h>

#include <corgi/ecs/Component.h>

#include <corgi/utils/types.h>

#include <corgi/rendering/Color.h>

#include <memory>

namespace corgi
{

class Transform;
class FrameBuffer;

class Layers
{
public:

	void enable_all();
	void disable_all();
	void enable_one_layer(int layer_id);
	void enable_layer(int layer_id);
	void disable_layer(int layer_id);

	[[nodiscard]]int_64 layers()const;

private:
	
	int_64 layers_ = ~int_64(0);	// 1 layer = 1 byte
};

struct Viewport
{
	int x{ 0 }, y{ 0 }, width{ 0 }, height{ 0 };
};

/*
 *  @brief  The camera component represents a camera viewpoint with its projection and viewport
 *			settings. Anything in "front" of the camera and inside its
 *			projection transformation will be rendered to a surface, primarily inside the window,
 *			but it can also be rendered inside a texture. The viewport is a rectangle that
 *			defines where the camera actually renders inside the surface.
 *
 *          You probably will only use 1, but some effect, like shadow mapping,
 *          can be achieved by using multiple cameras, that then render into a texture that
 *          can later on be used by a shader to do stuff.
 *
 *          You could also have multiple camera drawing into different viewport. A viewport
 *          being a rectangle on the screen/window.
 * 
 *          Or maybe camera on different windows?
 *
 *          In OpenGL, there is no notion of "camera". There's only "one", and it's always
 *          centered in 0.0.0, looking to the Z negative axis.
 *
 *          So to actually "move" the camera, you need to move the whole world. A solution
 *          that is often used to solve this problem is to apply the inverse of the camera's
 *          transformation.
 *
 *          So if your camera object is 6 units to the x axis, you actually
 *          move all your objects from -6 units
 *
 *          The projection Matrix is here to create a perspective effect
 *          (frustum and perspective matrix), or to expand the size
 *          of the NDC cube (ortho matrix) Though you could technically
 *          throw any kind of matrix as the projection matrix
 *
 *          When rendering, OpenGL will take every vertex, and multiply them
 *           with the MVP matrix, for ModelViewProjection matrix, which is the multiplication
 *          of the Model matrix, the View matrix and the projection matrix to transform
 *          the vertices to NDC space. Inside that space, any vertex that is outside
 *          the -1/+1 cube is clipped by OpenGL and won't be rendered.
 *
 *          Also the camera holds the color used to clear the screen, and the viewport
 *          By default, the viewport matches the window's dimensions. If you manually
 *          set the viewport, this behavior is deactivated, so, use with caution/
 *          know what you're doing
 */
class Camera : public Component
{
	friend class Renderer;

public:

// Lifecycle 

	Camera()=default;

	// I need to define the move and copy constructor because camera
	// has unique_ptr to a FrameBuffer... Maybe make it a value thing?
	
	Camera(Camera&& moved) noexcept;
	Camera& operator=(Camera&& moved) noexcept;

	Camera(const Camera& copied);
	Camera& operator=(const Camera& copied);

	~Camera() = default;

// Functions

	/*!
	 *  @brief  Sets an orthographic projection matrix.
	 *
	 *  @param  height  height of the projection
	 *  @param  ratio   ratio * height = width
	 *  @param  znear   near clip plane
	 *  @param  zfar    far clip plane
	 */
	void ortho(float height, float ratio, float znear, float zfar);

	/*!
	 *  @brief  Set the projection matrix as perspective
	 */
	void frustum(float l, float r, float b, float y, float n, float f);

	/*!
	 *  @brief  Sets the znear clip plane of the projection. 
	 *  
	 *			Anything drawn behind this value will be clipped out. This
	 *          will also raise the dirty flag so the projection matrix
	 *          is updated by the renderer system
	 */
	void znear(float value);

	/*!
		*  @brief  Sets the zfar clip plane of the projection. 
		*  
		*			Anything drawn after this value will be clipped out. This
		*          will also raise the dirty flag so the projection matrix
		*          is updated by the renderer system
		*/
	void zfar(float value);

	/*!
		*  @brief  Sets the ratio value. The ratio value is used to find
		*          the "width" of the orthographic projection by multiplying
		*          it to the @ref orthographicSize
		*/
	void ratio(float value);

	/*
		*  @brief  Sets the orthographicHeight value. The orthographicSize
		*          is the height of the orthographic projection.This will
		*          also raise the _dirty flag to true so the component's
		*          projection matrix is updated by the renderer system
		*/
	void orthographic_height(float value);

	/*
		*  @brief  Sets a custom projection matrix
		*
		*  If you set the projection using this method, the component won't
		*  use _orthographicSize and ratio anymore
		*/
	void projection_matrix(const Matrix& m);

	/*
		*  @brief  Sets the color used to clear the color attachment 
		*          of the framebuffer
		*/
	void clear_color(float r, float g, float b, float a);

	/*
		*  @brief  Returns the znear clip plane of the projection. Works
		*          for both the perspective and orthographic projection
		*/
	[[nodiscard]] float znear()const;

	/*
		*  @brief  Returns the zfar clip plane of the projection. Works
		*          for both the perspective and orthographic projection
		*/
	[[nodiscard]] float zfar()const;

	/*
		*  @brief  Returns the ratio between the height and the width
		*          of the projection
		*/
	[[nodiscard]] float ratio()const;

	/*
		*  @brief  Returns the height of the orthographic projection
		*/
	[[nodiscard]] float orthographic_height()const;

	/*
		*  @brief Returns true if the camera is in orthographic mode
		*/
	[[nodiscard]] bool is_orthographic()const;

	/*
		* @brief Returns true if the camera is in perspective mode
		*/
	[[nodiscard]] bool is_perspective()const;

	/*
		*  @brief  Returns true if a custom projection matrix was fed
		*          to the camera
		*/
	[[nodiscard]] bool is_custom()const;

	/*
		*  @brief  Returns the color used to clear the color attachment
		*          of the framebuffer
		*/
	[[nodiscard]] Color& clear_color();

	/*
		*  @brief  Returns the viewport from the camera
		*          The viewport is the rectangle where the camera renders
		*          on the screen/framebuffer
		*/
	[[nodiscard]] Viewport& viewport();

	/*
		*  @brief  Sets the camera's viewport. 
		*          The viewport is the rectangle where the camera renders on
		*          the screen/framebuffer
		*/
	void viewport(int x, int y, int width, int height);
	void viewport(int x, int y, const Vec2i& dimensions);
	void viewport(const Vec2i& position, const Vec2i& dimensions);

	/*
		*  @brief  Returns a reference to the projection matrix used by the
		*          the camera
		*/
	[[nodiscard]] Matrix& projection_matrix();
	[[nodiscard]] const Matrix& projection_matrix()const;

	void use_default_viewport();

	[[nodiscard]] Vec2 screen_to_world_point(Vec2 point, Matrix world_matrix, int width, int height);


	[[nodiscard]]bool operator<(const Camera& camera)const;

	[[nodiscard]] int order()const;
	void order(short v);

	[[nodiscard]] FrameBuffer* framebuffer();
	[[nodiscard]] const FrameBuffer* framebuffer()const;
	void framebuffer(int width, int height);

	[[nodiscard]] Layers& culling_layers();

protected:

	enum class Type : char{ Ortho, Perspective, Custom } ;

// functions

	void move(Camera&& c)noexcept;
	void copy(const Camera& c);

// Variables

	// When true the projection matrix need to be computed
	bool	is_dirty_			= false;			// 1
	bool	default_viewport_	= true;				// 1
	Type	type_				= Type::Custom;		// 1 

	// Every entity is attached to a layer
	// This will tell the camera to ignore entities based
	// on their layer
	Layers culling_layers_;	// on 8 bytes

	Matrix		projection_matrix_;		// 64

	// Might be a bit big and often useless but it's also
	// not like I'm going to have thousands of camera
	float		znear_					= 0.0f;	// 4 
	float		zfar_					= 0.0f;	// 4
	float		ratio_					= 0.0f;	// 4 
	float		orthographic_height_	= 0.0f;	// 4

	// if null, the camera renders on the default framebuffer
	std::unique_ptr<FrameBuffer> framebuffer_;	// 8 bytes

	Color		clear_color_= { 1.0f,1.0f,1.0f,1.0f };		// 16
	Viewport    viewport_	= { 0, 0, 0, 0 };				// 16
	// Order in which the camera are sorted for rendering
	short		order_		= 0;		// 2 
};

}