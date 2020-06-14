project(TestECS)

add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cpp TestEntity.cpp )

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    corgi-ecs
    corgiTest)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
