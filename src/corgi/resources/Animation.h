#pragma once

#include <corgi/containers/Vector.h>
#include <corgi/string/String.h>
#include <corgi/rendering/Sprite.h>

#include <corgi/utils/Strings.h>

namespace corgi
{
    // TODO : might use a template here to directly use
    //          a std::array inside of a std::vector to 
    //          have a smaller size?
    // Not sure if I can store them if I do that thought
    struct Animation 
    {
        struct Frame
        {
            Sprite sprite;
            float time      { 0.0f };
        };

        std::string_view name()const
        {
            return name_;
        }

        void name(const String& name)
        {
            delete name_;
            name_ = convert_std_string(name);
        }

        Vector<Frame>  frames;					    // 16
        char*				name_       {nullptr};	// 4
        float               speed       {1.0f};		// 4 
        bool                looping     {false};	// 1 
        bool                flipped_x   {false};	// 1 

        // Total : 26
        // True total : 28
    };
}