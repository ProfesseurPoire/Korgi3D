#pragma once

namespace corgi
{
    class Texture;
    class Renderer;
    class FrameBuffer;
}

namespace Editor
{
    // Control that is used to display/zoom in/zoom out a texture
    class Canvas
    {
    public:

        Canvas(corgi::Renderer& renderer, int width, int height);
        ~Canvas();

        void setTexture(corgi::Texture* texture);

        void show();

    private:

        void makeBackgroundTexture();

        int _width  = 0;
        int _height = 0;

        int _offsetX = 0;
        int _offsetY = 0;

        float _zoomValuePercentage = 100.0f;

        corgi::Renderer& _renderer;
        corgi::FrameBuffer* _frameBuffer = nullptr;

        corgi::Texture* _texture            = nullptr;
        corgi::Texture* _backgroundTexture  = nullptr;
    };
}