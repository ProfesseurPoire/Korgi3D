cmake_minimum_required(VERSION 3.8.0)

project(TestRun)

configure_file(config.h.in config.h)

add_executable(${PROJECT_NAME} TestRun.cpp)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W3 )
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic )
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_link_libraries(${PROJECT_NAME} PUBLIC 
corgiEngine
corgiTest)

target_include_directories(${PROJECT_NAME} 
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/tests/AutomatedTests/TestRun
)

add_test( NAME TestRun-test COMMAND TestRun)