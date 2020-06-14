#include "Animator.h"

#include <corgi/main/Game.h>
#include <corgi/ecs/Scene.h>


#include <corgi/components/SpriteRenderer.h>
#include <corgi/components/Transform.h>

#include <corgi/math/easing.h>

#include <fstream>

namespace corgi
{
	Animation& Animator::current_animation()
	{
		return _animations[current_animation_];
	}

	const Animation& Animator::current_animation()const
	{
		return _animations.at(current_animation_);
	}

	void set_key_frame(Animator& animator, SpriteRenderer& sprite, int keyframeIndex)
	{
		sprite.flipX = animator.current_animation().flipped_x ^ animator.is_flipped_;
		sprite.set_sprite(animator.current_animation().frames[keyframeIndex].sprite);
		animator.current_frame_index = keyframeIndex;
	}

	void Animator::add_animation(Animation animation)
	{
		_animations.emplace(animation.name().data(), animation);
	}

	void Animator::add_animations(const std::vector<Animation>& animations)
	{
		for(auto& anim : animations)
		{
			_animations.emplace(anim.name().data(), anim);
		}
	}

	void Animator::play(Animation animation)
	{
		Animation* a=nullptr;

		if (_animations.count(std::string(animation.name())) == 0)
		{
			auto i = _animations.emplace(animation.name(), animation);
			a = &i.first->second;
		}
		else
		{
			a = &_animations.at(std::string(animation.name()));
		}

		current_animation_ = a->name_;
		current_frame_index = 0;
		current_time = 0;
		running = true;

		if (int(current_animation().frames.size()) <= 0)
		{
			current_frame_index = 0% int(current_animation().frames.size());
		}

		_current_frame_threshold = 0.0f;

		for (int i = 0; i <= current_frame_index; ++i)
		{
			_current_frame_threshold += current_animation().frames[i].time;
		}
		//set_key_frame(*this, entity_->get_component<SpriteRenderer>(), current_frame_index);
	}

	void Animator::play(const std::string& name, const int frame, const float time)
	{
		current_animation_		= name;
		current_frame_index		= frame;
		current_time			= time;
		running					= true;

		if(int(current_animation().frames.size()) <= frame)
		{
			current_frame_index = frame % int(current_animation().frames.size());
		}

		_current_frame_threshold = 0.0f;

		for (int i = 0; i <= current_frame_index; ++i)
		{
			_current_frame_threshold += current_animation().frames[i].time;
		}
	}

	void Animator::play_scaling_animation(const ScalingAnimation& animation)
	{
		scaling_animation_ = animation;
		scaling_animation_.is_playing = true;
	}

    bool Animator::is_playing(const std::string& name)
    {
		if (current_animation_ != "")
		{
			return current_animation().name() == name;
		}
        return false;
    }

	void Animator::set_animations(std::vector<Animation> animations)
	{
		for (auto& animation : animations)
		{
			_animations[animation.name().data()] = animation;
		}
	}

	Animation& Animator::new_animation(const std::string& name)
	{
		_animations.emplace(name, Animation());
		return  _animations[name];
	}

	Animator::Animator(  const Animator& animator)
		:  _animations(animator._animations)
	{
		_current_frame_threshold 	= animator._current_frame_threshold;
		is_playing_ 				= animator.is_playing_;		

		current_animation_ 	= animator.current_animation_;	
		scaling_animation_ 	= animator.scaling_animation_;

		current_frame_index = animator.current_frame_index;
		current_time		= animator.current_time;
		running				= animator.running;
		is_flipped_			= animator.is_flipped_;
	}

	Animation& Animator::new_animation(const std::string& name, const std::string& reference)
	{
		_animations.emplace(name, _animations[reference]);
		return _animations[name];
	}

	Animation& Animator::new_animation(const std::string& name, const Animation& reference)
	{
		_animations.emplace(name, reference);
		return _animations[name];
	}

	Sprite Animator::current_frame()const
	{
		if (current_frame_index == current_animation().frames.size())
		{
			return current_animation().frames[current_frame_index - 1].sprite;
		}
		return current_animation().frames[current_frame_index].sprite;
	}

	bool Animator::is_playing()const
	{
		return is_playing_;
	}

	void ScalingAnimation::stop()
	{
		is_playing = false;
	}
	
	corgi::Vec3 ScalingAnimation::value()
	{
		auto segment = current_segment();
		Vec3 v;

		float t = float(current_tick - segment.first.time) / (segment.last.time - segment.first.time);
		
		switch (segment.interpolation)
		{
		case Interpolation::Linear:
			return easing::linear_easing(t, segment.first.value, segment.last.value);
		case Interpolation::Quadratic:
			return easing::quadratic_easing_in_out(t, segment.first.value, segment.last.value);
		case Interpolation::Cubic:
			break;
		}
		return v;
	}
	
	bool ScalingAnimation::current_segment_timed_out() const noexcept
	{
		return current_segment().last.time < current_tick;
	}

	bool ScalingAnimation::has_next_segment() const
	{
		return ((unsigned int(current_segment_ + 1)) < (keyframes_.size()-1));
	}

	bool ScalingAnimation::next_segment()
	{
		if(!has_next_segment())
		{
			return false;
		}
		current_segment_++;
		return true;
	}

	void Animator::enable()
	{
		is_enabled_=true;
	}

	void Animator::disable()
	{
		is_enabled_=false;
	}

	bool Animator::is_enabled()const
	{
		return is_enabled_;
	}
}