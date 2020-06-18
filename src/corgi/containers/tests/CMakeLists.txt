cmake_minimum_required(VERSION 3.13.0) # need 3.13 for target_sources

project(TestContainers CXX)

add_executable(${PROJECT_NAME} "")

target_sources(${PROJECT_NAME}
    PUBLIC 
    unit/FilledTree.cpp 
    unit/EmptyTree.cpp)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    corgi-containers
    corgiTest)

target_include_directories(TestContainers PRIVATE ./)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

add_test(NAME TestContainers COMMAND TestContainers)
