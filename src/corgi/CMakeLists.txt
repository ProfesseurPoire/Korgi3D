cmake_minimum_required (VERSION 3.9.0)

#WARNING : this cmake file is for travis only
enable_testing()

add_subdirectory(test)
add_subdirectory(string)
add_subdirectory(math)
add_subdirectory(ecs)
add_subdirectory(logger)