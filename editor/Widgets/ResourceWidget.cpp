#include "ResourceWidget.h"

#include <imgui.h>
#include <vector>

#include <corgi/filesystem/FileSystem.h>
#include <corgi/utils/ResourceManager.h>

#include <ext/imgui/IconsFontAwesome.h>
#include <ext/imgui/imgui_internal.h>

#include <GLFW/glfw3.h>

namespace Editor
{
    namespace Widgets
    {
        Resource::Resource(ResourceManager& manager)
            : _manager(manager), Widget("Resources"), _root(manager._rootFile) 
        {
            _root = buildTreeItems(manager._resourceDirectory.c_str());
        }

        TreeItem Resource::buildTreeItems(const corgi::String& directory, TreeItem* parent)
        {
            TreeItem item(directory);

            if (item.is_directory())
            {
                auto files = corgi::filesystem::list_directory(directory);

                for (auto file : corgi::filesystem::list_directory(directory))
                {
                    buildTreeItems(file.path(), &item);
                }
            }
            if(parent!=nullptr)
            {
                parent->addChild(item);
            }
            return item;
        }

        void Resource::parseFiles(TreeItem& item)
        {
            if (item.is_directory())
            {
                std::string str = ICON_FA_FOLDER;
                str += " ";
                str += item.name().cstr();
                
                ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow|
                    ImGuiTreeNodeFlags_OpenOnDoubleClick;

                bool selected=false;

                for (TreeItem* selectedItem : _selectedItems)
                {
                    if (&item == selectedItem)
                    {
                        selected = true;
                        node_flags |= ImGuiTreeNodeFlags_Selected;
                    }
                }
                
                // Only allow renaming when there is 1 item selected
                if (_selectedItems.size() == 1)
                {
                    if (selected)
                    {
                        if (ImGui::GetIO().KeysDown[GLFW_KEY_F2])
                        {
                            if (_isF2Down == false)
                            {
                                _isF2Down = true;
                                _editTreeNode = !_editTreeNode;

                                if (_editTreeNode == true)
                                {
                                    _itemEditedPath = item.path();
                                }

                                if (_editTreeNode == false)
                                {
                                    corgi::filesystem::rename(_itemEditedPath, 
                                        item.path());
                                    _refreshManager = true;
                                }
                            }
                        }
                        else
                        {
                            _isF2Down = false;
                        }
                    }
                }
                if(_editTreeNode&& selected)
                {
                    corgi::String nameStr = item.name();
                    char te[300];
                    std::strcpy(te, nameStr.data());
                    
                    ImGui::Indent();

                    ImGui::Text(ICON_FA_FOLDER);

                    ImGui::SameLine();

                    ImGui::SetKeyboardFocusHere(100);

                    ImGui::InputText("##CurrentlyEdited", te,300,
                        ImGuiInputTextFlags_AutoSelectAll);

                    ImGui::Unindent();

                    item.name(te);
                }
                else
                {
                    item.opened = ImGui::TreeNodeEx(str.c_str(), node_flags);

                    // TODO : CLEAN THIS SHIT OMG
                    // Make a real TreeView class or something because this is
                    // slowly starting to get out of hands
                    // Only allows things to be dragged if the treview is not
                    // being edited
                    if (!_editTreeNode)
                    {
                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                        {
                            _dragData = item.path();

                            ImGui::SetDragDropPayload("ResourceDrag", &_dragData, sizeof(int));        // Set payload to carry the index of our item (could be anything)
                            ImGui::Text("Move Directory %s", _dragData.data());

                            ImGui::EndDragDropSource();
                        }

                        if (ImGui::BeginDragDropTarget())
                        {
                            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ResourceDrag"))
                            {
                                std::string payload_n = *(std::string*)payload->Data;

                                for (TreeItem* selectedItem : _selectedItems)
                                {
                                    corgi::String thing = item.path();
                                    thing += "/";
                                    thing += corgi::filesystem::filename(selectedItem->path());
                                    corgi::filesystem::rename(selectedItem->path(), thing);
                                }
                                _refreshManager = true;
                                // Now I guess I do the thing haha
                            }
                            ImGui::EndDragDropTarget();
                        }
                    }
                }

                if(ImGui::IsItemClicked(0))
                {
                    if (ImGui::GetIO().KeyCtrl)
                    {
                        _selectedItems.emplace(&item);
                    }
                    else
                    {
                        _selectedItems.clear();
                        _selectedItems.emplace(&item);
                    }
                }

                // Not too sure what pushID does. I think it creates
                // an ID so this popup is unique?
                ImGui::PushID(str.c_str());

                if (ImGui::BeginPopupContextItem("Context Menu"))
                {
                    ImGui::PushItemWidth(-1);
                    ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);

                    if (ImGui::MenuItem("New Folder"))
                    {
                        corgi::String str = item.path();
                        str += "/newFolder";
                        corgi::filesystem::create_directory(str);
                        _refreshManager = true;
                    }

                    if (ImGui::MenuItem("Delete"))
                    {
                        ImGui::OpenPopup("ConfirmationFolderDelete");
                    }

                    ImGui::PopItemFlag();
                    ImGui::PopItemWidth();

                    if (ImGui::BeginPopupModal("ConfirmationFolderDelete"))
                    {
                        auto& io = ImGui::GetIO();

                        //TODO : Don't use GLFW thing please

                        if (io.KeysDown[GLFW_KEY_ENTER])
                        {
                            if (_modalOkSelected)
                            {
                                _modalOkSelected    = false;
                                _modalOpened        = false;
                                corgi::filesystem::remove_all(item.path());
                                _refreshManager = true;
                                ImGui::CloseCurrentPopup();
                            }
                            else
                            {

                            }
                        }

                        if (io.KeysDown[GLFW_KEY_TAB])
                        {
                            if (!isTabDown)
                            {
                                _modalOkSelected = !_modalOkSelected;
                                isTabDown = true;
                            }
                        }
                        else
                        {
                            isTabDown = false;
                        }

                        ImGui::Text("Are you sure you want to delete this folder");
                        auto pos = ImGui::GetCursorScreenPos();

                        int buttonWidth = 100;
                        int buttonHeight = 20;

                        if (ImGui::Button("Maybe not", ImVec2(buttonWidth, buttonHeight)))
                        {
                            _modalOkSelected = false;
                            _modalOpened = false;
                            ImGui::CloseCurrentPopup();
                        }
                        pos.x -= 2;
                        pos.y -= 2;
                        
                        ImGui::SameLine();

                        auto pos2 = ImGui::GetCursorScreenPos();
                        if (ImGui::Button("I'm sure", ImVec2(
                            buttonWidth,
                            buttonHeight)))
                        {
                            _modalOpened = false;
                            _modalOkSelected = false;
                            // close both popup
                            ImGui::CloseCurrentPopup();
                            ImGui::CloseCurrentPopup();

                            corgi::filesystem::remove_all(item.path());
                            _refreshManager = true;
                        }

                        if (!_modalOkSelected)
                        {
                            ImGui::GetWindowDrawList()->AddRect
                            (
                                pos,
                                ImVec2(pos.x + buttonWidth + 
                                    4, 4 + pos.y + buttonHeight),
                                ImGui::ColorConvertFloat4ToU32(ImVec4(
                                    0.4f, 0.4f, 1.0f, 1.0f))
                            );
                        }
                        else
                        {
                            pos2.x -= 2;
                            pos2.y -= 2;

                            ImGui::GetWindowDrawList()->AddRect
                            (
                                pos2,
                                ImVec2(pos2.x + buttonWidth +
                                    4, 4 + pos2.y + buttonHeight),
                                ImGui::ColorConvertFloat4ToU32(ImVec4(
                                    0.4f, 0.4f, 1.0f, 1.0f))
                            );
                        }
                        ImGui::EndPopup();
                    }
                    ImGui::EndPopup();
                }

                if (item.opened)
                {
                    if( (selected&& _editTreeNode))
                    {
                        ImGui::Indent();
                    }

                    for (auto& child : item.children())
                    {
                        parseFiles(child);
                    }

                    if( !(selected&& _editTreeNode))
                    {
                        ImGui::TreePop();
                    }
                    else
                    {
                        ImGui::Unindent();
                    }
                }
                ImGui::PopID();
            }
            else
            {
                ImGui::Indent();
                if ((item.extension()=="png") || (item.extension()=="bmp"))
                {
                    corgi::String str = ICON_FA_FILE_IMAGE_O;
                    str += " " ;
                    str += item.name();
                    ImGui::Selectable(str.cstr());
                }
                else if(item.extension()=="ttf")
                {
                    corgi::String str = ICON_FA_FONT;
                    str += " " ;
                    str += item.name();
                    ImGui::Selectable(str.cstr());
                }
                else
                {
                    corgi::String str = ICON_FA_FILE;
                    str += " " ;
                    str += item.name();
                    ImGui::Selectable(str.cstr());
                }

                if (!_editTreeNode)
                {
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                    {
                        _dragData = item.path();

                        ImGui::SetDragDropPayload("ResourceDrag", &_dragData, sizeof(int));        // Set payload to carry the index of our item (could be anything)
                        ImGui::Text("Move Directory %s", _dragData.data());

                        ImGui::EndDragDropSource();
                    }

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ResourceDrag"))
                        {
                            corgi::String payload_n = *(corgi::String*)payload->Data;

                            corgi::String thing = item.path();
                            thing += "/";
                            thing += corgi::filesystem::filename(_dragData);
                            corgi::filesystem::rename(_dragData, thing);
                            _refreshManager = true;
                            // Now I guess I do the thing haha
                        }
                        ImGui::EndDragDropTarget();
                    }
                }
                ImGui::Unindent();
            }
        }

        void Resource::show()
        {
            _rightClicked=false;
            corgi::String str = ICON_FA_SEARCH;
            str += " Search";
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
            ImGui::InputTextWithHint("##SearchBoxResources", str.cstr(), _searchText, 200);
            ImGui::PopStyleVar();

            ImVec2 cursorPosition   = ImGui::GetCursorPos();
            float widgetHeight      = ImGui::GetWindowHeight();
            float treeviewheight    = widgetHeight - cursorPosition.y - 10;
            
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.16f, 0.29f, 0.48f, 0.54f));
            ImGui::BeginChild("Child3", ImVec2(0, treeviewheight), true);

            parseFiles(_root);
            
            ImGui::EndChild();

            /*if(!_rightClicked)
            {
                if(ImGui::IsItemClicked(1))
                {
                    ImGui::OpenPopup("ResourcePopup");
                }
                
                if(ImGui::BeginPopup("ResourcePopup"))
                {
                    if(ImGui::Selectable("New Folder"))
                    {
                        std::string str = _manager._rootFile.path;
                        str+="/newFolder";
                        corgi::filesystem::create_directory(str);
                        _manager.refresh();
                    }
                    ImGui::EndPopup();
                }
            }*/

            if (_refreshManager)
            {
                _manager.refresh();
                _root = buildTreeItems(_manager._resourceDirectory.c_str());
                _refreshManager = false;
            }

            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
        }
    }
}