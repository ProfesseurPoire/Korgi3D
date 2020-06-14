#pragma once

namespace Editor
{
    class Application;

    namespace Widgets
    {
        class Widget
        {
        public:

            friend class Application;
            Widget(const char* title);

            // Run the widget (kinda call the show() function)
            // with a bit of extra syntax sugar
            void exec();

            const char* title()const;

            virtual void init() {}

            bool isOpen()const;
            void isOpen(bool value);
            bool _isOpen = true;

        protected:


            // Implement the Imgui stuff here 
            virtual void show() = 0;

            void setBorderSize(float value);
            void setPadding(float x, float y);
            void setNoScrollbar(bool value);

            const char* title_ = nullptr;

            // Sending the application to every widget
            // will make it easier to retrieve things like dialogs and stuff
            Application* _application;

        private:

            float _borderSize = 1.0f;
            float _paddingX = 0.0f;
            float _paddingY = 0.0f;

            // Real Quick and dirt way to know if I need to 
            // use PushStyleVar or not 
            // Might want another way to handle that later on
            bool _applyBorderSize   = false;
            bool _applyPadding      = false;

            bool _noScrollbar = false;
        };
    }
}