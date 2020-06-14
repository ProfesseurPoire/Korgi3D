cmake_minimum_required(VERSION 3.9.0)

project(corgi-containers)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME}
INTERFACE 
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
$<INSTALL_INTERFACE:include>)

add_subdirectory(tests)