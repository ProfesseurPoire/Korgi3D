cmake_minimum_required(VERSION 3.13.0)

project(corgi-math LANGUAGES CXX VERSION 1.0.0 )

# Not really sure what this does already haha
#set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS TRUE)

add_library(${PROJECT_NAME} STATIC "")

add_subdirectory(src)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(${PROJECT_NAME} PUBLIC 
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
$<INSTALL_INTERFACE:./../>)

add_subdirectory(tests)