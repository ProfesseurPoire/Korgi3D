#include <corgi/test/test.h>

#include <corgi/main/Game.h>
#include <corgi/ec/Entity.h>

#include <corgi/components/SpriteRenderer.h>
#include <corgi/components/Camera.h>

#include "config.h"

using namespace corgi;

int main()
{
	corgi::test::run_all();
}

/*!
 * @brief   This is a test that will check if everything engine related is in place
 *          by running only 1 entity with 1 component for 1 frame. That way I can always check if the 
 *          code at least compiles and run on different platforms using Travis CI or Jenkins
 */
TEST(TestRun, Test)
{
    Game::initialize(PROJECT_RESOURCE_DIRECTORY, LIBRARY_RESOURCE_DIRECTORY);

	Game::initialize_window
	(
		"TestRun",
		500,	// TODO 
		100,
		200,200,
		-1,
		false
	);

	//Physic::set_collision(0, 1);

	auto& camera_entity = Scene::new_entity("Camera");
	auto& camera = camera_entity.add_component<Camera>();

	camera.ortho(100, 16.0f / 9.0f, 0.0F, 100.0f);
	camera.viewport(0, 0, 200, 200);
	
	
	auto& entity = Scene::new_entity("Entity");
	auto& spr = entity.add_component<SpriteRenderer>();
	//spr.texture(Resources::texture("Corgi.png"));
	
	Game::update_inputs();
	
	Game::scene().update(0.1f);
	
	
	Game::renderer().draw();
	Game::window().swap_buffers();
	Game::finalize();
}