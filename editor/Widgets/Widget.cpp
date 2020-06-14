#include "Widget.h"

#include <ext/imgui/imgui.h>

namespace Editor
{
    namespace Widgets
    {
        Widget::Widget(const char * title)
            :title_(title) {}

        // Might think of using my own framework one day instead of Imgui
        void Widget::exec()
        {
            if (!_isOpen)
                return;

            if (_applyBorderSize)
            {
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,
                    _borderSize);
            }

            if (_applyPadding)
            {
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(
                    _paddingX, _paddingY));
            }

            if (_noScrollbar)
            {
                ImGui::Begin(title_, &_isOpen, ImGuiWindowFlags_NoScrollbar);
            }
            else
            {
                ImGui::Begin(title_, &_isOpen);
            }
            

            show();

            ImGui::End();

            if (_applyPadding)
            {
                ImGui::PopStyleVar();
            }

            if (_applyBorderSize)
            {
                ImGui::PopStyleVar();
            }
        }

        void Widget::setBorderSize(float value)
        {
            _applyBorderSize = true;
            _borderSize     = value;
        }

        void Widget::setPadding(float x, float y)
        {
            _applyPadding = true;
            _paddingY = x;
            _paddingY = y;
        }

        void Widget::setNoScrollbar(bool value)
        {
            _noScrollbar = value;
        }

        const char* Widget::title()const
        {
            return title_;
        }

        bool Widget::isOpen()const
        {
            return _isOpen;
        }

        void Widget::isOpen(bool value)
        {
            _isOpen = value;
        }
    }
}