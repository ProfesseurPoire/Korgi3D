cmake_minimum_required(VERSION 3.13.0) # need 3.13 for target_sources

project(TestFileSystem CXX)

add_executable(${PROJECT_NAME} "")

add_subdirectory(src)

target_link_libraries(${PROJECT_NAME} PRIVATE CorgiFilesystem CorgiTest)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
