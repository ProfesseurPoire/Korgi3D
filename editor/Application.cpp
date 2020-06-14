#include "Application.h"

#include <iostream>
#include <windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <corgi/math/Vector2f.h>
#include <corgi/inputs/Window.h>

#include <corgi/rendering/renderer.h>
#include <corgi/rendering/FrameBuffer.h>
#include <corgi/rendering/texture.h>

#include <corgi/components/Mesh.h>
#include <corgi/components/Transform.h>
#include <corgi/components/Camera.h>
#include <corgi/components/Panel.h>
#include <corgi/components/AABBCollider.h>
#include <corgi/components/Animator.h>
#include <corgi/components/StateMachine.h>
#include <corgi/components/Sprite.h>
#include <corgi/components/Text.h>

#include <corgi/ecs/SystemManager.h>
#include <corgi/ecs/World.h>

#include <corgi/systems/TransformSystem.h>
#include <corgi/systems/CameraSystem.h>


#include "ext/imgui/imgui.h"
#include "ext/imgui/imgui_impl_glfw.h"
#include "ext/imgui/imgui_impl_opengl2.h"

#include <corgi/utils/ResourceManager.h>

#include <corgi/ecs/EntityManager.h>
#include <corgi/ecs/entity.h>
#include <corgi/inputs/Window.h>

#include "config.h"

#include "TileMapEditor.h"


#include "Dialogs/TextureSelectionDialog.h"
#include "Dialogs/JSonSelectionDialog.h"

#include "Widgets/GameWidget.h"
#include "Widgets/ResourceWidget.h"
#include "Widgets/TilesetCreationWidget.h"
#include "Widgets/SceneWidget.h"
#include "Widgets/PropertyWidget.h"

#include <corgi/containers/Map.h>
#include "IconsFontAwesome.h"

using namespace corgi;

namespace Editor
{
    void Application::Save()
    {
        FILE* configurationFile;
        configurationFile = fopen("config", "wb");  // r for read, b for binary
        fwrite(&windowWidth, sizeof(int), 1, configurationFile); // read 10 bytes to our buffer
        fwrite(&windowHeight, sizeof(int), 1, configurationFile);
        fwrite(&windowPositionX, sizeof(int), 1, configurationFile);
        fwrite(&windowPositionY, sizeof(int), 1, configurationFile);
        fclose(configurationFile);
    }

    void Application::Load()
    {
        FILE* configurationFile;
        configurationFile = fopen("config", "rb");  // r for read, b for binary

        if (configurationFile != nullptr)
        {
            fread(&windowWidth, sizeof(int), 1, configurationFile); // read 10 bytes to our buffer
            fread(&windowHeight, sizeof(int), 1, configurationFile);
            fread(&windowPositionX, sizeof(int), 1, configurationFile);
            fread(&windowPositionY, sizeof(int), 1, configurationFile);
            fclose(configurationFile);
        }
        else    // Default configuration
        {
            windowWidth = 1000;
            windowHeight= 800;
            windowPositionX = 100;
            windowPositionY = 100;
        }
    }

    void Application::import_menu()
    {
        if (ImGui::BeginMenu("Import"))
        {
            if (ImGui::MenuItem("Import Tiled Tilemap"))
            {
                _json_selection_dialog->open();

                /*_json_selection_dialog->callback=
                [](const std::string& selection)
                {
                    TiledImporter imp(selection);
                    imp.process();
                };*/
            }

            
            ImGui::EndMenu();
        }
        
    }

    void Application::main_menu()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ImGui::MenuItem("hi");

                if (ImGui::MenuItem("New Entity"))
                {
                    //entityManager.Pool();
                }
                ImGui::EndMenu();
            }

            // Widget is too "generic" though
            if (ImGui::BeginMenu("Widget"))
            {
                if (ImGui::MenuItem(_widgets["GameWidget"]->title(), "", _widgets["GameWidget"]->_isOpen))
                {
                    _widgets["GameWidget"]->_isOpen = !_widgets["GameWidget"]->_isOpen;
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("New"))
            {
                if (ImGui::MenuItem("New Tileset", "CTRL+T"))
                {
                    // New Tileset
                    Tileset ts;
                    ts.name = "new tileset";
                    resource_manager().tilesets.add(ts);

                    auto * widget =
                        dynamic_cast<TilesetCreationWidget*>(_widgets["TilesetCreationWidget"]);

                    widget->isOpen(true);
                    widget->setTileset(&resource_manager().tilesets.last());
                }
                ImGui::EndMenu();
            }

            import_menu();

            

            if (ImGui::BeginMenu("Tools"))
            {
               /* if (ImGui::MenuItem("Tilemap", "", tilemapEditor.showed))
                {
                    tilemapEditor.showed = !tilemapEditor.showed;
                }*/
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    Application::Application(){}
    Application::~Application(){}

    ResourceManager& Application::resource_manager()
    {
        return *_resource_manager;
    }

    void Application::run()
    {
        Load();

        _window = new Window(
			"t", 
			windowPositionX, 
			windowPositionY, 
			windowWidth,
			windowHeight,
            -1,
			true
        );

        // is that for Imgui or my own stuff?
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;

        io.Fonts->AddFontDefault();
        ImFontConfig config;
        config.MergeMode = true;
        config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
        static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        io.Fonts->AddFontFromFileTTF("C:\\dev\\corgiEngine\\editor\\ext\\imgui\\fontawesome-webfont.ttf", 13.0f, &config, icon_ranges);

        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
    
        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)_window->underlying_window(), true);
        ImGui_ImplOpenGL2_Init();

        //bool truestuff = true;

        //ImGui::Begin("BigThing", &truestuff, ImVec2(800, 800));
        //ImGui::End();
        // Initializing the components here
        // Maybe this should be done directly inside World? I don't know


        // I should probably remove that part from the application thing?
        // dunno
        World world(1000);

        world.add_component_pool<Transform>();
        world.add_component_pool<Text>();
        world.add_component_pool<StateMachine>();
        world.add_component_pool<SpriteRenderer>();
        world.add_component_pool<Panel>();
        world.add_component_pool<MeshRenderer>();
        world.add_component_pool<Camera>();
        world.add_component_pool<Animator>();
        world.add_component_pool<BoxCollider>();

        world.add_system<TransformSystem>();
        world.add_system<CameraSystem>();

        Entity* camera = &world.entity_pool().get();
        camera->name = "Camera";

        camera->add_component<Transform>();
        camera->add_component<Camera>();

        camera->get_component<Camera>()->viewport(0, 0, _window->width(), _window->height());
        camera->get_component<Camera>()->ortho( 1.0f, 2.0f, -1.0f, 100);
        camera->get_component<Camera>()->clearColor(0.6f, 0.6f, 0.6f, 1.0f);

        // Creating a mesh to be displayed in game 

        Entity* e = &world.entity_pool().get();
        e->add_component<Transform>();

        e->add_component<MeshRenderer>();
        auto mesh = e->get_component<MeshRenderer>();

        mesh->type = MeshRenderer::PrimitiveType::Triangles;

        mesh->AddVertex()
            .Position(-0.5f, 0.0f, 0.0f)
            .Color(1.0f, 0.0f, 0.0f);

        mesh->AddVertex()
            .Position(0.5f, 0.0f, 0.0f)
            .Color(0.0f, 1.0f, 0.0f);

        mesh->AddVertex()
            .Position(0.0f, 0.5f, 0.0f)
            .Color(0.0f, 0.0f, 1.0f);

        Renderer& renderer = _window->renderer();
        _window->renderer().ClearColor(1.0f, 0.0f, 0.0f);

        // Widget Creation

        _resource_manager           = std::make_unique<ResourceManager>
            (renderer, RESOURCE_DIRECTORY);
        _texture_selection_dialog   = std::make_unique<TextureSelectionDialog>
            (resource_manager());
        _json_selection_dialog = std::make_unique<JsonSelectionDialog>(
            resource_manager());

        auto frameBuffer = renderer.generateFrameBuffer(800, 600);

        TileMapEditor tilemapEditor(*_texture_selection_dialog);

        // Initialize Widgets

        SceneModel * sceneModel = new SceneModel();

        addWidget(new TilesetCreationWidget());
        addWidget(new Widgets::Resource(resource_manager()));
        addWidget(new Widgets::Scene(world.entity_pool(), sceneModel));
        addWidget(new Widgets::Property(world.entity_pool(), textureSelectionDialog(),sceneModel));
        addWidget(new Widgets::Game(frameBuffer, camera));

        Widgets::Game* gameWidget = dynamic_cast<Widgets::Game*>(_widgets["GameWidget"]);

        while (!_window->is_closing())
        {
            // Update the framebuffer if the widget has been resized
            if (gameWidget->updateFrameBuffer())
            {
                renderer.resizeFrameBuffer(frameBuffer, gameWidget->width(),
                    gameWidget->height());
            }

            // We draw the world into the framebuffer
            renderer.draw(world, *frameBuffer);

            _window->update_events();

            // Start drawing the editor's stuff

            renderer.Clear();

            ImGui_ImplOpenGL2_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            static bool truestuff = true;
            static bool opt_fullscreen_persistant = true;
            bool opt_fullscreen = opt_fullscreen_persistant;
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
            // because it would be confusing to have two docking targets within each others.
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            if (opt_fullscreen)
            {
                ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->Pos);
                ImGui::SetNextWindowSize(viewport->Size);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            }

            // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;

			
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockSpace Demo", &truestuff, window_flags);
            ImGui::PopStyleVar();

            if (opt_fullscreen)
                ImGui::PopStyleVar(2);

            // DockSpace
            ImGuiIO& io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            }

            ImGui::End();

            bool t = true;
            ImGui::ShowDemoWindow(&t);
            //ImGui::ShowMetricsWindow();
            //ImGui::ShowUserGuide();

            for (auto& pair : _widgets)
                pair.second->exec();

            main_menu();


            std::vector<String> entities;
            for (auto& entity : world.entity_pool()._registered_entities)
            {
                entities.push_back(world.entity_pool().get(entity)->name);
            }

            tilemapEditor.Show();
            _json_selection_dialog->show();

            world.update();

            ImGui::Render();
            ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }
            _window->swap_buffers();
        }

        // Cleanup

        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        windowWidth     = _window->width();
        windowHeight    = _window->height();

        windowPositionX = _window->x();
        windowPositionY = _window->y();

        Save();

        //editorRenderer.windowDrawList().AddRectangle(100, 100, 100, 100, { 1.0f,0.0f,0.0f,1.0f });

        //if (argc < 2)
        //{
        //	return 0;
        //}

        //// First argument will be the Resource Folder
        //std::string resourceFolder = argv[1];
        //std::string outputFolder	= argv[2];

        //std::vector<std::string> files;

        //corgi::filesystem::list_files(files, resourceFolder.c_str(),false);

        //for (auto file : files)
        //{
        //	if (corgi::filesystem::extension(file)=="png")
        //	{
        //		WriteImage(resourceFolder + "/" + file, outputFolder);
        //	}
        //	else if (corgi::filesystem::extension(file) == "ttf")
        //	{
        //		WriteFont(resourceFolder + "/" + file, outputFolder);
        //	}
        //}
    }

    void Application::addWidget(Widgets::Widget* widget)
    {
        std::string str(widget->title());

        widget->_application = this;
        widget->init();
        _widgets.emplace(str, widget);
    }

    TextureSelectionDialog& Application::textureSelectionDialog()
    {
        return *_texture_selection_dialog;
    }

    corgi::Renderer& Application::renderer()
    {
        return _window->renderer();
    }
}