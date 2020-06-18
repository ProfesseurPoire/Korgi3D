cmake_minimum_required (VERSION 3.9.0)

project(corgiTest VERSION 1.0.0)

add_library(corgiTest INTERFACE)

target_include_directories(corgiTest
INTERFACE 
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
$<INSTALL_INTERFACE:include>)

#########################################
# Tests
#########################################

option(BUILD_TESTS "Build the corgi test tests" ON)

if(BUILD_TESTS)
enable_testing()
add_subdirectory(test)
endif()