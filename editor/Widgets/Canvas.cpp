#include "Canvas.h"

#include <corgi/resources/image.h>

#include <corgi/rendering/texture.h>
#include <corgi/rendering/renderer.h>
#include <corgi/rendering/FrameBuffer.h>

#include <ext/imgui/imgui.h>

namespace Editor
{
    Canvas::Canvas(corgi::Renderer& renderer, int width, int height)
        : _renderer(renderer), _width(width), _height(height)
    {
        _frameBuffer = _renderer.generateFrameBuffer(_width, _height);
    }

    Canvas::~Canvas()
    {
        _renderer.deleteFrameBuffer(_frameBuffer);
        _renderer.deleteTexture(_backgroundTexture);
    }

    void Canvas::setTexture(corgi::Texture* texture)
    {
        _texture = texture;
        makeBackgroundTexture();
    }

    void Canvas::show()
    {
        _zoomValuePercentage += ImGui::GetIO().MouseWheel * 10;

        ImGui::InputFloat("Zoom", &_zoomValuePercentage);

        float zoomCoef = _zoomValuePercentage / 100.0f;
        float maxOffsetWidth;
        float maxOffsetHeight;

        if (_texture->width()*zoomCoef > _width)
        {
            maxOffsetWidth = (_texture->width()*zoomCoef - _width) / 2.0f;
            maxOffsetWidth += _width / 2.0f;
        }
        else
        {
            maxOffsetWidth = _width / 2.0f;
        }

        if (_texture->height()*zoomCoef > _height)
        {
            maxOffsetHeight = (_texture->height()*zoomCoef - _height) / 2.0f;
            maxOffsetHeight += _height / 2.0f;
        }
        else
        {
            maxOffsetHeight = _height / 2.0f;
        }

        float rectWidth = ((float)_width / (_width + (float)maxOffsetWidth))*_width;
        float rectHeight = ((float)_height / (_height + (float)maxOffsetHeight))*_height;

        static bool startDrag = false;
        static ImVec2 startDragPoint;
        static float last_offsetX;
        static float last_offsetY;

        if (ImGui::GetIO().MouseDown[2])
        {
            if (startDrag == false)
            {
                startDragPoint = ImGui::GetMousePos();
                startDrag = true;
                last_offsetX = _offsetX;
                last_offsetY = _offsetY;
            }
        }
        else
        {
            startDrag = false;
        }

        ImVec2 currentMousePos = ImGui::GetMousePos();

        if (startDrag == true)
        {
            _offsetX = last_offsetX + currentMousePos.x - startDragPoint.x;
            _offsetY = last_offsetY + currentMousePos.y - startDragPoint.y;
        }

        if (_offsetX > maxOffsetWidth)
        {
            _offsetX = maxOffsetWidth;
        }

        if (_offsetX < -maxOffsetWidth)
        {
            _offsetX = -maxOffsetWidth;
        }

        if (_offsetY > maxOffsetHeight)
        {
            _offsetY = maxOffsetHeight;
        }

        if (_offsetY < -maxOffsetHeight)
        {
            _offsetY = -maxOffsetHeight;
        }

        _renderer.beginFrameBuffer(_frameBuffer);
        _renderer.setBlend();
        _renderer.ClearColor(0.4f, 0.5f, 0.4f, 1.0f);
        _renderer.Clear();
        _renderer.setViewport(0, 0, _width, _height);
        _renderer.setProjectionMatrix(1, _height, 0.0f, 10.0f);
        _renderer.drawSprite(_offsetX, -_offsetY, _texture->width()*zoomCoef,
            _texture->height()*zoomCoef, _backgroundTexture);

        _renderer.drawSprite(_offsetX, -_offsetY, _texture->width()*zoomCoef,
            _texture->height()*zoomCoef, _texture);

        _renderer.endFrameBuffer();

        ImGui::NewLine();
        auto cursor = ImGui::GetCursorScreenPos();

        ImGui::Image((void*)_frameBuffer->colorAttachment(), ImVec2(_width, _height), ImVec2(0, 0), ImVec2(1, -1));

        float valx = (float)_width / 2.0f - rectWidth / 2.0f;
        float valx2 = (float)_width / 2.0f + rectWidth / 2.0f;


        ImGui::GetWindowDrawList()->AddRectFilled
        (
            ImVec2(cursor.x + _offsetX / maxOffsetWidth * ((float)_width - rectWidth) / 2.0f + valx, 2 + cursor.y + _height),
            ImVec2(cursor.x + _offsetX / maxOffsetWidth * ((float)_width - rectWidth) / 2.0f + valx2, 2 + cursor.y + 15.0f + _height),

            ImGui::ColorConvertFloat4ToU32(ImVec4(0.70f, 0.70f, 0.70f, 1.0f))
        );


        ImGui::GetWindowDrawList()->AddRectFilled
        (
            ImVec2(cursor.x + _width + 2,
                cursor.y + _offsetY / maxOffsetHeight * ((float)_height - rectHeight) / 2.0f + (float)_height / 2.0f - rectHeight / 2.0f),


            ImVec2(cursor.x + _width + 2 + 15.0f,
                cursor.y + _offsetY / maxOffsetHeight * ((float)_height - rectHeight) / 2.0f + (float)_height / 2.0f + rectHeight / 2.0f),

            ImGui::ColorConvertFloat4ToU32(ImVec4(0.70f, 0.70f, 0.70f, 1.0f))
        );


    }

    void Canvas::makeBackgroundTexture()
    {
        const int greyValue     = 160;
        const int whiteValue    = 200;

        corgi::Image image(_texture->width(),_texture->height(),4);

        for (int i = 0; i < image.height(); ++i)
        {
            for (int j = 0; j < image.width(); j++)
            {
                int val = i / 16 % 2;

                if (val == 0)
                {
                    if (j / 16 % 2)
                    {
                        image.pixel(whiteValue, whiteValue, whiteValue, 255, j, i);
                    }
                    else
                    {
                        image.pixel(greyValue, greyValue, greyValue, 255, j, i);
                    }
                }
                else
                {
                    if (j / 16 % 2)
                    {
                        image.pixel(greyValue, greyValue, greyValue, 255, j, i);
                    }
                    else
                    {
                        image.pixel(whiteValue, whiteValue, whiteValue, 255, j, i);
                    }
                }
            }
        }

        if (_backgroundTexture == nullptr)
        {
            _backgroundTexture = _renderer.generateTextureFromImage(image);
        }
    }
}