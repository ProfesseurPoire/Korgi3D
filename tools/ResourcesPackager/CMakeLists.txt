cmake_minimum_required(VERSION 3.13.0)

project(ResourcesPackager LANGUAGES CXX VERSION 1.0.0)

add_library(ResourcesPackagerLib "")
add_executable(ResourcesPackagerExe "")

add_subdirectory(src)

set_property(TARGET ResourcesPackagerLib PROPERTY CXX_STANDARD 20)
set_property(TARGET ResourcesPackagerExe PROPERTY CXX_STANDARD 20)

#target_link_libraries(ResourcesPackagerLib PUBLIC CorgiEngine)
target_link_libraries(ResourcesPackagerExe PUBLIC ResourcesPackagerLib CorgiFilesystem)
target_include_directories(ResourcesPackagerLib PUBLIC src/)

add_custom_command(TARGET ResourcesPackagerExe POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/ext/SDL/lib/x86/SDL2.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:ResourcesPackagerExe>)                 # <--this is out-file path


add_custom_command(TARGET ResourcesPackagerExe POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/ext/SDLMixer/lib/x86/dependencies"
        $<TARGET_FILE_DIR:ResourcesPackagerExe>)


add_subdirectory(tests)