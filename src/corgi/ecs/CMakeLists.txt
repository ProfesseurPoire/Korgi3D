cmake_minimum_required(VERSION 3.9.0)

project(CorgiECS VERSION 1.0.0)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS TRUE)

add_library(${PROJECT_NAME} STATIC
	src/Component.cpp
    src/ComponentPool.cpp
    src/Entity.cpp
    src/Entities.Cpp
    src/Scene.cpp
    src/System.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_link_libraries(${PROJECT_NAME} PRIVATE CorgiContainers CorgiMemory CorgiLogger)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:../>)