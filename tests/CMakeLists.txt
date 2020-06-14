

#add_executable(PerformanceTests main.cpp)


#add_executable(WindowDrawListTest WindowDrawListTest.cpp)
#add_executable(UnitTests UnitTests.cpp)

#target_link_libraries( WindowDrawListTest 
#    PUBLIC
#    corgiEngine
#)

#target_link_libraries(PerformanceTests PUBLIC corgiTest)

#set_property(TARGET PerformanceTests PROPERTY CXX_STANDARD 20)
#set_property(TARGET WindowDrawListTest PROPERTY CXX_STANDARD 20)
#set_property(TARGET UnitTests PROPERTY CXX_STANDARD 20)

#add_subdirectory(data_alignment)
add_subdirectory(multithreading)
add_subdirectory(transform_system)
add_subdirectory(ecs)
#add_subdirectory(AutomatedTests)