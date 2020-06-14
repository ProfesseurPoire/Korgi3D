cmake_minimum_required(VERSION 3.9.0)

project(UnitTestString)

add_executable(${PROJECT_NAME}
    main.cpp
    UTString.cpp
    UTCString.cpp
)

target_link_libraries(${PROJECT_NAME}
    corgiTest
    corgiString
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

add_test( NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})