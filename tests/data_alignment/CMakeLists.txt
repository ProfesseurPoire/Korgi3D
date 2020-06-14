add_executable(DataAlignment DataAlignment.cpp)

target_link_libraries( DataAlignment 
    PUBLIC
    corgiEngine
)

set_property(TARGET DataAlignment PROPERTY CXX_STANDARD 20)
