#pragma once

#include <corgi/rendering/Color.h>
#include <corgi/rendering/Sprite.h>

#include <corgi/math/Matrix.h>
#include <corgi/components/MeshRenderer.h>

#include <corgi/rendering/Mesh.h>
#include <corgi/rendering/Material.h>
#include <corgi/rendering/Shader.h>
#include <corgi/rendering/PostProcessing.h>

#include <corgi/math/Ray.h>
#include <corgi/math/Vec3.h>
#include <corgi/math/Vec4.h>

#include <corgi/utils/TimeHelper.h>

#include <vector>
#include <memory>

#include <corgi/containers/Vector.h>

using namespace corgi;

namespace corgi
{
	class Transform;
	class SpriteRenderer;
	class Camera;
	class Text;
	struct Panel;
	class Window;
	class Image;
	class FrameBuffer;
	class Texture;
	struct Viewport;
	class Tilemap;
	class ParticleEmitter;
	class BoxCollider;
	class ColliderComponent;
	class MeshRenderer;
	class Mesh;
	struct Canvas;

	class Scene;

	

	// So the Renderer is basically the OpenGLContext here. It can creates Buffer objects and stuff,
	// but theses buffer objects only makes sense for the current OpenGL Context.
	// So the architecture will stay the way it is now. Window owns a "Renderer" Object
	// which owns the state of every opengl thing.
	class Renderer
	{
	public:

		Counter counter_;

		static void MeshDeleter(Mesh*m);
		static void TextureDeleter(Texture*);
		static void frame_buffer_deleter(FrameBuffer*);
		static void shader_deleter(Shader*);
		static void program_deleter(ShaderProgram*);

		void register_mesh(Mesh* mesh);

		// TODO : Slowly put everything renderer related as static functions instead of having it as a game object
		static void unregister_mesh(Mesh* mesh);

		// Loaded material is register inside the material maps
		Material load_material(const std::string& file);

		// The default lit material will contains data for phong lighting

		// The idea with the window_draw_list thing was to be able to quickly draw 
		// shapes on the window directly... Not really sure how to actually do this now 
		// But it would be handy if I wanted to do a immediate mode GUI thingie
		// I could try to just make vbo on the fly for each shape and use the regular
		// drawing thing -> would just need to change the projection matrix before
		//
		// So for now this kinda work, but obviously it needs to be expanded, and it's
		// very not performant at all, but if this is just to display a few boxes
		// for GUI stuff I think it should be all right. Alternatively I could also 
		// do it through a "true" mesh etc but well, I'll see
		//
		class DrawList
		{
			friend class Renderer;

			class Color
			{
				float r = 1.0f;
				float g = 1.0f;
				float b = 1.0f;
				float a = 1.0f;
			};

			struct Rectangle
			{
				float x;
				float y;
				float width;
				float height;
				float r, g, b, a;
				Texture* texture = nullptr;
				bool _is_depth	=true;
			};

			struct RectangleMaterial
			{
				float x;
				float y;
				float width; 
				float height;
				Material* material;
			};

			struct Line
			{
				float x;
				float y;
				float z;

				float x2;
				float y2;
				float z2;
				
				float r, g, b, a;
			};

			struct Circle
			{
				float x;
				float y;
				float z;

				float radius;
				float discretisation;
				
				Color color;
			};

		public:

			DrawList(bool is_window_space):is_window_space(is_window_space){}

			bool is_window_space = false;

			void clear();
			void add_rectangle(float x, float y, float width, float height);
			void add_rectangle(float x, float y, float width, float height, float r, float g, float b, float a);

			void add_line(float x, float y, float x2, float y2);
			void add_line(float x, float y, float z, float x2, float y2, float z2);

			void add_line(float x, float y, float x2, float y2, float r, float g, float b, float a);
			void add_line(float x, float y, float z, float x2, float y2, float z2, float r, float g, float b, float a);

			void add_line(Ray ray);
			void add_line(Ray ray, float r, float g, float b, float a);

			void add_textured_rectangle(float x, float y, float width, float height, Texture* t);
			void add_rectangle(Material& m, float x, float y, float width, float height);

//			void add_circle(float x, float y, float z, float radius, );

		private:

			std::vector<RectangleMaterial> _rectangles_material;
			std::vector<Rectangle>	_rectangles;
			std::vector<Line>		_lines;
		};

	// Constructors

		// The renderer is tightly related to the window anyways since
		// the window actually creates and keep the opengl context
		// Anything 
		Renderer(Window& window);

	// Functions

		void clear();
		void initialize();
		void draw();


		DrawList& window_draw_list();
		DrawList& world_draw_list();

		void register_mesh_creation(const Mesh* mesh);

		[[nodiscard]] PostProcessing* post_processing();

	// Shader Stuff

		// Returns a shader with the given name
		std::weak_ptr<Shader> get_shader(const std::string& name)const;
		std::weak_ptr<ShaderProgram> get_shader_program(const std::string& name)const;

		// Tries to build a new shader using the given source code
		// Returns an empty weak_ptr if the generation failed
		std::weak_ptr<Shader> generate_shader(
			const std::string& name,
			Shader::Type type, 
			const std::string& source_code);

		// Tries to generate a new program using the given vertex and fragment shader
		// Returns an empty weak_ptr if the generation failed
		std::weak_ptr<ShaderProgram>generate_program(
			const std::string& name,
			std::weak_ptr<Shader> vertex_shader, 
			std::weak_ptr<Shader> fragment_shader);

		std::weak_ptr<ShaderProgram> generate_shader_program(const std::string& shader_program_name,
			const std::string& vertex_shader_name,
			const std::string& fragment_shader_name);

	// FrameBuffer stuff

		void resize_frame_buffer(FrameBuffer* framebuffer, int width, int height);

		static void begin_default_frame_buffer();
		static void begin_frame_buffer(const FrameBuffer* frameBuffer);
		static void end_frame_buffer();

		void set_uniform(const std::string& name, float v);
		void set_uniform(const std::string& name, int v);
		void set_uniform(const std::string& name, Vec3 v);
		void set_uniform(const std::string& name, Vec4 v);
		void set_uniform(const std::string& name, Matrix m);

		/*
		 * @brief	Actually send the model matrix with the mesh's vertex to the GPU
		 */
		void draw(const Mesh& mesh, const Matrix& matrix);
		
		/*
		 * @brief	Defines a lot of opengl stuff tied to the current material
		 *			Must be called before a a draw function
		 */
		void begin_material(const Material& material);


		/*
		 * @brief	Don't forget to call that after defining a material
		 *			TODO : Maybe begin/end material could be an object so begin
		 *			is the constructor and end is the destructor with the Material
		 *			given as a parameter to the constructor?
		 */
		void end_material(const Material& material);

	// Variables

		Window* window			= nullptr;

		// TODO : probably implement that on the game side?
		bool _show_colliders	= true;
		void check_errors();

	private:

		Matrix _view_matrix;
		Matrix _projection_matrix;

		// Use this when you need to draw temporary stuff (or GUI stuff)
		// Objects will be drawn relative to the screen coordinates/size
		DrawList _window_draw_list	= DrawList(true);

		// Use this when you need to draw temporary stuff in the world
		DrawList _world_draw_list	= DrawList(false);

	// Functions

		static bool depth_sorting(const Component* a, const Component* b, Entity& ea, Entity& eb);

		void set_clear_color(Color c);
		void set_viewport(const Viewport& v);
		void set_viewport(int x, int y, int width, int height);

		/*
		 * @brief	Draw a bunch of RendererComponent objects that share the same material
		 */
		void draw(const std::vector<std::pair<const RendererComponent*,Entity*>>& renderers,const Material& material);

		// TODO :	I'm not sure I should keep that like that 
		void draw(ColliderComponent* collider, Entity& entity);
		void draw(DrawList& drawlist);
		void draw(DrawList::Rectangle& rectangle);
		void draw(const Canvas& canvas);

	// Member Variables

		unsigned int model_matrix_id;

		const Material* current_material = nullptr;
		Camera* _current_camera			= nullptr;

		std::unique_ptr<PostProcessing> post_processing_;

		std::vector<std::shared_ptr<ShaderProgram>>		_shader_programs;
		std::vector<std::shared_ptr<Shader>>			_shaders;

		std::vector<RendererComponent*>		components;

		// So this actually link the meshes with their ID 
		// The id here is that meshes are only built on the GPU once they get
		// inside the renderer. I want to keeps things correctly separated
		// So when you create a new mesh
		std::map<const Mesh*, std::tuple<unsigned int, unsigned int>> meshes_;

		// TODO : Opaque and transparent are kinda the same thing, maybe merge them		   ?
		std::vector<const Material*> opaques_materials_;
		std::vector<const Material*> transparent_materials_;

		Vector<Vector<std::pair<const RendererComponent*, Entity*>>> opaques_;
		Vector<Vector<std::pair<const RendererComponent*, Entity*>>> transparents_;
	};
}