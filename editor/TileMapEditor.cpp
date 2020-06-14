#include "TileMapEditor.h"

#include <corgi/rendering/texture.h>
#include "Dialogs/TextureSelectionDialog.h"

#include "ext/imgui/imgui.h"

namespace Editor
{
    TileMapEditor::TileMapEditor(TextureSelectionDialog& dialog)
        : textureSelectionDialog(dialog)
    {

    }
    void TileMapEditor::Show()
    {
        if (!showed) 
            return;

        ImGui::Begin("Tilemap", &showed);

/*
        if (ImGui::Button("Select Texture"))
        {
            textureSelectionDialog.open();
        }
*/
        //if (textureSelectionDialog.isOpened())
        //{
        //    textureSelectionDialog.show();

        //    if (textureSelectionDialog.getSelectedTexture()!=nullptr)
        //    {
        //        _tileset.texture = textureSelectionDialog.getSelectedTexture();
        //    }
        //}

        //if (_tileset.texture != nullptr)
        //{
        //    ImGui::LabelText("Tileset", _tileset.texture->name);
        //    ImGui::InputInt2("Size", &_tileset.width);


        //    float textureRatio = _tileset.texture->texture->ratio();
        //                        
        //    float textureHeight = 100.0f;
        //    float textureWidth  = textureRatio* 100.0f;

        //    static float zoomValue = 1.0f;

        //    ImGui::SliderFloat("TilesetZoom", &zoomValue, 0, 10);
        //    
        //    ImGui::Image((void*)_tileset.texture->texture->id(), ImVec2(textureWidth*zoomValue, textureHeight*zoomValue), ImVec2(0, 0), ImVec2(1, -1));
        //    ImVec2 offset = ImGui::GetCursorScreenPos();
        //    offset.y -= 5;

        //    ImVec4 imageRect;
        //    imageRect.x = offset.x;
        //    imageRect.y = offset.y;
        //    imageRect.z = textureWidth;
        //    imageRect.w = textureHeight;

        //    // Vertical lines
        //    for (float i = 0.0f; i <= _tileset.texture->texture->width(); i+= _tileset.width)
        //    {
        //        float ratioWidth = i / _tileset.texture->texture->width();
        //        ImGui::GetWindowDrawList()->AddLine(
        //            ImVec2(offset.x + ratioWidth*textureWidth*zoomValue, offset.y + 0.0f),
        //            ImVec2(offset.x + ratioWidth*textureWidth*zoomValue, offset.y - textureHeight* zoomValue),
        //            ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.0f, 0.0f, 0.5f)));
        //    }

        //    // Horizontal Lines
        //    for (float i = 0.0f; i <= _tileset.texture->texture->height(); i += _tileset.height)
        //    {
        //        float ratioHeight = i / _tileset.texture->texture->height();

        //        ImGui::GetWindowDrawList()->AddLine(
        //            ImVec2(offset.x ,                           offset.y - ratioHeight*textureHeight*zoomValue),
        //            ImVec2(offset.x +textureWidth*zoomValue,    offset.y - ratioHeight * textureHeight*zoomValue),
        //            ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.0f, 0.0f, 0.5f)));
        //    }

        //    if (ImGui::IsMouseClicked(0,true))
        //    {
        //        ImVec2 mousePosition = ImGui::GetMousePos();

        //        float x = mousePosition.x - imageRect.x;
        //        float y = mousePosition.y - imageRect.y;

        //        x = x /( textureWidth * zoomValue);
        //        y = y /( textureHeight * zoomValue);

        //        _selectedGridX  = x * _tileset.texture->texture->width()  / _tileset.width;
        //        _selectedGridY  = y * _tileset.texture->texture->height() / _tileset.height;

        //        int totalX = _tileset.texture->texture->width() / _tileset.width;
        //        int totalY = _tileset.texture->texture->height() / _tileset.height;

        //        if ((_selectedGridX >= 0 && _selectedGridX < totalX)
        //            && (_selectedGridY <= 0 && _selectedGridY > -totalY))
        //        {
        //            _tileSelected = true;
        //        }
        //        else
        //        {
        //            _tileSelected = false;
        //        }
        //    }

        //    if (_tileSelected)
        //    {
        //        int totalX = _tileset.texture->texture->width() / _tileset.width;
        //        int totalY = _tileset.texture->texture->height() / _tileset.height;

        //        ImGui::GetWindowDrawList()->AddQuadFilled(
        //            ImVec2(imageRect.x + (float)_selectedGridX / (float)totalX * textureWidth*zoomValue, imageRect.y + (float)_selectedGridY / (float)totalY * textureHeight*zoomValue),
        //            ImVec2(imageRect.x + (float)(_selectedGridX + 1) / (float)totalX * textureWidth*zoomValue, imageRect.y + (float)_selectedGridY / (float)totalY * textureHeight*zoomValue),
        //            ImVec2(imageRect.x + (float)(_selectedGridX + 1) / (float)totalX * textureWidth*zoomValue, imageRect.y + (float)(_selectedGridY - 1) / (float)totalY * textureHeight*zoomValue),
        //            ImVec2(imageRect.x + (float)_selectedGridX / (float)totalX * textureWidth*zoomValue, imageRect.y + (float)(_selectedGridY - 1) / (float)totalY * textureHeight*zoomValue),
        //            ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 1.0f, 0.0f, 0.50f)));
        //    }
        //}
        ImGui::End();
    }
}