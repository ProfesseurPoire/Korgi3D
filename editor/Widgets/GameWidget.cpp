#include "GameWidget.h"

#include <corgi/rendering/FrameBuffer.h>

#include <corgi/components/Camera.h>
#include <corgi/components/Transform.h>

#include <corgi/ecs/entity.h>

#include "../ext/imgui/imgui.h"
#include "../ext/imgui/imgui_impl_glfw.h"
#include "../ext/imgui/imgui_impl_opengl2.h"

#include <iostream>

namespace Editor
{
    namespace Widgets
    {
        Game::Game( 
            corgi::FrameBuffer* frameBuffer,
            corgi::Entity* cameraEntity)
            : _frameBuffer(frameBuffer), 
            _camera(cameraEntity->get_component<corgi::Camera>().operator->()), 
            _cameraEntity(cameraEntity),Widget("GameWidget")
        {
            setBorderSize(2.0f);
            setPadding(0.0f, 0.0f);
            setNoScrollbar(true);
        }

        int Game::width()const
        {
            return _width;
        }

        int Game::height()const
        {
            return _height;
        }

        bool Game::updateFrameBuffer()const
        {
            return _updateFrameBuffer;
        }

        static bool _isDragging = false;
        static ImVec2 _dragPosition;
        static corgi::math::Vector3f _dragCameraPosition;

        void Game::show()
        {
            ImGuiIO& io = ImGui::GetIO();
            
            _camera->ratio((float)width() / (float)height());

            if (io.MouseWheel !=0.0f)
            {
                _camera->orthographicHeight
                (
                    _camera->orthographicHeight() + io.MouseWheel*0.1f
                );
            }

            if (io.MouseDown[2])
            {
                auto transform = _cameraEntity->get_component<corgi::Transform>();

                if (_isDragging == false)
                {
                    _dragPosition = io.MousePos;
                    _dragCameraPosition = transform->position();
                    _isDragging = true;
                }
                else
                {
                    float offsetX = _dragPosition.x - io.MousePos.x;
                    float offsetY = _dragPosition.y - io.MousePos.y;

                    offsetX /= (float)width();
                    offsetY /= (float)height();

                    // just making sure the movement matches
                    // what the user sees
                    offsetX *= 2.0f * _camera->ratio() * _camera->orthographicHeight();
                    offsetY *= 2.0f * _camera->orthographicHeight();

                    transform->position
                    (
                        _dragCameraPosition.x + offsetX, 
                        _dragCameraPosition.y - offsetY,
                        0.0f
                    );
                }
            }
            else
                _isDragging = false;

            float windowWidth   = ImGui::GetContentRegionAvail().x;
            float windowHeight  = ImGui::GetContentRegionAvail().y;

            _updateFrameBuffer = (_width != windowWidth || _height != windowHeight);

            _width = static_cast<int>(windowWidth);
            _height = static_cast<int>(windowHeight);

            _camera->viewport().width   = static_cast<int>(windowWidth);
            _camera->viewport().height  = static_cast<int>(windowHeight);

            ImGui::Image((void*)_frameBuffer->colorAttachment(), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
        }
    }
}