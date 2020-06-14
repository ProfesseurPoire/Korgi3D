#include "PropertyWidget.h"

#include <Dialogs/TextureSelectionDialog.h>

#include <corgi/ecs/entity.h>
#include <corgi/ecs/EntityManager.h>

#include <corgi/components/transform.h>
#include <corgi/components/camera.h>
#include <corgi/components/Mesh.h>

#include <corgi/String.h>

#include <ext/imgui/imgui.h>

#include <ext/imgui/IconsFontAwesome.h>

using namespace corgi;

namespace Editor
{
    namespace Widgets
    {
        Property::Property(corgi::EntityPool& em,
            TextureSelectionDialog& dialog,
            SceneModel* model)
                :   _textureSelectionDialog(dialog),
                    _entityManager(em),
                    _model(model),
                Widget("Properties"){}

        void Property::show()
        {
            Entity* selectedEntity = nullptr;

            if (_model->_selectedEntity != nullptr)
            {
                selectedEntity = _model->_selectedEntity->entity;
            }

            if (selectedEntity != nullptr)
            {
                if (ImGui::CollapsingHeader(ICON_FA_USER" Entity"), ImGuiTreeNodeFlags_DefaultOpen)
                {
                    ImGui::Indent();
                    selectedEntity->name.resize(200);
                    ImGui::InputText("Name", &selectedEntity->name.first(), 200);
                    ImGui::Unindent();
                }

                for (int i = 0; i < selectedEntity->_registered_components.size(); ++i)
                {
                    std::type_index componentIndex = selectedEntity->_registered_components[i];

                    if (componentIndex == typeid(Transform))
                    {

                        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
                        {
                            ImGui::Indent();

                            auto transform = selectedEntity->get_component<Transform>();

                            math::Vector3f p = transform->position();
                            math::Vector3f rot = transform->euler_angles();

                            ImGui::InputFloat3("Position", p.array());
                            ImGui::InputFloat3("Rotation", rot.array());

                            transform->position(p);
                            transform->euler_angles(rot);
                            ImGui::Unindent();

                        }
                    }
                }
            }

            if (_model->_selectedComponent != nullptr)
            {
                selectedEntity = _model->_selectedComponent->parentEntity->entity;

                if ( _model->_selectedComponent->component== typeid(Camera))
                {
                    if (ImGui::CollapsingHeader(ICON_FA_VIDEO_CAMERA" Camera", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        ImGui::Indent();
                        auto cam = selectedEntity->get_component<Camera>();
                        ImGui::InputInt4("Viewport", &cam->viewport().x);
                        ImGui::ColorEdit4("Background Color", &cam->clearColor().r);

                        bool isOrtho = cam->isOrthographic();

                        ImGui::Separator();
                        ImGui::Checkbox("Ortho", &isOrtho);

                        static float height = cam->orthographicHeight();
                        static float ratio  = cam->ratio();

                        float znear = cam->znear();
                        float zfar  = cam->zfar();

                        ImGui::InputFloat("Height", &height);
                        ImGui::InputFloat("Ratio",  &ratio);

                        ImGui::InputFloat("Near",   &znear);
                        ImGui::InputFloat("Far",    &zfar);

                        cam->ortho(height, ratio, znear, zfar);
                        
                        ImGui::Unindent();
                    }
                }
                   
                if (_model->_selectedComponent->component== typeid(MeshRenderer))
                {
                    showMesh(*selectedEntity->get_component<corgi::MeshRenderer>().operator->());
                }
            }
            _textureSelectionDialog.show();
        }

        void Property::showMesh(corgi::MeshRenderer& mesh)
        {
            if (ImGui::CollapsingHeader(ICON_FA_CUBES" Mesh", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();

                for (int i = 0; i < mesh.vertices.size(); ++i)
                {
                    corgi::String str = "##Vertex";
                    str += corgi::String::from_integer(i);
                    ImGui::InputFloat4(str.cstr(), &mesh.vertices[i].x);
                }

                ImGui::Unindent();
            }
        }
    }
}