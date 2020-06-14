#cmake_minimum_required(VERSION 3.8.0)
#
#configure_file(config.h.in config.h)
#
##add_executable(ResourceManager 
#    main.cpp
#    font.cpp
#    Application.cpp
#    Application.h
#    TileMapEditor.h
#    TileMapEditor.cpp
#    font.h
#    SceneModel.cpp
#    SceneModel.h
#
#    Dialogs/TextureSelectionDialog.cpp
#    Dialogs/TextureSelectionDialog.h
#    Dialogs/JSonSelectionDialog.h
#    Dialogs/JSonSelectionDialog.cpp
#
#    Models/SceneModel.cpp
#    Models/SceneModel.h
#
#    Widgets/TreeItem.h
#    Widgets/TreeItem.cpp
#    Widgets/GameWidget.cpp
#    Widgets/GameWidget.h
#    Widgets/ResourceWidget.h
#    Widgets/ResourceWidget.cpp
#    Widgets/TilesetCreationWidget.cpp
#    Widgets/TilesetCreationWidget.h
#    Widgets/Widget.cpp
#    Widgets/Widget.h
#    Widgets/SceneWidget.cpp
#    Widgets/SceneWidget.h
#    Widgets/PropertyWidget.cpp
#    Widgets/PropertyWidget.h
#    Widgets/Canvas.h
#    Widgets/Canvas.cpp
#
#    ext/imgui/imconfig.h
#    ext/imgui/imgui.cpp
#    ext/imgui/imgui.h
#    ext/imgui/imgui_demo.cpp
#    ext/imgui/imgui_draw.cpp
#    ext/imgui/imgui_impl_glfw.cpp
#    ext/imgui/imgui_impl_glfw.h
#    ext/imgui/imgui_impl_opengl2.cpp
#    ext/imgui/imgui_impl_opengl2.h
#    ext/imgui/imgui_internal.h
#    ext/imgui/imgui_widgets.cpp
#    ext/imgui/imstb_rectpack.h
#    ext/imgui/imstb_textedit.h
#    ext/imgui/imstb_truetype.h
#)
#
#target_include_directories(ResourceManager 
#PUBLIC
#    ${CMAKE_CURRENT_SOURCE_DIR}
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/
#    ${CMAKE_BINARY_DIR}/editor/
#    ext/freetype/include/
#    ext/imgui
#)
#
#set_property(TARGET ResourceManager PROPERTY CXX_STANDARD 20)
#
#target_link_libraries( ResourceManager
#    PUBLIC
#    corgi-filesystem
#    corgi-math
#    corgiEngine
#    corgiString
#    ${CMAKE_CURRENT_SOURCE_DIR}/ext/freetype/freetype240.lib
#)