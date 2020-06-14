project(TestMultithreading)

add_executable(${PROJECT_NAME} TestMultithreading.cpp)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    corgiEngine
    corgiTest)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
