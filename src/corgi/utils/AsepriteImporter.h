#pragma once

#include <string>
#include <vector>

#include <corgi/components/Animator.h>

namespace corgi
{
    class Texture;
    class Animator;
    /*
     *  @brief  Loads the data contained inside the json files aseprite
     *          generates after exporting the sprite sheet
     */
    class AsepriteImporter
    {
    public:

        // Just so the operation isn't done in the constructor
        void load(const std::string& path);

		std::vector<Animation> animations(Texture* texture);

        struct SourceSize
        {
            int w, h;
        };

        struct SpriteSourceSize
        {
            int x, y, w, h;
        };

        struct Frame
        {
            int x, y, w, h;
        };

        struct AsepriteFrame
        {
            Frame frame;
            SpriteSourceSize    sprite_source_size;
            SourceSize          source_size;
            bool rotated;
            bool trimmed;
            int duration;
        };

        struct FrameTag
        {
            std::string name;
            int from;
            int to;
            std::string direction;
        };

        struct Layer
        {
            std::string name;
            std::string blend_mode;
            int opacity;
        };

        struct Meta
        {
            std::string app;
            std::string version;
            std::string image;
            std::string format;
            std::string scale;
            std::vector<Layer> layers;
            std::vector<FrameTag> frame_tags;
            int w;
            int h;
        };

        std::vector<AsepriteFrame> frames;
        Meta meta;
    };
}