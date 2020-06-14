
add_executable(FontExporter FontExporter.cpp )

set_property(TARGET FontExporter PROPERTY CXX_STANDARD 20)

target_include_directories( FontExporter
	PUBLIC
	ext/freetype/include
)

target_link_libraries( FontExporter
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/ext/freetype/freetype240.lib
)



add_subdirectory(test)