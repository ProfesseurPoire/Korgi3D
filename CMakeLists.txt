cmake_minimum_required(VERSION 3.12.0)

project(cabba)

set(Architecture "x86" CACHE STRING "BaseName chosen by the user at CMake configure time")

enable_testing()

set_property(CACHE Architecture PROPERTY STRINGS x86 x64)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/b)

add_subdirectory(src/corgi/string)
add_subdirectory(src/corgi/test)
add_subdirectory(src/corgi/memory)
add_subdirectory(src/corgi/containers)
add_subdirectory(src/corgi/math)
add_subdirectory(src/corgi/logger)
add_subdirectory(src/corgi/ecs)
add_subdirectory(src/corgi/filesystem)
add_subdirectory(tools)
add_subdirectory(tests)

add_subdirectory(ext/glfw/)

configure_file(src/config.h.in config.h)

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_library(CorgiEngine STATIC
	src/corgi/components/Transform.cpp
	src/corgi/components/SpriteRenderer.cpp
	src/corgi/components/Camera.cpp
	src/corgi/components/Animator.cpp
	src/corgi/components/StateMachine.cpp
	src/corgi/components/Panel.cpp
	src/corgi/components/BoxCollider.cpp
	src/corgi/components/Tilemap.cpp
	src/corgi/components/ParticleEmitter.cpp
	src/corgi/components/MeshRenderer.cpp
	src/corgi/components/MeshCollider.cpp
	src/corgi/components/ColliderComponent.cpp
	src/corgi/components/TextRenderer.cpp

	src/corgi/components/lights/PointLight.cpp

	src/corgi/ui/Canvas.cpp

	src/corgi/main/Game.cpp
	src/corgi/main/Window.cpp
	src/corgi/main/Settings.cpp
	src/corgi/main/AudioPlayer.cpp
	
	src/corgi/inputs/Keyboard.cpp
	src/corgi/inputs/Inputs.cpp
	src/corgi/inputs/GamePad.cpp

	src/corgi/rendering/renderer.cpp
	src/corgi/rendering/texture.cpp
	src/corgi/rendering/FrameBuffer.cpp
	src/corgi/rendering/Color.cpp
	src/corgi/rendering/Mesh.cpp
	src/corgi/rendering/Material.cpp
	src/corgi/rendering/Shader.cpp
	src/corgi/rendering/RenderCommand.cpp
	src/corgi/rendering/PostProcessing.cpp
	src/corgi/rendering/PostProcessing.h

	src/corgi/resources/image.cpp
	src/corgi/resources/Font.cpp
	src/corgi/resources/Sound.cpp
	
	src/corgi/systems/CameraSystem.cpp
	src/corgi/systems/TransformSystem.cpp
	src/corgi/systems/AnimatorSystem.cpp
	src/corgi/systems/StateMachineSystem.cpp
	src/corgi/systems/CollisionSystem.cpp
	src/corgi/systems/AnimationSystem.cpp
	
	src/corgi/utils/Rectangle.cpp 
	src/corgi/utils/TimeHelper.cpp 
	src/corgi/utils/ResourcesLoader.cpp
	src/corgi/utils/TiledImporter.cpp 
	src/corgi/utils/AsepriteImporter.cpp
	src/corgi/utils/Physic.cpp
	src/corgi/utils/EntityPool.cpp

	src/corgi/glad/glad.c) 

target_include_directories(CorgiEngine 
	PUBLIC
	${CMAKE_SOURCE_DIRECTORY}ext/glfw/include
	${CMAKE_SOURCE_DIRECTORY}ext/glad/include/
	${CMAKE_SOURCE_DIRECTORY}ext/openal/include/
	${CMAKE_SOURCE_DIRECTORY}ext/SDL/include/
	${CMAKE_SOURCE_DIRECTORY}ext/SDLMixer/include
	${CMAKE_SOURCE_DIRECTORY}src/
	PRIVATE
	${CMAKE_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

set_property(TARGET CorgiEngine PROPERTY CXX_STANDARD 20)

target_precompile_headers(CorgiEngine
  PUBLIC 
  "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/corgi/main/corgi_pch.h>"
)

if(MSVC)
	target_link_libraries(CorgiEngine 
	PUBLIC
	CorgiECS
	CorgiMemory
	CorgiContainers
	CorgiMath   
	CorgiFilesystem
	CorgiLogger
	CorgiString
	opengl32	
	${CMAKE_SOURCE_DIR}/ext/SDL/lib/x86/SDL2.lib
	${CMAKE_SOURCE_DIR}/ext/SDLMixer/lib/x86/SDL2_mixer.lib)
else()
	target_link_libraries(corgiEngine PUBLIC
	CorgiECS
	CorgiLogger
	CorgiContainers
	CorgiMemory
	CorgiMath   
	CorgiFilesystem
	CorgiString
	GL
	stdc++fs)
endif()