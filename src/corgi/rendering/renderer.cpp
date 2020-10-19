#include "renderer.h"


#include <corgi/ui/Canvas.h>

#include <corgi/math/Matrix.h>
#include <corgi/math/Vec2.h>
#include <corgi/math/Vec4.h>
#include <corgi/math/Vec3.h>

#include <corgi/ecs/Scene.h>
#include <corgi/main/Window.h>

#include <corgi/components/Transform.h>
#include <corgi/components/Tilemap.h>
#include <corgi/components/Camera.h>
#include <corgi/components/BoxCollider.h>

#include <corgi/components/lights/PointLight.h>

#include <corgi/resources/image.h>

#include <corgi/rendering/texture.h>
#include <corgi/rendering/FrameBuffer.h>
#include <corgi/rendering/Mesh.h>

#include <glad/glad.h>

#include <corgi/logger/log.h>

#include <corgi/utils/ResourcesLoader.h>

#include <corgi/filesystem/FileSystem.h>

#include <corgi/rapidjson/rapidjson.h>
#include <corgi/rapidjson/document.h>
#include <corgi/rapidjson/filereadstream.h>

#include <corgi/rendering/RenderCommand.h>

#include <corgi/ecs/Entity.h>
#include <corgi/ecs/Component.h>

#include <corgi/main/Game.h>

#include <corgi/systems/RenderingSystem.h>

#include <algorithm>
#include <assert.h>
#include <fstream>

namespace corgi 
{
	static std::map<Mesh*, std::tuple<GLuint, GLuint>> meshes_;

	void Renderer::register_mesh(Mesh* mesh)
	{
		meshes_[mesh] =
		{
			RenderCommand::generate_buffer_object(),
			RenderCommand::generate_buffer_object()
		};
	}

	void Renderer::unregister_mesh(Mesh* mesh)
	{
		RenderCommand::delete_vertex_buffer_object(std::get<0>(Game::renderer().meshes_[mesh]));
		RenderCommand::delete_vertex_buffer_object(std::get<1>(Game::renderer().meshes_[mesh]));

		Game::renderer().meshes_.erase(mesh);
	}

	// Made this a macro so the when I log the error, I actually knows who called what
	#define check_gl_error()\
		{GLenum result;										\
															\
		while ((result = glGetError()) != GL_NO_ERROR)		\
		{	\
			switch (result)	\
			{	\
			case GL_INVALID_ENUM:	\
				log_error("Invalid Enum");	\
				break;	\
				\
			case GL_INVALID_VALUE:	\
				log_error("Invalid Value");	\
				break;	\
				\
			case GL_INVALID_OPERATION:	\
				log_error("Invalid Operation");	\
				break;	\
				\
			case GL_INVALID_FRAMEBUFFER_OPERATION:\
				log_error("Invalid Framebuffer Operation");\
				break;\
				\
			case GL_OUT_OF_MEMORY:\
				log_error("Out of Memory");\
				break;\
			default:\
				break;\
			}\
		}}\

	std::string read_file_to_string(const std::string& path)
	{
		std::ifstream t(path);
		std::string str;

		t.seekg(0, std::ios::end);
		str.reserve(std::string::size_type(t.tellg()));
		t.seekg(0, std::ios::beg);

		str.assign((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());

		return str;
	}

	Material Renderer::load_material(const std::string& path)
	{
		if (!path.empty())
		{
			std::ifstream file(path.c_str(), std::ifstream::in | std::ifstream::binary);

			int fileSize = 0;

			if (file.is_open())
			{
				file.seekg(0, std::ios::end);
				fileSize = int(file.tellg());
				file.close();
			}

			FILE* fp = fopen(path.c_str(), "rb"); // non-Windows use "r"

			char* readBuffer = new char[fileSize];

			rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
			rapidjson::Document document;
			document.ParseStream(is);

			// Not optional
			assert(document.HasMember("vertex_shader"));
			assert(document.HasMember("fragment_shader"));

			std::string vertex_shader_path		= document["vertex_shader"].GetString();
			std::string fragment_shader_path	= document["fragment_shader"].GetString();

			std::string dir = filesystem::directory(path.c_str()).c_str();
			dir += "/";
			std::string vertex_shader_source	= read_file_to_string(dir+vertex_shader_path);
			std::string fragment_shader_source	= read_file_to_string(dir+fragment_shader_path);

			generate_shader(vertex_shader_path,		Shader::Type::Vertex, vertex_shader_source);
			generate_shader(fragment_shader_path,	Shader::Type::Fragment, fragment_shader_source);
			generate_shader_program(filesystem::filename(path), vertex_shader_path, fragment_shader_path);

			Material material(filesystem::filename(path));

			material.shader_program = get_shader_program(filesystem::filename(path)).lock().get();

			if (document.HasMember("is_lit"))
				material.is_lit = document["is_lit"].GetBool();
			else
				material.is_lit = true;

			if (document.HasMember("Uniforms"))
			{
				for (auto& value : document["Uniforms"].GetArray())
				{
					if (value["type"] == "int")
					{
						set_uniform(value["name"].GetString(), value["value"].GetInt());
						continue;
					}
					if (value["type"] == "float")
					{
						set_uniform(value["name"].GetString(), value["value"].GetFloat());
						continue;
					}
					if (value["type"] == "Vec2")
					{
						auto arr = value["value"].GetArray();
						set_uniform(value["name"].GetString(), Vec2(arr[0].GetFloat(), arr[1].GetFloat()));
						continue;
					}
					if (value["type"].GetString() == "Vec3")
					{
						auto arr = value["value"].GetArray();
						set_uniform(value["name"].GetString(), Vec3(arr[0].GetFloat(), arr[1].GetFloat(), arr[2].GetFloat()));
						continue;
					}
					if (value["type"].GetString() == "Vec4")
					{
						auto arr = value["value"].GetArray();
						set_uniform(value["name"].GetString(), Vec4(arr[0].GetFloat(), arr[1].GetFloat(), arr[2].GetFloat(), arr[3].GetFloat()));
					}
				}
			}
			return material;
		}
		return Material("empty");
	}

	//// TODO :	For now I create material and shaders like that, maybe later on I could
	////			serialize/deserialize materials and shaders inside a repo maybe?
	//void Renderer::initialize_default_lit_material()
	//{
	//	Material material;

	//	// For now I kinda duplicate it, but I think I prefer to be able to see
	//	// everything in the same function
	//	char default_vertex_shader[] = R"(
	//		#version 330 core
	//		
	//		layout(location = 0) in vec3 position;
	//		layout(location = 1) in vec2 texture_coordinates;
	//		layout(location = 2) in vec3 normals;

	//		out vec2 uv;
	//		out vec3 normal;
	//		out vec3 fragment_position;

	//		uniform mat4 model_matrix;
	//		uniform mat4 view_matrix;
	//		uniform mat4 projection_matrix;

	//		void main()
	//		{
	//			// We get the normal matrix here
	//			normal = mat3(transpose(inverse(model_matrix))) * normals;  

	//			gl_Position = projection_matrix* view_matrix*model_matrix*  vec4(position,1.0);
	//			fragment_position = (model_matrix * vec4(position, 1.0f)).xyz;
	//			uv = texture_coordinates;
	//		}	
	//	)";


	//	char default_fragment_shader[] = R"(
	//		#version 330 core

	//		vec3 ambient_color;
	//		vec3 diffuse_color;
	//		vec3 specular_color;
	//		float shininess;

	//		struct Light
	//		{
	//			vec3 position;
	//			vec3 ambient;
	//			vec3 diffuse;
	//			vec3 specular;

	//			float constant;
	//			float linear;
	//			float quadratic;
	//		};

	//		in vec2 uv;
	//		in vec3 normal;
	//		in vec3 fragment_position;

	//		uniform Light		light;

	//		uniform vec3		camera_position;

	//		uniform sampler2D	main_texture;
	//		uniform int			use_texture;
	//		uniform vec4		main_color;

	//		out vec4 color;

	//		void main()
	//		{
	//			vec3 l_normal			= normalize(normal);
	//			vec3 camera_direction	= normalize(camera_position - fragment_position);
	//			vec3 light_direction	= normalize(light.position - fragment_position);
	//			vec3 reflect_direction	= reflect(-light_direction, l_normal);  

	//			float distance    = length(light.position - fragment_position);
	//			float attenuation = 1.0 / (light.constant + light.linear * distance + 
 //   		    light.quadratic * (distance * distance));    

	//			// Ambient

	//			vec3 ambient = (light.ambient*attenuation) * ambient_color;

	//			// Diffuse
	//			
	//			float diff		= max(dot(normal, light_direction), 0.0);
	//			vec3 diffuse	=  (light.diffuse*attenuation) * ( diffuse_color * diff);
	//			// Specular 

	//			// 32 here is the shiniess of the object 
	//			float spec = pow(max(dot(camera_direction, reflect_direction), 0.0), shininess);
	//			vec3 specular = (light.specular*attenuation) * (spec * specular_color);
	//			if(use_texture == 1)
	//			{
	//				// We discard fragments that are totally transparent
	//				if(texture( main_texture, uv ).a==0.0)
	//					discard; 
	//				color		= texture( main_texture, uv ).rgba;
	//				color.rgb		+= (ambient + diffuse + specular);
	//			}
	//			else
	//			{
	//				color	= vec4((ambient + diffuse + specular),1.0f);
	//			}
	//		}
	//	)";

	//	material.is_lit = true;

	//	material.set_uniform("ambient_color", Vec3());
	//	material.set_uniform("diffuse_color", Vec3());
	//	material.set_uniform("specular_color", Vec3());
	//	material.set_uniform("shininess", 0.0f);

	//	generate_shader("default_vertex_shader", Shader::Type::Vertex, default_vertex_shader);
	//	generate_shader("default_fragment_shader", Shader::Type::Fragment, default_fragment_shader);
	//	generate_shader_program("default_lit_shader_program", "default_vertex_shader", "default_fragment_shader");
	//	material.shader_program = get_shader_program("default_lit_shader_program").lock().get();

	//	register_material("default_lit_material", material);
	//}

	void Renderer::begin_frame_buffer(const FrameBuffer* frameBuffer)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->id_);
	}

	// Actually sets the default framebuffer
	void Renderer::end_frame_buffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Renderer::DrawList::add_rectangle(float x, float y, float width, float height)
	{
		_rectangles.push_back({ x,y,width,height });
	}

	void Renderer::DrawList::add_rectangle(float x, float y, float width, float height, float r, float g, float b, float a)
	{
		_rectangles.push_back({x,y,width,height,r,g,b,a });
	}

	void Renderer::DrawList::add_rectangle(Material& m, float x, float y, float width, float height)
	{
		_rectangles_material.push_back({x,y,width,height,&m });
	}

	void Renderer::DrawList::add_textured_rectangle(float x, float y, float width, float height, Texture* t)
	{
		Rectangle rect;
		rect.x = x;
		rect.y = y;
		rect.width = width;
		rect.height = height;
		rect.texture = t;
		_rectangles.push_back(rect);
	}

	void Renderer::DrawList::add_line(float x, float y, float x2, float y2)
	{
		_lines.push_back(
			{
		x,y,0.0f, x2, y2, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f 
			});
	}

	void Renderer::DrawList::add_line(float x, float y, float x2, float y2, float r, float g, float b, float a)
	{
		_lines.push_back
		(
			{ x,y,0.0f, x2,y2,0.0f, r,g,b,a }
		);
	}

	void Renderer::DrawList::add_line(Ray ray)
	{
		add_line(ray, 1.0f, 1.0f, 1.0f, 1.0f);
	}

	void Renderer::DrawList::add_line(Ray ray, float r, float g, float b, float a)
	{
		_lines.push_back
		(
			{
				ray.start.x,
				ray.start.y,
				ray.start.z,

				ray.start.x + (ray.direction.x * ray.length), 
				ray.start.y + (ray.direction.y * ray.length),
				ray.start.z + (ray.direction.z * ray.length),
				
				r,
				g,
				b,
				a
			}
		);
	}

	void Renderer::DrawList::add_line(float x, float y, float z, float x2, float y2, float z2)
	{
		_lines.push_back(
			{ x,y,z,x2,y2,z2,1.0f,1.0f,1.0f,1.0f }
		);
	}

	void Renderer::DrawList::add_line(float x, float y, float z, float x2, float y2, float z2, float r, float g, float b, float a)
	{
		_lines.push_back(
			{
			x,y,z,x2,y2,z2,r,g,b,a}
		);
	}

	void Renderer::DrawList::clear()
	{
		_rectangles.clear();
		_lines.clear();
		_rectangles_material.clear();
	}

	Renderer::DrawList& Renderer::window_draw_list()
	{
		return _window_draw_list;
	}

	
	void Renderer::resize_frame_buffer(FrameBuffer* frameBuffer, int width, int height)
	{
		// TODO : Remake this
		//frameBuffer->_width = width;
		//frameBuffer->_height = height;

		//glDeleteFramebuffers(1, &frameBuffer->_frameBufferObjectId);
		//glDeleteTextures(1, &frameBuffer->_color_attachment.lock().get()->id());

		//glGenFramebuffers(1, &frameBuffer->_frameBufferObjectId);
		//glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->_frameBufferObjectId);

		//glGenTextures(1, &frameBuffer->_colorAttachmentId);
		//glBindTexture(GL_TEXTURE_2D, frameBuffer->_colorAttachmentId);

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frameBuffer->_width, frameBuffer->_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		////unsigned int rbo;
		////glGenRenderbuffers(1, &rbo);
		////glBindRenderbuffer(GL_RENDERBUFFER, rbo);

		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, frameBuffer->_width, frameBuffer->_height);

		////glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBuffer->_colorAttachmentId, NULL);

		//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		//{
		//	//std::cout << "Error lol" << std::endl;
		//}
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	std::weak_ptr<Shader> Renderer::generate_shader(const std::string& name,
		Shader::Type type, const std::string& source)
	{
		GLuint id;

		switch (type)
		{
		case Shader::Type::Vertex:
			id = glCreateShader(GL_VERTEX_SHADER);
			break;
		case Shader::Type::Fragment:
			id = glCreateShader(GL_FRAGMENT_SHADER);
			break;
		default:
			break;
		}

		const char* cstr = source.c_str();
		glShaderSource(id, 1, &cstr, NULL);
		glCompileShader(id);
		
		// Error handling
		GLint success = 0;
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);

		if(success==GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::string errorLog;
			errorLog.reserve(maxLength);
			glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);

			std::string str=" Error while loading " + name + "shader";
			log_error(str.c_str());

			log_error(errorLog.c_str());

			// Provide the infolog in whatever manor you deem best.
			// Exit with failure.
			glDeleteShader(id); // Don't leak the shader.
			return std::weak_ptr<Shader>();
		}

		_shaders.push_back(std::shared_ptr<Shader>(new Shader(name,id, source, type), shader_deleter));
		return _shaders.back();
	}

	Renderer::Renderer(Window& window)
		:window(&window)
	{
		post_processing_ = std::make_unique<PostProcessing>();
	}

	std::weak_ptr<Shader> Renderer::get_shader(const std::string& name)const
	{
		for(auto& shader : _shaders)
			if (shader->_name == name)
				return shader;
		return std::weak_ptr<Shader>();
	}

	std::weak_ptr<ShaderProgram> Renderer::get_shader_program(const std::string& name)const
	{
		for (auto& shader_program : _shader_programs)
			if (shader_program->_name == name)
				return shader_program;
		return std::weak_ptr<ShaderProgram>();
	}

	void Renderer::initialize()
	{
		
	}

	std::weak_ptr<ShaderProgram> Renderer::generate_shader_program(const std::string& shader_program_name,
		const std::string& vertex_shader,
		const std::string& fragment_shader)
	{
		return generate_program(shader_program_name, get_shader(vertex_shader), get_shader(fragment_shader));
	}

	std::weak_ptr<ShaderProgram> Renderer::generate_program(const std::string& name, 
		std::weak_ptr<Shader> vertex_shader, std::weak_ptr<Shader> fragment_shader)
	{
		// PreCondition is that the vertex shader is actually a vertex shader
		// and that the fragment_shader is actually a fragment shader
		assert(vertex_shader.lock()->type() == Shader::Type::Vertex);
		assert(fragment_shader.lock()->type() == Shader::Type::Fragment);

		auto program = new ShaderProgram(name, vertex_shader.lock(), fragment_shader.lock(), glCreateProgram());

		glAttachShader(program->_id, vertex_shader.lock()->id());
		glAttachShader(program->_id, fragment_shader.lock()->id());
		check_gl_error();

		// Apparently it's possible to bind the attriblocation before hand or 
		// something, not sure if it's really needed or better to to it that way
		//glBindAttribLocation(vertex_shader, 0, "Position");
		//check_gl_error();
		//glBindFragDataLocation(geomFragProgram, 0, "FragColor");

		glLinkProgram(program->_id);
		check_gl_error();

		_shader_programs.push_back(std::shared_ptr<ShaderProgram>(program, program_deleter));
		return _shader_programs.back();
	}

	void setRectangle(float startX, float startY, float width, float height, float startTextureX, float startTextureY, float textureWidth, float textureHeight)
	{
		glTexCoord2f(startTextureX, startTextureY);
		glVertex2f(startX, startY);

		glTexCoord2f(startTextureX + textureWidth, startTextureY);
		glVertex2f(startX + width, startY);

		glTexCoord2f(startTextureX + textureWidth, startTextureY+ textureHeight);
		glVertex2f(startX + width, startY + height);

		glTexCoord2f(startTextureX, startTextureY+ textureHeight);
		glVertex2f(startX, startY + height);
	}

	void Renderer::MeshDeleter(Mesh* m)					{delete m;}
	void Renderer::TextureDeleter(Texture* t)			{delete t;}
	void Renderer::frame_buffer_deleter(FrameBuffer*f)	{delete f;}
	void Renderer::program_deleter(ShaderProgram* p)	{delete p;}
	void Renderer::shader_deleter(Shader* s)			{delete s;}

	void Renderer::check_errors()
	{
		check_gl_error();
	}

	//void Renderer::drawWindowDrawList()
	//{
	//    glViewport(0, 0, window->width(), window->height());
	//    glMatrixMode(GL_PROJECTION);
	//    
	//    auto mat = math::Matrixf::ortho
	//    (
	//        0.0f, static_cast<float>( window->width()),
	//        static_cast<float>(window->height()),0.0f ,
	//        -0.10f, 10.0f
	//    );
	//                                
	//    glLoadMatrixf((float*) &mat);

	//    glMatrixMode(GL_MODELVIEW);
	//    glLoadIdentity();

	//   /* for (auto& m : _windowDrawList.rectangles)
	//    {
	//        draw(&m);
	//    }*/
	//    //_windowDrawList.rectangles.clear();
	//}

   /* void Renderer::draw(World & world, FrameBuffer & frameb)
	{
		beginFrameBuffer(&frameb);
		Clear();
		Draw(world);
		endFrameBuffer();
	}*/

	bool Renderer::depth_sorting(const Component* a, const Component* b, Entity& ea, Entity& eb)
	{
		Vec3 va = (ea.get_component<Transform>().world_position());
		Vec3 vb = (eb.get_component<Transform>().world_position());

		if (va.z == vb.z)
		{
			return va.y > va.y;
		}

		return	va.z < vb.z;
	}

	void Renderer::draw(ColliderComponent* collider, Entity& entity)
	{
		static Material material = *ResourcesLoader::material("unlit_color.mat");
		material.enable_depth_test	= false;
		material.enable_blend		= false;

		if (collider->colliding)
			material.set_uniform("main_color", Vec4(1.0f, 0.0f, 0.0f, 1.0f));
		else
			material.set_uniform("main_color", Vec4(0.0f, 1.0f, 0.0f, 1.0f));

		material.mode = Material::PolygonMode::Line;

		begin_material(material);
			draw(*collider->_mesh, entity.get_component<Transform>().world_matrix());
		end_material(material);
	}

	PostProcessing* Renderer::post_processing()
	{
		return post_processing_.get();
	}

	void Renderer::begin_default_frame_buffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Renderer::clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		check_gl_error();
		glClear(GL_STENCIL_BUFFER_BIT);
	}

	void Renderer::draw(const Canvas& canvas)
	{
		for(auto frame : canvas.frames)
		{
			DrawList::Rectangle rectangle;
			rectangle.x			= frame.x;
			rectangle.y			= frame.y;
			rectangle.width		= frame.width;
			rectangle.height	= frame.height;
			rectangle.r			= frame.r / 255.0f;
			rectangle.g			= frame.g / 255.0f;
			rectangle.b			= frame.b / 255.0f;
			rectangle.a			= 1.0f;

			rectangle.texture = frame.texture_;
			draw(rectangle);
		}
	}

	// TODO : This probably needs more granularity, make some stuff into functions
	// because it's hard to understand what's actually going on here
	void Renderer::draw()
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		Scene& scene = Game::scene();

		// Sort the camera by their order value
		//std::sort(scene.cameras.begin(), scene.cameras.end());
		
		for (auto& camera : *scene.pools().get<Camera>())
		{
			_current_camera = &camera.second;

			auto& camera_entity = *scene.entities_[camera.first];

			// Set the framebuffer attached to the camera. If the camera
			// has no framebuffer attached, we use the default one
			if(_current_camera->framebuffer_)
			{
				begin_frame_buffer(_current_camera->framebuffer_.get());
			}
			else
			{
				begin_default_frame_buffer();
			}

			set_clear_color(_current_camera->clear_color());

			// Probably should add a flag to the camera too, to tell
			// if it's supposed to used a depth buffer or not
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			check_gl_error();

			//glStencilMask(~0);
			//glDisable(GL_SCISSOR_TEST);
			glClear(GL_STENCIL_BUFFER_BIT);


			// TODO :	A Renderer is linked to a window, so it would make more sense
			//			for the renderer to have a reference to the window it's associated
			//			with. I could even go through window.draw(), which then calls
			//			the renderer with the correct default width/height
			if (_current_camera->default_viewport_)
			{
				set_viewport(0, 0, Game::window().width(), Game::window().height());
			}
			else
			{
				set_viewport(_current_camera->viewport());
			}

			_projection_matrix		= _current_camera->projection_matrix();
			_view_matrix			= camera_entity.get_component<Transform>().world_matrix().inverse();

			//  TODO : Use a priority_queue and use this type underneath
			std::map<int, std::vector<std::pair<const RendererComponent*,Entity*>>> render_queues;

			auto * renderingSystem = &scene.systems().get<RenderingSystem>();
			
			for(auto r : renderingSystem->renderer_components_)
			{
				//TODO : Change how we set the layer in entity so I don't make this mistake again
				// of shifting the bits here
				// We only display the current renderer if the layers are valid
				if(( (int_64(1)<< int_64(r.second->layer_)) & _current_camera->culling_layers().layers())!= 0)
				{
					render_queues[r.first->material.render_queue].push_back({r.first, r.second});
				}
			}

			for(auto& render_queue : render_queues)
			{
				// We're making 2 drawing list. One for the opaque stuff, and one for the 
			// transparent stuff

				// So the problem is that I sort the material by using the map thingie

				opaques_materials_.clear();
				transparent_materials_.clear();
				opaques_.clear();
				transparents_.clear();

				//  TODO :  Probably should also use a priority_queue here 
				for (auto& r : render_queue.second)
				{
					if (r.first->material.is_transparent)
					{
						// TODO : I should really consider adding a util/vector for this kind of stuff
						//			so I don't rewrite it a thousand time, because this is a bit tricky
						auto rr = std::find_if(transparent_materials_.begin(), transparent_materials_.end(), [&](const Material* material)
						{
							return ((*material) == r.first->material);
						});

						if (rr != transparent_materials_.end())
						{
							transparents_[std::distance(transparent_materials_.begin(), rr)].push_back(r);
						}
						else
						{
							transparent_materials_.push_back(&r.first->material);
							transparents_.push_back(std::vector<std::pair<const RendererComponent*, Entity*>>());
							transparents_.back().push_back(r);
						}
					}
					else
					{
						// TODO : I should really consider adding a util/vector for this kind of stuff
						//			so I don't rewrite it a thousand time, because this is a bit tricky
						auto rr = std::find_if(opaques_materials_.begin(), opaques_materials_.end(), [&](const Material* material)
						{
							return ( (*material) == r.first->material);
						});

						if(rr != opaques_materials_.end())
						{
							opaques_[std::distance(opaques_materials_.begin(), rr)].push_back(r);
						}
						else
						{
							opaques_materials_.push_back(&r.first->material);
							opaques_.emplace_back();
							opaques_.back().push_back(r);
						}
					}
						
				}

				// Here we're suppose to sort the transparent object
				// from back to front but it probably sucks a little bit
				for (auto& transparent : transparents_)
				{
					//std::sort(transparent.begin(), transparent.end(), Renderer::depth_sorting);
				}
					

				// TODO :	Also I should sort opaque objects from front to back
				//			Makes the thing more optimized since Opengl will skip
				//			some fragments using the depth buffer value

				int i = 0;

				// drawing opaque stuff first
				for (auto& o : opaques_)
				{
					draw(o, *opaques_materials_[i++]);
				}

				check_gl_error();

				i = 0;
				for (auto& t : transparents_)
				{
					draw(t, *transparent_materials_[i++]);
				}

				check_gl_error();
			}

			check_gl_error();
			
			if (_show_colliders)
			{
				for (auto& collider : (*scene.pools().get<BoxCollider>()))
				{
					auto& collider_entity = *scene.entities_[collider.first];

					if(collider.second.is_enabled() && collider_entity.enabled_)
					{
						draw(&collider.second,collider_entity);
					}
				}
			}

			check_gl_error();
			draw(_world_draw_list);
			check_gl_error();
		}

		glDisable(GL_DEPTH_TEST);

		// This was supposed to be my post rendering things
		// TODO : See what shit I did here

		begin_default_frame_buffer();

		_view_matrix.identity();

		_projection_matrix.identity();
		//post_processing_->update(Game::scene().find("MainCamera")->get_component<Camera>(),*this);


		_projection_matrix = Matrix::ortho(
			0.0f, static_cast<float>(window->width()),
			0.0f, static_cast<float>(window->height()),
			-1.0f, 100.0f
		);
		draw(window_draw_list());
	}

	struct UniformVisitor
	{
		// Copying the string kinda suck
		const char* name;

		void operator()(int n)const
		{
			Game::renderer().set_uniform(name, n);
		}

		void operator()(float n)const
		{
			Game::renderer().set_uniform(name, n);
		}

		void operator()(Vec2 v)const
		{
			Game::renderer().set_uniform(name, v);
		}

		void operator()(Vec3 v)const
		{
			Game::renderer().set_uniform(name, v);
		}

		void operator()(Vec4 v)const
		{
			Game::renderer().set_uniform(name, v);
		}
	};

	void Renderer::begin_material(const Material& material)
	{
		current_material = &material;

		if (material.enable_depth_test)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
		check_gl_error();

		if (!material.depth_mask)
			glDepthMask(GL_FALSE);
		else
			glDepthMask(GL_TRUE);

		if (material.enable_stencil_test)
		{
			glEnable(GL_STENCIL_TEST);
			glClear(GL_STENCIL_BUFFER_BIT);	//I guess I only have to clear it when it's enabled
											// also maybe I don't need to clear it for every material
		}
		else
		{
			glStencilMask(~0);
			glDisable(GL_SCISSOR_TEST);
			glDisable(GL_STENCIL_TEST);
		}

		if (material.enable_stencil_test)
		{
			if (material.stencil_mask)
				glStencilMask(0xFF);
			else
				glStencilMask(0x00);

			switch (material.stencil_test)
			{
			case StencilTest::Always:
				// Means we always write in the stencil test
				glStencilFunc(GL_ALWAYS, 0x1, 0x1);
				break;

			case StencilTest::NotEqual:
				glStencilFunc(GL_NOTEQUAL, 0x1, 0x1);
				break;
			}

			if (material.stencil_op == StencilOp::Replace)
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			if (material.stencil_op == StencilOp::Keep)
				glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		}

		if (!material.render_color)
			glColorMask(0, 0, 0, 0);
		else
			glColorMask(1, 1, 1, 1);

		switch (material.depth_test)
		{
		case DepthTest::Always:
			glDepthFunc(GL_ALWAYS);
			break;
		case DepthTest::Never:
			glDepthFunc(GL_NEVER);
			break;
		case DepthTest::Less:
			glDepthFunc(GL_LESS);
			break;
		case DepthTest::Equal:
			glDepthFunc(GL_EQUAL);
			break;
		case DepthTest::LEqual:
			glDepthFunc(GL_LEQUAL);
			break;
		case DepthTest::Greater:
			glDepthFunc(GL_GREATER);
			break;
		case DepthTest::NotEqual:
			glDepthFunc(GL_NOTEQUAL);
			break;
		case DepthTest::GEqual:
			glDepthFunc(GL_GEQUAL);
			break;
		}

		switch (material.mode)
		{
		case Material::PolygonMode::Fill:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case Material::PolygonMode::Line:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		}

		check_gl_error();

		if (material.enable_blend)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			glDisable(GL_BLEND);
		}
			
		check_gl_error();

		glUseProgram(material.shader_program->id());
		check_gl_error();

		// Handle textures

		if (!material.textures_.empty())
			glEnable(GL_TEXTURE_2D);

		int texture_index = 0;
		for (auto pair: material.textures_)
		{
			const GLint texture_location = glGetUniformLocation(material.shader_program->id(), pair.first.c_str());
			if (texture_location != -1)
			{
				glActiveTexture(GL_TEXTURE0 + texture_index); // Texture unit 0
				check_gl_error();
				glBindTexture(GL_TEXTURE_2D, pair.second->id());
				check_gl_error();
				glUniform1i(texture_location, texture_index);
				check_gl_error();
			}
			texture_index++;
		}

		check_gl_error();

		// member variable because needed inside the draw function

		// 3 matrix, kinda almost always needed 
		// Model matrix is sent from the draw function
		model_matrix_id				= glGetUniformLocation(material.shader_program->id(), "model_matrix");

		// Handle common matrix

		set_uniform("view_matrix",			_view_matrix);
		set_uniform("projection_matrix",	_projection_matrix);
		
		// Handle lighting

		//if (material.is_lit)	
		//{
		//	auto light = Game::scene()._point_lights.front();

		//	set_uniform("light.ambient",	light->ambient);
		//	set_uniform("light.diffuse",	light->diffuse);
		//	set_uniform("light.specular",	light->specular);
		//	set_uniform("light.constant",	light->constant);
		//	set_uniform("light.linear",		light->linear);
		//	set_uniform("light.quadratic",	light->quadratic);
		//	set_uniform("light.position",	light->entity().transform().world_position());

		//	// This is also kinda light related but I'm not really sure how to actually handle it
		//	// I probably need the ability to create material and manually add/remove uniforms stuff
		//	/*set_uniform("material.ambient_color",	material.u);
		//	set_uniform("material.diffuse_color",	material.diffuse_color);
		//	set_uniform("material.specular_color",	material.specular_color);
		//	set_uniform("material.shininess",		material.shininess);*/

		//	// The camera position is also needed as a part of the lighting shader thing
		//	if (_current_camera != nullptr)
		//		set_uniform("camera_position", _current_camera->entity().transform().world_position());
		//}

		UniformVisitor visitor;

		for (auto& uniform : material.uniforms_)
		{
			visitor.name = uniform.first.c_str();
			std::visit(visitor, uniform.second);
		}
	}

	void Renderer::set_uniform(const std::string& name, float v)
	{
		GLint location = glGetUniformLocation(current_material->shader_program->id(), name.c_str());
		glUniform1f(location, v);
	}

	void Renderer::set_uniform(const std::string& name, int v)
	{
		const GLint location = glGetUniformLocation(current_material->shader_program->id(), name.c_str());
		glUniform1i(location, v);
	}

	void Renderer::set_uniform(const std::string& name, Vec3 v)
	{
		const GLint location = glGetUniformLocation(current_material->shader_program->id(), name.c_str());
		glUniform3fv(location, 1, &v.x);
	}

	void Renderer::set_uniform(const std::string& name, Vec4 v)
	{
		const GLint location = glGetUniformLocation(current_material->shader_program->id(), name.c_str());
		glUniform4fv(location, 1, &v.x);
	}

	void Renderer::set_uniform(const std::string& name, Matrix m)
	{
		const GLint location = glGetUniformLocation(current_material->shader_program->id(), name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, m.data());
	}

	void Renderer::end_material(const Material& material)
	{
		current_material = nullptr;
		glUseProgram(0);

		int t = 0;
		for (const auto& pair: material.textures_)
		{
			glActiveTexture(GL_TEXTURE0 + t);
			glBindTexture(GL_TEXTURE_2D, 0);
			t++;
		}
		glDisable(GL_TEXTURE_2D);
	}

	void Renderer::draw(const std::vector<std::pair<const RendererComponent*,Entity*>>& renderers, const Material& material)
	{
		begin_material(material);

		check_gl_error();
		
		for (auto& renderer : renderers)
		{
			// Todo : probably not the best thing here
			draw(*renderer.first->_mesh,renderer.second->get_component<Transform>().world_matrix());
		}
		
		end_material(material);
		check_gl_error();
	}

	void Renderer::draw(const Mesh& mesh, const Matrix& matrix)
	{
		// Send our transformation to the currently bound shader, in the "MVP" uniform
		// This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
		glUniformMatrix4fv(model_matrix_id,			1, GL_FALSE, matrix.data());
		check_gl_error();
		glBindVertexArray(mesh.vao_id_);
		check_gl_error();
		switch (mesh.primitive_type())
		{
		case PrimitiveType::Triangles:
			glDrawElements(GL_TRIANGLES, mesh.indexes_.size(), GL_UNSIGNED_INT, (void*)0);
			break;

		case PrimitiveType::Quads:
			glDrawElements(GL_QUADS, mesh.indexes_.size(), GL_UNSIGNED_INT, (void*)0);
			break;
		}
		GLenum result;
		while ((result = glGetError()) != GL_NO_ERROR)
		{
			switch (result)
			{
			case GL_INVALID_ENUM:
				log_error("Invalid Enum");
				break;

			case GL_INVALID_VALUE:
				log_error("Invalid Value");
				break;

			case GL_INVALID_OPERATION:
				log_error("Invalid Operation");
				break;

			case GL_INVALID_FRAMEBUFFER_OPERATION:
				log_error("Invalid Framebuffer Operation");
				break;

			case GL_OUT_OF_MEMORY:
				log_error("Out of Memory");
				break;
			default:
				break;
			}
		}
		
		glBindVertexArray(0);
		check_gl_error();
		/*glBindBuffer(GL_ARRAY_BUFFER, 0);
		check_gl_error();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		check_gl_error();*/
	}

	void Renderer::draw(DrawList::Rectangle& rectangle)
	{
		// Create a new mesh
		auto mesh = Mesh::new_standard_mesh();

		// Slightly hacky way to handle the fact that the window space
		// is inverted in y axis but ok
		float coef = 1.0f;
		//if (drawlist.is_window_space)
		//{
			mesh->add_vertex().position(rectangle.x, window->height() - rectangle.y, 0.0f)
				.uv(0.0f, 1.0f);
			mesh->add_vertex().position(rectangle.x + rectangle.width, window->height() - rectangle.y, 0.0f).
				uv(1.0f, 1.0f);
			mesh->add_vertex().position(rectangle.x + rectangle.width, window->height() - (rectangle.y + rectangle.height), 0.0f).
				uv(1.0f, 0.0f);
			mesh->add_vertex().position(rectangle.x, window->height() - (rectangle.y + rectangle.height), 0.0f).
				uv(0.0f, 0.0f);
		/*}
		else
		{
			mesh->add_vertex().position(rectangle.x, rectangle.y, 0.0f);
			mesh->add_vertex().position(rectangle.x + rectangle.width, rectangle.y, 0.0f);
			mesh->add_vertex().position(rectangle.x + rectangle.width, (rectangle.y + rectangle.height), 0.0f);
			mesh->add_vertex().position(rectangle.x, (rectangle.y + rectangle.height), 0.0f);
		}*/

		mesh->add_triangle(0, 1, 2);
		mesh->add_triangle(0, 2, 3);

		mesh->update_vertices();

		//TODO  : For now I just want to check the depth texture, change this later
		// on for something more generic that maybe can get a material
		static Material material = *ResourcesLoader::material("unlit_texture.mat");
		material.set_uniform("main_color", Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		material.enable_blend = false;
		material.enable_depth_test = false;
		material.add_texture(rectangle.texture, "main_texture");
		//material.shader_program	= get_shader_program("Depth").lock().get();

		begin_material(material);
		draw(*mesh, Matrix());
		end_material(material);
	}

	void Renderer::draw(DrawList& drawlist) 
	{
		for (auto& line : drawlist._lines)
		{
			auto mesh = Mesh::new_standard_mesh();

			// TODO : Build a Cylinder here instead of doing this crap that only
			// works on the x-y plan
			Vec3 a(line.x, line.y, line.z);
			Vec3 b(line.x2, line.y2, line.z2);

			Vec2 c(b.x - a.x, b.y - a.y);

			float line_width = 0.5f;
			c = c.orthogonal_vector();
			c.normalize();
			c *= line_width;

			mesh->vertices().resize(4 * 8, 0.0f);

			auto& vertices = mesh->vertices();

			vertices[0]		= line.x - c.x;
			vertices[1]		= line.y - c.y;
			vertices[2]		= line.z;

			vertices[8]		= line.x2 - c.x;
			vertices[9]		= line.y2 - c.y;
			vertices[10]	= line.z2;

			vertices[16]	= line.x2 + c.x;
			vertices[17]	= line.y2 + c.y;
			vertices[18]	= line.z2;

			vertices[24]	= line.x + c.x;
			vertices[25]	= line.y + c.y;
			vertices[26]	= line.z;

			mesh->indexes().insert(mesh->indexes().begin(), { 0,1,2,0,2,3 });
			mesh->update_vertices();

			static Material material = *ResourcesLoader::material("unlit_color.mat");
			material.set_uniform("main_color", Vec4(line.r, line.g, line.b, line.a));
			material.enable_depth_test	= false;
			material.enable_blend		= false;

			Matrix identity;

			begin_material(material);
			draw(*mesh, identity);
			end_material(material);
		}

		for(auto& rectangle : drawlist._rectangles_material)
		{
			// Create a new mesh
			auto mesh = Mesh::new_standard_mesh();

			// Slightly hacky way to handle the fact that the window space
			// is inverted in y axis but ok
			float coef = 1.0f;

			if (drawlist.is_window_space)
			{
				mesh->add_vertex().position(rectangle.x, window->height() - rectangle.y, 0.0f)
					.uv(0.0f, 1.0f);
				
				mesh->add_vertex().position(rectangle.x + rectangle.width, window->height() - rectangle.y, 0.0f).
					uv(1.0f, 1.0f);
				
				mesh->add_vertex().position(rectangle.x + rectangle.width, window->height() - (rectangle.y + rectangle.height), 0.0f).
					uv(1.0f, 0.0f);
				
				mesh->add_vertex().position(rectangle.x, window->height() - (rectangle.y + rectangle.height), 0.0f).
					uv(0.0f, 0.0f);
			}
			
			mesh->add_triangle(0, 1, 2);
			mesh->add_triangle(0, 2, 3);

			mesh->update_vertices();
			Matrix identity;

			begin_material(*rectangle.material);
			draw(*mesh, identity);
			end_material(*rectangle.material);
		}

		for (auto& rectangle : drawlist._rectangles)
		{
			draw(rectangle);
		}
	}

	Renderer::DrawList& Renderer::world_draw_list()
	{
		return _world_draw_list;
	}

	//void Renderer::DrawTexts(ComponentPool<Text>* texts, ComponentPool<Transform>* transforms)
	//{
	//    for (int entityIndex : texts->registered_components())
	//    {
	//        Text& text = *texts->get_raw(entityIndex);

	//        float x = 0;
	//        float y = 0.0f;
	//        int line = 0;
	//        float start_x = x;

	//        float sx = 0.20f;
	//        float sy = 0.20f;

	//        if (text.font->texture)
	//        {
	//            LoadWorldMatrix(transforms->get_raw(entityIndex));

	//            // This doesn't work, maybe I should make a 1 byte texture
	//            // for the image?
	//            glColor4f(text.r, text.g, text.b, text.a);

	//            beginTexture(text.font->texture);
	//            glBegin(GL_QUADS);

	//            for (int i = 0; i < text.str.size(); i++)
	//            {
	//                char c = text.str[i];

	//                if (c == '\n')
	//                {
	//                    x = start_x;
	//                    line++;
	//                    continue;
	//                }

	//                int ssss = sizeof(CharacterInfo);
	//                float x2 = x + text.font->characters[c].bl * sx;
	//                float y2 = -y - text.font->characters[c].bt * sy;
	//                float w = text.font->characters[c].bw *sx;
	//                float h = text.font->characters[c].bh *sy;

	//                float offset_y = line * text.font->texture->height()*sy;

	//                /* Advance the cursor to the start of the next character */
	//                x += text.font->characters[c].ax*sx;
	//                y += text.font->characters[c].ay*sy;

	//                /* Skip glyphs that have no pixels */
	//                if (!w || !h)
	//                    continue;

	//                // Height of the character, since the bitmap is one line
	//                float tex_h = text.font->characters[c].bh / text.font->texture->height();
	//                float offset_text_x = text.font->characters[c].tx;
	//                float offset_plus_x = text.font->characters[c].tx + text.font->characters[c].bw / text.font->texture->width();

	//                glTexCoord2f(offset_text_x, 1.0f - tex_h);
	//                glVertex2f(x2, -y2 - h - offset_y);

	//                glTexCoord2f(offset_plus_x, 1.0f - tex_h);
	//                glVertex2f(x2 + w, -y2 - h - offset_y);

	//                glTexCoord2f(offset_plus_x, 1.0f);
	//                glVertex2f(x2 + w, -y2 - offset_y);

	//                glTexCoord2f(offset_text_x, 1.0f);
	//                glVertex2f(x2, -y2 - offset_y);
	//            }
	//            glEnd();
	//        }
	//    }
	//}

	void Renderer::set_clear_color(Color c)
	{
		glClearColor(
			static_cast<GLfloat>(c.r),  static_cast<GLfloat>(c.g),
			static_cast<GLfloat>(c.b),  static_cast<GLfloat>(c.a));
	}

	void Renderer::set_viewport(const Viewport& v)
	{
		set_viewport(static_cast<GLint>(v.x), static_cast<GLint>(v.y), static_cast<GLint>(v.width),
			static_cast<GLint>(v.height));
	}

	void Renderer::set_viewport(int x, int y, int width, int height)
	{
		glViewport(x, y,width, height);
	}

   
   /* void Renderer::WindowDrawList::reserve(int size)
	{
		rectangles.resize(size);
	}*/

   /* void Renderer::WindowDrawList::AddRectangle(float x, float y, float width, float height, Color color)
	{
		MeshRenderer::Vertex * vs = new MeshRenderer::Vertex[4];

		vs[0] = { x,			y,			0.0f, color.r, color.g, color.b,color.a, 0.0f, 0.0f };
		vs[1] = { x + width,	y,			0.0f, color.r, color.g, color.b,color.a, 0.0f, 0.0f };
		vs[2] = { x + width,	y + height,	0.0f, color.r, color.g, color.b,color.a, 0.0f, 0.0f };
		vs[3] = { x,			y + height,	0.0f, color.r, color.g, color.b,color.a, 0.0f, 0.0f };
			
		MeshRenderer m(vs, 4, MeshRenderer::PrimitiveType::Quads);
		rectangles.add(m);
	}*/

	void tex_sub_image(int mipmap_level, int x, int y, int width, int height, Image::Format format,
		void* pixels)
	{
		if (format == Image::Format::RGBA_8888)
			glTexSubImage2D(GL_TEXTURE_2D, mipmap_level, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	}
}