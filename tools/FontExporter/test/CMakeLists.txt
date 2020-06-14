add_executable(FontExporterTest FontExporterTest.cpp ../FontLoader.cpp)

configure_file(config.h.in config.h)

target_include_directories(FontExporterTest PUBLIC ../ext/freetype/include ../

${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries( FontExporterTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../ext/freetype/freetype240.lib

corgiTest)