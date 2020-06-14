#pragma once

#include <corgi/containers/List.h>

namespace corgi
{
    class Window;
    class GLFWwindow;

    class WindowManager
    {
    public:

        WindowManager();

        void CreateNewWindow();


        std::vector<Window*> windows;

    private :

        static void ResizeEvent(GLFWwindow* window, int width, int height);

        // I really don't like it but that's the only way to get back 
        // the stuff, since GLFW uses callback functions that don't allow
        // for data to be passed
        static WindowManager* instance;


    };
}