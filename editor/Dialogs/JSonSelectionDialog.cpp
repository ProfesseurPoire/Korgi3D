#include "JSonSelectionDialog.h"

#include <corgi/rendering/texture.h>
#include "../ext/imgui/imgui.h"

#include <corgi/String.h>
#include <corgi/CString.h>

using namespace corgi;

namespace Editor
{
JsonSelectionDialog::JsonSelectionDialog(ResourceManager& manager)
    :_resourceManager(manager) {}


void JsonSelectionDialog::open()
{
    ImGui::OpenPopup("JsonSelectionDialog");
    _opened = true;
}

std::string JsonSelectionDialog::selection()const
{
    return _selection;
}

bool JsonSelectionDialog::is_opened()const
{
    return _opened;
}

void JsonSelectionDialog::show()
{
    if(!_opened)
        return;

    if (_opened)
    {
        ImGui::OpenPopup("JsonSelectionDialog");
    }

    ImGui::SetNextWindowPos(ImVec2(200, 200));
    if (ImGui::BeginPopup("JsonSelectionDialog"))
    {
        ImVec2 cursorPosition = ImGui::GetCursorScreenPos();

        static char searchText[200];
        ImGui::InputText("Search", searchText, 200);
        ImGui::SameLine();

        static float zoomValue = 0.001f;
        ImGui::SliderFloat("Zoom", &zoomValue, 0.0f, 3.0f);

        int line = 0;
        int column = -1;

        float	columnWidth = 100.0f*zoomValue + 20.0f;
        int		leftPadding = 10;

        for (int i = 0; i < _resourceManager._json_files.size(); ++i)
        {
            if (CString::text_match(searchText, _resourceManager._json_files[i].c_str()))
            {
                // We change the way the things are displayed
                if (zoomValue < 0.1f)
                {
                    if (ImGui::Button(_resourceManager._json_files[i].c_str()))
                    {
                        _selection = _resourceManager._json_files[i];
                        ImGui::CloseCurrentPopup();
                        _opened = false;
                        callback(_selection);
                    }
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