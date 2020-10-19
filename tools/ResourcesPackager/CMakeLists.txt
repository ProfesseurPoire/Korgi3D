

add_executable(ResourcesPackager ResourcesPackager.cpp )

set_property(TARGET ResourcesPackager PROPERTY CXX_STANDARD 20)

target_link_libraries( ResourcesPackager corgiEngine)

add_custom_command(TARGET ResourcesPackager POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/ext/SDL/lib/x86/SDL2.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:ResourcesPackager>)                 # <--this is out-file path


add_custom_command(TARGET ResourcesPackager POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/ext/SDLMixer/lib/x86/dependencies"
        $<TARGET_FILE_DIR:ResourcesPackager>)