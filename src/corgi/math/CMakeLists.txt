cmake_minimum_required(VERSION 3.9.0)

project(corgi-math VERSION 1.0.0)

set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS TRUE)

add_library(${PROJECT_NAME} STATIC
	Matrix.cpp
	MathUtils.cpp
	MathUtils.h
	Ray.cpp
	Collisions.cpp
)

set_property(TARGET corgi-math PROPERTY CXX_STANDARD 20)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../>
    $<INSTALL_INTERFACE:../../>)


target_link_libraries(${PROJECT_NAME} corgi-logger)

#add_subdirectory(UnitTests)