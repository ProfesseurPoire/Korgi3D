#pragma once

#include <corgi/ecs/System.h>
#include <corgi/ecs/Scene.h>
#include <corgi/ecs/ComponentPool.h>

#include <corgi/components/Animator.h>
#include <corgi/components/SpriteRenderer.h>
#include <corgi/components/Transform.h>

namespace corgi
{

class AnimationSystem : public AbstractSystem
{
public:

	static void set_key_frame(Animator& animator, SpriteRenderer& sprite, int keyframeIndex)
	{
		sprite.flipX = animator.current_animation().flipped_x ^ animator.is_flipped_;
		sprite.set_sprite(animator.current_animation().frames[keyframeIndex].sprite);
		animator.current_frame_index = keyframeIndex;
	}

	static void update_scaling_animation(ScalingAnimation& animation, Transform& transform)
	{
		if (animation.is_playing)
		{
			if(animation.current_segment_timed_out())
			{
				if(!animation.next_segment())
				{
					animation.stop();
				}
			}
			else
			{
			/*	auto v = scaling_animation_.value();
				file << v.x << ";" << v.y << std::endl;*/
				transform.scale(animation.value());
			}
			animation.current_tick++;
		}
	}

	AnimationSystem(Scene& scene) :
		AbstractSystem(scene)
	{
		
	}

	void update()override
	{
		auto opt_pool = scene_.pools().get<Animator>();

		if(!opt_pool)
		{
			log_error("No Animator pool for Animation System to work with");
			return;
		}

		auto& pool = opt_pool->get();
		
		for(auto [entity_id, animator] : pool)
		{
			if(animator.is_enabled())
			{
				auto opt_entity = scene_.entities()[entity_id];

				if(!opt_entity)
				{
					log_error("Could not find entity attached to animator");
					continue;
				}
				
				auto& entity   =   opt_entity->get();

				update_scaling_animation(animator.scaling_animation_, entity.get_component<Transform>()->get());
	
				if (animator._animations.empty())
				{
					return;
				}

				if (!animator.running)
				{
					return;
				}

				SpriteRenderer& sprite = entity.get_component<SpriteRenderer>()->get();
				animator.current_time += Game::scene().elapsed_time();
				
				if (animator.current_time > animator._current_frame_threshold)
				{
					animator.current_frame_index++;
					
					// If we displayed every keyframe
					if (animator.current_frame_index == animator.current_animation().frames.size())
					{
						// If we're looping, we start back from the first frame
						if (animator.current_animation().looping)
						{
							animator.current_frame_index = 0;
							set_key_frame(animator, sprite, 0);
							animator._current_frame_threshold = animator.current_animation().frames[animator.current_frame_index].time;
							animator.current_time = 0.0f;
						}
						else
						{
							animator.running = false;   // stops the animation otherwise
						}
					}
					else
					{
						animator._current_frame_threshold += animator.current_animation().frames[animator.current_frame_index].time;
						set_key_frame(animator, sprite, animator.current_frame_index);
					}
				}
			}
		}
	}
};
}