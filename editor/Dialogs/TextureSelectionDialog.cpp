#include "TextureSelectionDialog.h"

#include <corgi/rendering/texture.h>
#include "../ext/imgui/imgui.h"

#include <corgi/String.h>
#include <corgi/CString.h>

using namespace corgi;

namespace Editor
{
    TextureSelectionDialog::TextureSelectionDialog(ResourceManager& manager)
        :_resourceManager(manager){}

    void TextureSelectionDialog::open()
    {
        ImGui::OpenPopup("TextureSelectionDialog");
        _opened= true;
    }

    corgi::TextureHandle* TextureSelectionDialog::getSelectedTexture()const
    {
        return _selectedTexture;
    }

    bool TextureSelectionDialog::isOpened()const
    {
        return _opened;
    }

    void TextureSelectionDialog::show()
    {
        ImGui::SetNextWindowPos( ImVec2( 200  , 200));
        if (ImGui::BeginPopup("TextureSelectionDialog"))
        {
            ImVec2 cursorPosition = ImGui::GetCursorScreenPos();

            static char searchText[200];
            ImGui::InputText("Search", searchText,200);
            ImGui::SameLine();
            
            static float zoomValue = 1.0f;
            ImGui::SliderFloat("Zoom", &zoomValue, 0.0f, 3.0f);

            int line	= 0;
            int column	= -1;

            float	columnWidth = 100.0f*zoomValue + 20.0f;
            int		leftPadding = 10;

            // Ok so I probably need to make that a function somehow

            for (int i = 0; i < _resourceManager.textures.size(); ++i)
            {
                if (CString::text_match(searchText, _resourceManager.textures[i].name.cstr()))
                {
                    // We change the way the things are displayed
                    if (zoomValue < 0.1f)
                    {
                        if (ImGui::Button(_resourceManager.textures[i].name.cstr()))
                        {
                            _selectedTexture = &_resourceManager.textures[i];
                            ImGui::CloseCurrentPopup();
                            _opened = false;
                        }
                    }
                    else
                    {
                        float textureWidth  = zoomValue*100.0f;
                        float ratio         = (float)_resourceManager.textures[i].texture->height()/(float) _resourceManager.textures[i].texture->width();
                        float height        = textureWidth * ratio;
                        
                        column++;
                        
                        if ((cursorPosition.x + column * textureWidth) > 1200)
                        {
                            // NewLine
                            line++;
                            column = 0;
                        }

                        float squish = 1.0f;
                        if (height > textureWidth)
                        {
                            squish = textureWidth / height;
                        }

                        ImGui::BeginGroup();

                        ImGui::SetCursorPos
                        (
                            ImVec2
                            (
                                leftPadding+column*columnWidth,
                                line* ((textureWidth)+40) + 40 
                            )
                        );

                        float buttonPadding = 16;

                        float offsetX = leftPadding+buttonPadding/2.0f+ column * columnWidth + ( textureWidth - squish * textureWidth) / 2.0f;
                        float offsetY = line* (textureWidth+40) +buttonPadding / 2.0f + 40.0f + (textureWidth - (squish * textureWidth * ratio)) / 2.0f;
                        ImGui::SetCursorPos
                        (
                            ImVec2(offsetX,offsetY)
                        );

                        ImGui::GetWindowDrawList()->AddRectFilled
                        (
                            ImVec2
                            (
                            cursorPosition.x + offsetX - 8,
                            cursorPosition.y + offsetY -8
                        ),
                            ImVec2
                            (
                            cursorPosition.x + offsetX +
                            squish * textureWidth - 8,
                            cursorPosition.y + offsetY +
                            squish * textureWidth * ratio - 8
                        ),
                            IM_COL32(150, 150, 150, 255), 0.0f
                        );

                        
                        if (ImGui::ImageButton
                        (
                            (void*)_resourceManager.textures[i].texture->id(),
                            {
                                squish*textureWidth,
                                squish*textureWidth * ratio
                            }, ImVec2(0, 0), ImVec2(1, -1)
                        ))
                        {
                            _selectedTexture = &_resourceManager.textures[i];
                            ImGui::CloseCurrentPopup();
                            _opened = false;
                        }
                        
                        // What I get is that the thing is relative to the window
                        
                        
                        
                        ImGui::SetCursorPos
                        (
                            ImVec2
                            (
                            leftPadding+buttonPadding / 2.0f + column * columnWidth,
                                line * (textureWidth+40) + 5+ buttonPadding / 2.0f + 40.0f + (textureWidth )
                            )
                        );

                        ImGui::Text(_resourceManager.textures[i].name.cstr());

                        ImGui::EndGroup();
                    }
                }
            }
            ImGui::Separator();

            if (ImGui::Button("Close"))
            {
                ImGui::CloseCurrentPopup();
                _opened = false;
            }

            ImGui::EndPopup();
        }
    }
}