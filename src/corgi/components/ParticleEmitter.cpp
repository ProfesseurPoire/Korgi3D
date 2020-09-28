#include "ParticleEmitter.h"

#include <corgi/ecs/Scene.h>
#include <corgi/ecs/Entity.h>

#include <corgi/components/Transform.h>

#include <corgi/math/Vec2.h>
#include <corgi/math/MathUtils.h>

#include <math.h>

#include <algorithm>

namespace corgi
{
	void ParticleEmitter::update() 
	{
		if (emitting)
		{
			//  The emitter will go to sleep if not set on repeat
			if (_elapsed_ticks > emitting_time)
			{
				if (repeat)
				{
					_ticks_since_last_emit	= 0;
					_elapsed_ticks			= 0;
				}
				else
				{
					emitting = false;
				}
				return;
			}

			if ((_ticks_since_last_emit > static_cast<int>(rate_->get_value())))
			{
				_ticks_since_last_emit = 0;
				emit();
			}
			_ticks_since_last_emit++;
			_elapsed_ticks++;
		}
		update_particles();
	}

	void ParticleEmitter::lifetime(float range_start, float range_end)
	{
		lifetime_ = std::make_unique<Range>(range_start, range_end);
	}

	void ParticleEmitter::start()
	{
		
		// Cheating a bit here to make sure that some particles are emitted
		// right when the thing starts
		_ticks_since_last_emit	= 10000000;
		_elapsed_ticks			= 0;
		emitting				= true;
		_use_cached_matrix = true;
		//matrix_ = entity().get_component<Transform>().world_matrix();
	}

	void ParticleEmitter::velocity(float range_start, float range_end)
	{
		velocity_ = std::make_unique<Range>(range_start, range_end);
	}

	void ParticleEmitter::update_particles()
	{
		if (particles_.size() > 0)
		{
			for (auto& particle : particles_)
			{
				if (particle.current_life > particle.life)
				{
					entity_pool_->push(particle._entity);
					particle._entity->is_enabled(false);
				}
			}

			// We remove the "dead" particles
			particles_.erase
			(
				std::remove_if
				(
					particles_.begin(), 
					particles_.end(),
					[](const Particle& particle)
					{
						return (particle.current_life > particle.life);
					}
				),
				particles_.end()
			);
		}

		for (auto& particle : particles_)
		{
			particle.transform()->translate(particle.direction_x * particle.velocity, particle.direction_y * particle.velocity);
			particle.current_life++;

			if (particle.animation.frames.size() > 0)
			{
				particle.sprite_renderer()->alpha		= particle._transparency_over_time.get_value(particle.current_life / particle.life);
				
				particle.sprite_renderer()->set_sprite
				(
					particle.animation.frames[int( (particle.current_life / particle.life) * float(particle.animation.frames.size() - 1))].sprite
				);
			}
		}
	}

	Circle::Circle(float radius) :
		radius_(radius){}
	
	void ParticleEmitter::lifetime(float value)
	{
		lifetime_ = std::make_unique<Linear>(value);
	}
	
	void ParticleEmitter::rate(float value)
	{
		rate_ = std::make_unique<Linear>(value);
	}

	void ParticleEmitter::shape(Shape* shape)
	{
		shape_.reset(shape);
	}

	void ParticleEmitter::enable()
	{
		is_enabled_=true;
	}

	void ParticleEmitter::disable()
	{
		is_enabled_=false;
	}

	bool ParticleEmitter::is_enabled()const
	{
		return is_enabled_;
	}

	void ParticleEmitter::count(float value)
	{
		count_ = std::make_unique<Linear>(value);
	}

	void ParticleEmitter::count(float start_range, float end_range)
	{
		count_ = std::make_unique<Range>(start_range, end_range);
	}

	Vec3 Circle::point()const noexcept
	{
		// Gets a random point in a circle
		const float rr1 = static_cast<float>(rand() % RAND_MAX) / static_cast<float>(RAND_MAX);
		const float rr2 = static_cast<float>(rand() % RAND_MAX) / static_cast<float>(RAND_MAX);

		const float a = rr1 * 2.0f * math::pi_f;
		const float r = radius_ * sqrtf(rr2);

		return Vec3(r * cosf(a), r * sinf(a), 0.0f);
	}

	void ParticleEmitter::emit()
	{
		// TODO : move all of that to the particleSystem
		
		// if(!entity_pool_)
		// {
		// 	entity_pool_ = std::make_unique<EntityPool>(&entity());
		// }
		
		// int particles_to_spawn( int(count_->get_value()));

		// for (int i = 0; i < particles_to_spawn; ++i)
		// {
		// 	Particle particle;

		// 	auto* particle_entity =  entity_pool_->pop();

		// 	if (!particle_entity->has_component<Transform>())
		// 	{
		// 		particle_entity->add_component<Transform>();
		// 	}
			
		// 	if(!particle_entity->has_component<SpriteRenderer>())
		// 	{
		// 		particle_entity->add_component<SpriteRenderer>();
		// 	}

		// 	particle_entity->is_enabled(true);
		// 	particle.emitter	= this;
		// 	particle._entity	= particle_entity;
		// 	particle.animation	= animation;

		// 	particle.z			= 0;
		// 	particle.depth		= 0;
		// 	particle.depth		= particles_.size();

		// 	particle.current_life = 0;
		// 	particle.life			= lifetime_->get_value();
		// 	particle.velocity		= velocity_->get_value();

		// 	auto p = shape_->point();

		// 	particle.x = p.x;
		// 	particle.y = p.y;

		// 	particle.start_z = 0.0f;

		// 	if (is_world_position)
		// 	{
		// 		// Convert the position in world space 
		// 		particle_entity->get_component<Transform>().is_world(true);

		// 		if (_use_cached_matrix)
		// 		{
		// 			particle_entity->get_component<Transform>().position(matrix_ * Vec3(p.x, p.y, 0.0f));
		// 			_use_cached_matrix = false;
		// 		}
		// 		else
		// 		{
		// 			particle_entity->get_component<Transform>().position(entity().get_component<Transform>().world_matrix() * Vec3(p.x, p.y, 0.0f));
		// 		}
		// 	}

		// 	Vec2 v(p.x, p.y);
		// 	v = v.normalized();
			
		// 	particle.direction_x = v.x;
		// 	particle.direction_y = v.y;

		// 	particle.width	= 1.0f;
		// 	particle.height = 1.0f;
			
		// 	particle._transparency_over_time = _transparency_over_time;
		// 	particles_.push_back(particle);
		// }
	}

	std::unique_ptr<Shape> Circle::clone() const noexcept
	{
		return std::make_unique<Circle>(radius_);
	}
	
	void ParticleEmitter::stop()
	{
		emitting = false;
	}

	// Fuck that, for now the particle emitter will create new entities with transforms and sprites 

	//void ParticleEmitter::add_sprites(std::vector<SpriteRenderer>& sprites)
	//{
	//	int i = -1;
	//	for (auto& particle : particles_)
	//	{
	//		i++;
	//		float x = particle.x;
	//		float y = particle.y;
	//		float w = 1.0f;
	//		float h = 1.0f;

	//		float life			= particle.life;
	//		float current_life	= particle.current_life;

	//		float a = particle._transparency_over_time.get_value(current_life / life);
	//		Animation anim = particle.animation;

	//		if (anim.frames.size() > 0)
	//		{
	//			// A bit hacky but i do want to reuse the code for 
	//			// rendering
	//			SpriteRenderer sprite;
	//			sprite.alpha = a;

	//			int index = current_life / life * (anim.frames.size() - 1);
	//			sprite.sprite = anim.frames[index].sprite;
	//			sprite.world_matrix.identity();

	//			// using the offset was a bad idea I think
	//			// since sorting is done on with the model/world matrix
	//			sprite.offset_x = x;
	//			sprite.offset_y = y;

	//			// as simple as using the particle index in the vector
	//			// to decide for the z value
	//			particle.z = particle.start_z + ((float)(i)) * 0.00001f;

	//			sprite.offset_z = particle.z;

	//			sprites.push_back(sprite);
	//		}
	//	}
	//}


	//ParticleEmitter::ParticleEmitter(Entity& entity, const ParticleEmitter& c):
	//	Component(c.entity()),
	//	particles_(c.particles_),
	//	lifetime_(c.lifetime_->copy()),
	//	velocity_(c.velocity_->copy()),
	//	rate_(c.rate_->copy()),
	//	count_(c.count_->copy()),
	//	matrix_(c.matrix_),
	//	animation(c.animation),
	//	_transparency_over_time(c._transparency_over_time),
	//	is_world_position(c.is_world_position),
	//	running(c.running),
	//	emitting(c.emitting),
	//	_ticks_since_last_emit(c._ticks_since_last_emit),
	//	emitting_time ( c.emitting_time),
	//	_elapsed_ticks ( c._elapsed_ticks),
	//	repeat(c.repeat),
	//	shape_(c.shape_->clone())	// If shape is empty, what happens?
	//{}

	/*ParticleEmitter& ParticleEmitter::operator=(const ParticleEmitter& c)
	{
		lifetime_.reset(c.lifetime_->copy());
		velocity_.reset(c.velocity_->copy());
		rate_.reset(c.rate_->copy());
		count_.reset(c.count_->copy());

		particles_ = c.particles_;
		matrix_ = c.matrix_;
		animation = c.animation;
		shape_ = c.shape_->clone();
		
		running = c.running;
		repeat = c.repeat;
		_transparency_over_time = c._transparency_over_time;
		emitting = c.emitting;
		is_world_position = c.is_world_position;
		_ticks_since_last_emit = c._ticks_since_last_emit;

		emitting_time = c.emitting_time;
		_elapsed_ticks = c._elapsed_ticks;
		return *this;
	}*/

	void ParticleEmitter::velocity(float value)
	{
		velocity_ = std::make_unique<Linear>(value);
	}

	float ParticleEmitter::Gradient::get_value(float t)const
	{
		if (stuffs.empty())
		{
			return 1.0f;
		}

		// So there should always be 2 stuff in that
		for (size_t i = 0; i < stuffs.size() - 1; ++i)
		{
			if (t > stuffs[i].time && t < stuffs[i + 1].time)
			{
				float val = (t - stuffs[i].time) / (stuffs[i + 1].time - stuffs[i].time);
				return stuffs[i].value + val * (stuffs[i + 1].value - stuffs[i].value);
			}
		}
		return 0.0f;
	}

	Transform* ParticleEmitter::Particle::transform()
	{
		
		return &_entity->get_component<Transform>()->get();
	}

	SpriteRenderer* ParticleEmitter::Particle::sprite_renderer()
	{
		return &_entity->get_component<SpriteRenderer>()->get();
	}
}
