#pragma once

#include <corgi/containers/Vector.h>
#include <corgi/containers/Map.h>

#include <corgi/string/String.h>

#include <corgi/ecs/Component.h>
#include <corgi/rendering/Sprite.h>

#include <corgi/math/Vec3.h>

#include <corgi/resources/Animation.h>

namespace corgi
{
    class Texture;

    struct ScalingAnimation
    {
        enum class Interpolation : char
        {
            Linear,
            Quadratic,
            Cubic
        };

        struct KeyFrame
        {
            int time;
            Vec3 value;
            Interpolation interpolation;
        };
        
        struct Segment
        {
            KeyFrame first;
            KeyFrame last;
            Interpolation interpolation;
        };
        
        void stop();

        Vec3 value();
        bool has_next_segment()const;

        /*!
         * @brief	This function will try to change the current segment to the next one
         *			If there's no segment after, returns false, true otherwise
         *
         * @returns		Returns false if no next segment exist, true otherwise
         */
        bool next_segment();

        Vector<KeyFrame> keyframes_;

        [[nodiscard]]  bool current_segment_timed_out() const noexcept;

        Segment current_segment()const
        {
            return
            {
                keyframes_[current_segment_],
                keyframes_[current_segment_ + 1],
                keyframes_[current_segment_ + 1].interpolation
            };
        }
        
        int current_segment_ =  0;
        int current_tick	 =  0;
        bool is_playing = false;
    };

    

class Animator : public Component
{
public:

    friend class AnimationSystem;

//   Lifecycle

    Animator() = default;
    Animator(const Animator& a);	

// Functions

    void add_animation(Animation animation);
    void add_animations(const Vector<Animation>& animations);

    //  ?
    Animation& new_animation(const String& name);
    Animation& new_animation(const String& name, const Animation& reference);
    Animation& new_animation(const String& name, const String& reference);

    // ?
    void set_animations(Vector<Animation> animations);

    [[nodiscard]] Animation& current_animation();
    [[nodiscard]] const Animation& current_animation()const;

    void play(const String& name, int frame=0, float time=0.0f);
    void play(Animation animation);

    void play_scaling_animation(const ScalingAnimation& anim);

    /*
        * @brief   Checks if the given animation is currently playing
        * @param   name    Name of the animation we want to check
        * @return  Returns true if the animation is playing, false otherwise
        */
    bool is_playing(const String& name);

    // Returns the current frame of the current animation
    Sprite	current_frame()const;
    
    bool is_playing()const;

//  Member variables

    String		current_animation_;	// 4 

    ScalingAnimation	scaling_animation_;

    int         current_frame_index     { 0 };		// 4 
    float       current_time            { 0.0f };	// 4

    // Checks if an animation is still playing
    bool        running                 { false };	// 1 

    Map<String, Animation> _animations;	// 12

    bool is_flipped_;

    void enable();
    void disable();

    bool is_enabled()const;

private:

    bool is_enabled_;

    // We move to the next frame when current_time_ is > to current_frame_threshold 
    float _current_frame_threshold = 0.0f;	// 4
    bool is_playing_ = false;	// 1 
};

}