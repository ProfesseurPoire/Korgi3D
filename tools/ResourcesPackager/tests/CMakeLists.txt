cmake_minimum_required(VERSION 3.13.0)

project(TestResourcePackager LANGUAGES CXX VERSION 1.0.0)

configure_file(config.h.in config.h)

add_executable(${PROJECT_NAME} "")
add_subdirectory(src)

target_include_directories(${PROJECT_NAME} 
PUBLIC
${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE CorgiTest ResourcesPackagerLib)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})