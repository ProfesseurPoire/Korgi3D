project(TestContainers)

add_executable(${PROJECT_NAME} 
    unit/FilledTree.cpp 
    unit/EmptyTree.cpp)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    corgi-containers
    corgiTest)

target_include_directories(TestContainers PUBLIC ./)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
