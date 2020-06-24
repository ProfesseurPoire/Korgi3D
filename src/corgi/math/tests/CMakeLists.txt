cmake_minimum_required(VERSION 3.13.0) # need 3.13 for target_sources

project(TestMath CXX)

add_executable(${PROJECT_NAME} "")

target_sources(${PROJECT_NAME}
    PUBLIC 
    UnitTestMath.cpp
	UTMatrix.cpp
	UTVector2f.cpp
	UTVector3f.cpp
	UTVector4f.cpp
	UTCollisions.cpp)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    corgi-math
    corgiTest)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

add_test(NAME TestMath COMMAND TestMath)