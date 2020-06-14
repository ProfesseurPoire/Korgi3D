cmake_minimum_required(VERSION 3.9.0)

project(imgui VERSION 1.0.0)

set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS TRUE)

add_library(${PROJECT_NAME} STATIC
	imconfig.h
	imgui.cpp
	imgui.h
	imgui_demo.cpp
	imgui_draw.cpp
	imgui_impl_glfw.cpp
	imgui_impl_glfw.h
	imgui_impl_opengl2.cpp
	imgui_impl_opengl2.h
	imgui_internal.h
	imgui_widgets.cpp
	imstb_rectpack.h
	imstb_textedit.h
	imstb_truetype.h
)

set_property(TARGET imgui PROPERTY CXX_STANDARD 20)


target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/ext/glfw/include/>
    $<INSTALL_INTERFACE:../>)

	
target_link_libraries( imgui 
    PUBLIC
    opengl32
    ${CMAKE_SOURCE_DIR}/ext/glfw/x86/glfw3.lib
)
