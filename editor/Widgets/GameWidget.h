#pragma once

#include <Widgets/Widget.h>

namespace corgi
{
    class FrameBuffer;
    class Camera;
    class Entity;
}

namespace Editor{ namespace Widgets {

    class Game : public Widget
    {
        corgi::FrameBuffer*	_frameBuffer{ nullptr };
        corgi::Camera*		_camera{ nullptr };
        corgi::Entity*      _cameraEntity =nullptr;

        int     _width{ 300 };
        int	    _height{ 300 };
        bool    _opened{ true };
        bool	_updateFrameBuffer{ false };

    public:

        Game(corgi::FrameBuffer* frameBuffer, corgi::Entity* cameraEntity);

        int width()const;
        int height()const;

        bool updateFrameBuffer()const;

    private:

        void show()override;
    };

 }}