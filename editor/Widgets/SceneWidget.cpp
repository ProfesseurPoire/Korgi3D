
#include "SceneWidget.h"

#include <corgi/ecs/EntityManager.h>
#include <corgi/ecs/entity.h>

#include <corgi/components/Transform.h>
#include <corgi/components/Mesh.h>
#include <corgi/components/camera.h>

#include <corgi/String.h>
#include <corgi/CString.h>

#include <imgui.h>

#include <ext/imgui/IconsFontAwesome.h>

#include <iostream>

namespace Editor
{
    namespace Widgets
    {
        Scene::Scene(corgi::EntityPool& entityManager,
            SceneModel* model)
            :_entityManager(entityManager), Widget("Scene")
        {
            _model = model;

            // Probably will be done by Model itself 
            for (int entityId : entityManager._registered_entities)
            {
                EntityModel em;
                em.entity       = entityManager.get(entityId);
                em.isSelected   = false;

                for(std::type_index componentId : em.entity->_registered_components)
                {
                    ComponentModel cm(componentId);
                    cm.isSelected   = false;
                    em.components.add(cm);
                }

                _model->_registeredEntities.add(em);
            }
        };

       /* static std::vector<char*> ConvertString(std::vector<corgi::String>& strings)
        {
            std::vector<char*> result;
            for (auto& string : strings)
            {
                result.push_back(&string.first());
            }
            return result;
        }*/

        void Scene::entityNode(EntityModel& entityModel)
        {
            corgi::Entity* entity = entityModel.entity;

            if (!corgi::CString::text_match(entity->name.cstr(),_searchText))
                return;
            
            ImGuiTreeNodeFlags node_flags =
               
                ImGuiTreeNodeFlags_OpenOnArrow |
                ImGuiTreeNodeFlags_OpenOnDoubleClick;

            if (_model->_selectedEntity == &entityModel)
            {
                node_flags = 
                    ImGuiTreeNodeFlags_OpenOnArrow |
                    ImGuiTreeNodeFlags_OpenOnDoubleClick |
                    ImGuiTreeNodeFlags_Selected;
            }

            bool nodeOpen = ImGui::TreeNodeEx(
                corgi::String::from_integer(entity->id()).cstr(),
                node_flags,
                ("%s " + entity->name).cstr(), ICON_FA_USER);

                if (ImGui::IsItemClicked())
                {
                    _model->_selectedEntity      = &entityModel;
                    _model->_selectedComponent   = nullptr;
                }

                if (ImGui::IsItemClicked(1))
                {
                    _cancelRightClick = true;
                    ImGui::OpenPopup("GigaPopup");
                }

                if (ImGui::BeginPopup("GigaPopup"))
                {
                    if (ImGui::BeginMenu("Add"))
                    {
                        if (!entity->has_component<corgi::MeshRenderer>())
                        {
                            if (ImGui::MenuItem("Mesh"))
                            {
                                if (!entity->has_component<corgi::MeshRenderer>())
                                {
                                    entity->add_component<corgi::MeshRenderer>();
                                    entityModel.components.add(ComponentModel(typeid(corgi::MeshRenderer)));
                                }
                            }
                        }

                        if (!entity->has_component<corgi::Camera>())
                        {
                            if (ImGui::MenuItem("Camera"))
                            {
                                entity->add_component<corgi::Camera>();
                                entityModel.components.add(ComponentModel(typeid(corgi::Camera)));
                            }
                        }
                        ImGui::EndMenu();
                    }

                    ImGui::Separator();

                    if (ImGui::MenuItem("Delete"))
                    {
                        for (int i=0; i<_model->_registeredEntities.size(); ++i)
                        {
                            EntityModel& em = _model->_registeredEntities[i];
                            if (em.entity == entity)
                            {
                                _model->_registeredEntities.removeAt(i);
                                continue;
                            }
                        }
                        _entityManager.release(entity);
                    }
                    
                    ImGui::EndPopup();
                }

                    /*if (ImGui::IsItemClicked(1))
                    {
                        ImGui::OpenPopup("Scene Context Menu");
                    }*/

            if (nodeOpen)
            {
               // ImGui::OpenPopupOnItemClick("Scene Context Menu", 1);

                auto mesh = entity->get_component<corgi::MeshRenderer>();
                if (mesh.valid())
                {
                    ImGui::Indent();
                    
                    for (auto& c : entityModel.components)
                    {
                        if (c.component == typeid(corgi::MeshRenderer))
                        {
                            c.parentEntity = &entityModel;
                            bool selection = (_model->_selectedComponent == &c);

                            ImGui::Selectable(ICON_FA_CUBES" Mesh", &selection);
                            //ImGui::Text(ICON_FA_CUBES" Mesh", ICON_FA_CUBES);

                            if (selection)
                            {
                                _model->_selectedEntity = nullptr;
                                _model->_selectedComponent = &c;
                            }

                            if (ImGui::IsItemClicked(1))
                            {
                                ImGui::OpenPopup("UltraPopup");
                                _cancelRightClick = true;
                            }

                            float value;
                            if (ImGui::BeginPopup("UltraPopup"))
                            {
                                if (ImGui::Selectable("Add"))
                                    value = 0.0f;
                                ImGui::EndPopup();
                            }
                            ImGui::Unindent();
                        }
                    }
                }

                auto camera = entity->get_component<corgi::Camera>();

                if (camera.valid())
                {
                    for (auto& c : entityModel.components)
                    {
                        if (c.component == typeid(corgi::Camera))
                        {
                            c.parentEntity = &entityModel;
                            ImGui::Indent();

                            bool selection = (_model->_selectedComponent == &c);

                            ImGui::Selectable(ICON_FA_VIDEO_CAMERA" Camera", &selection);
                            //ImGui::Text(ICON_FA_CUBES" Mesh", ICON_FA_CUBES);

                            if (selection)
                            {
                                _model->_selectedEntity = nullptr;
                                _model->_selectedComponent = &c;

                            }

                            if (ImGui::IsItemClicked(1))
                            {
                                // ImGui::OpenPopup("Component Context Menu");
                            }
                            ImGui::Unindent();
                        }
                    }
                }
                ImGui::TreePop();
            }
        }

        void Scene::show()
        {
            _cancelRightClick = false;
            corgi::String str = ICON_FA_SEARCH;
            str += " Search";
           
            ImVec2 v = ImGui::GetWindowSize();

            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
            ImGui::InputTextWithHint("##SearchBox", str.cstr(), _searchText, 200);
            ImGui::PopStyleVar();

            ImVec2 cursorPosition   = ImGui::GetCursorPos();
            float widgetHeight      = ImGui::GetWindowHeight();
            float treeviewheight = widgetHeight - cursorPosition.y - 10;
            
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.16f, 0.29f, 0.48f, 0.54f));
            
            ImGui::BeginChild("Child2", ImVec2(0, treeviewheight), true);
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Menu"))
                {
                    //ShowExampleMenuFile();
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            for(EntityModel& em : _model->_registeredEntities)
                entityNode(em);

            float value;

        /*  if (ImGui::BeginPopupContextItem("Component Context Menu", 4))
        {
            if (ImGui::Selectable("Add"))
                value = 0.0f;
            ImGui::EndPopup();
        }*/

            ImGui::EndChild();

            if (ImGui::IsItemClicked(1))
            {
                if(!_cancelRightClick)
                    ImGui::OpenPopup("SuperPopup");
            }

            float vvalue;
            if (ImGui::BeginPopup("SuperPopup"))
            {
                if (ImGui::Selectable("New Entity"))
                {
                    addEntity();
                }
                ImGui::EndPopup();
            }

            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
        }

        void Scene::addEntity()
        {
            // So every entity has a transform by default

            corgi::Entity* e = &_entityManager.get();

            e->add_component<corgi::Transform>();
            e->name = "Entity";

            EntityModel em;

            em.entity = e;
            ComponentModel cm;
            cm.component = typeid(corgi::Transform);
            em.components.add(cm);

            _model->_registeredEntities.add(em);
            _model->_selectedComponent = nullptr;
            _model->_selectedEntity = &_model->_registeredEntities.last();
        }
    }
}


