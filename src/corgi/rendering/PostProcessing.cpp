#include <corgi/rendering/PostProcessing.h>
#include <corgi/rendering/Mesh.h>
#include <corgi/rendering/renderer.h>

#include <corgi/components/Camera.h>

#include <corgi/utils/ResourcesLoader.h>

namespace corgi
{
	void PostProcessing::Step::update(Renderer& renderer)
	{
		Mesh mesh({
					VertexAttribute("position", 0, 3),
					VertexAttribute("texture_coordinates",3, 2)
			});

		mesh.add_vertex()
			.position(-1.0f, -1.0f, 0.0f)
			.uv(0.0f, 0.0f);

		mesh.add_vertex()
			.position(1.0f, -1.0f, 0.0f)
			.uv(1.0f, 0.0f);

		mesh.add_vertex()
			.position(1.0f, 1.0f, 0.0f)
			.uv(1.0f, 1.0f);

		mesh.add_vertex()
			.position(-1.0f, 1.0f, 0.0f)
			.uv(0.0f, 1.0f);

		mesh.add_triangle(0, 1, 2);
		mesh.add_triangle(0, 2, 3);

		mesh.update_vertices();

		renderer.begin_frame_buffer(&framebuffer);
		renderer.clear();
		renderer.begin_material(material);
		renderer.draw(mesh, Matrix());
		renderer.end_material(material);
		renderer.end_frame_buffer();
	}

    void PostProcessing::update(const Camera& camera, Renderer& renderer)
    {
        for(auto& step : steps_)
            step.update(renderer);

        render_on_screen(camera, renderer);
    }

    void PostProcessing::render_on_screen(const Camera& main_camera, Renderer& renderer)
    {
		static Material material = *ResourcesLoader::material("unlit_texture.mat");

		// If no step has been defined, we simply used the main camera framebuffer
		if(steps_.empty())
			material.add_texture(main_camera.framebuffer()->color_attachment(),"main_texture");
		else
			material.add_texture(steps_.back().framebuffer.color_attachment(), "main_texture");

		Mesh mesh({
					VertexAttribute("position", 0, 3),
					VertexAttribute("texture_coordinates",3, 2)
			});

		mesh.add_vertex()
			.position(-1.0f, -1.0f, 0.0f)
			.uv(0.0f, 0.0f);

		mesh.add_vertex()
			.position(1.0f, -1.0f, 0.0f)
			.uv(1.0f, 0.0f);

		mesh.add_vertex()
			.position(1.0f, 1.0f, 0.0f)
			.uv(1.0f, 1.0f);

		mesh.add_vertex()
			.position(-1.0f, 1.0f, 0.0f)
			.uv(0.0f, 1.0f);

		mesh.add_triangle(0, 1, 2);
		mesh.add_triangle(0, 2, 3);

		mesh.update_vertices();

		renderer.begin_default_frame_buffer();
		renderer.clear();
		renderer.begin_material(material);
		renderer.draw(mesh, Matrix());
		renderer.end_material(material);

		renderer.end_frame_buffer();
    }
}