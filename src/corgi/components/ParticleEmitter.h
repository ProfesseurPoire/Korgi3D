#pragma once

#include <corgi/ecs/Component.h>

#include <corgi/rendering/texture.h>
#include <corgi/components/Animator.h>
#include <corgi/components/SpriteRenderer.h>

#include <corgi/math/Matrix.h>

#include <corgi/utils/EntityPool.h>

#include <corgi/containers/Vector.h>
#include <corgi/memory/UniquePtr.h>

namespace corgi
{
	class Transform;

	class Shape
	{
	public :

		[[nodiscard]] virtual std::unique_ptr<Shape> clone()const noexcept = 0;
		
		/*!
		 * @brief	Returns a random point from inside the defined shape
		 */
		[[nodiscard]] virtual Vec3 point() const noexcept = 0;
	};

	class Circle : public Shape
	{
	public:

	//  Lifecycle;
		
		Circle(float radius);

	//   Functions
		
		[[nodiscard]] std::unique_ptr<Shape> clone()const noexcept override;
		[[nodiscard]] Vec3 point()const noexcept override;

	private:

		float radius_;
	};

	class ParticleEmitter : public Component
	{
	public:
		
		friend class Renderer;

		class OverTimeStuff
		{
		public:

			OverTimeStuff(float t, float v):
				time(t),value(v){}

			float time;	//relative to the lifetime of the particle
			float value;
		};

		class Gradient
		{
		public:
			std::vector<OverTimeStuff> stuffs;

			float get_value(float t)const;
		};

		struct Property
		{
			virtual float get_value() = 0;
			virtual Property* copy() = 0;
		};

		struct Range : Property
		{
			Range(float start, float end) :
				start(start), end(end) {}

			float get_value()override
			{
				double r = (static_cast<double>(rand() % RAND_MAX) / RAND_MAX);
				return static_cast<float>( start + (end - start) * r);
			}

			Property* copy()override
			{
				return new Range(start,end);
			}

			float start;
			float end;
		};

		struct Linear : Property
		{
			Linear(float v) :value(v) {}

			Property* copy()override
			{
				return new Linear(value);
			}

			float get_value()override
			{
				return value;
			}
			float value;
		};

		struct Frame
		{
			float		x;
			float		y;
			float		width;
			float		height;
			Texture*	texture;
		};

		struct Particle
		{
			~Particle() = default;

			Entity* _entity;
			ParticleEmitter* emitter;

			Transform*		transform();
			SpriteRenderer* sprite_renderer();

			float current_life;
			float life;

			float velocity;

			float direction_x;
			float direction_y;
			float start_z;
			
			float x	{ 0.0f };
			float y	{ 0.0f };
			float z	{ 0.0f };

			int depth;

			float width;
			float height;

			// Frames will be chosen depending on the current
			// lifetime and the lifetime of the particle
			// for now
			Animation animation;

			Gradient _transparency_over_time;
		};

		class Burst
		{
			float time;
			std::unique_ptr<Property> count;
		};

		void lifetime(float value);

		void rate(float value);
		void shape(Shape* shape);

		void count(float value);
		void count(float start_range, float end_range);

	// Lifecycle
		
		ParticleEmitter() = default;

		ParticleEmitter(ParticleEmitter&& emiter)
			: Component(nullptr)
		{
			entity_pool_= std::move(emiter.entity_pool_);
			particles_	= std::move(emiter.particles_);
			shape_		= std::move(emiter.shape_);
			lifetime_	= std::move(emiter.lifetime_);
			velocity_	= std::move(emiter.velocity_);
			rate_		= std::move(emiter.rate_);
			count_		= std::move(emiter.count_);
		}

		ParticleEmitter& operator=(ParticleEmitter&& emiter)
		{
			entity_pool_ 	= std::move(emiter.entity_pool_);
			particles_ 		= std::move(emiter.particles_);
			shape_ 			= std::move(emiter.shape_);
			lifetime_ 		= std::move(emiter.lifetime_);
			velocity_ 		= std::move(emiter.velocity_);
			rate_ 			= std::move(emiter.rate_);
			count_ 			= std::move(emiter.count_);
			return *this;
		}

	// Functions

		void start();

		void velocity(float value);
		void velocity(float range_start, float range_end);

		void lifetime(float range_start, float range_end);

		void add_sprites(std::vector<SpriteRenderer>& sprites);
		
		void update();
		void update_particles();
		void emit();

		/*!
		 * @brief	Stop generating particles
		 */
		void stop();

		void enable();
		void disable();

		bool is_enabled()const;

	// Variable

		// How many ticks the emitter should try to emit particles
		int	 emitting_time;
		int _elapsed_ticks;
		int _ticks_since_last_emit = 0;

		bool is_world_position = false;

		// When true, the emitter is still emitting particles
		// When false, the emitter stopped emitting but some particles might
		// still be running
		bool emitting		= false;
		
		Gradient _transparency_over_time;

		// When false, there's no particles and the emitter isn't emitting
		// anymore
		bool running		= false;
		bool repeat			= false;	// Means the thing will loop
		
		Animation animation;

	private:
		
		bool _use_cached_matrix = false;
		bool is_enabled_=true;
		Matrix matrix_;

		std::unique_ptr<EntityPool> entity_pool_;

		std::vector<Particle>	particles_;

		//TODO : ALl  the unique_ptr here make it slightly difficult force
		// my hand in making a copy and move constructor/operator
		// Might want to take a look at this one day. But this whole thing will
		// need a overhaul anyways
		std::unique_ptr<Shape> shape_;
		std::unique_ptr<Property> lifetime_	= std::make_unique<Linear>(0.0f);	// lifetime of the particles
		std::unique_ptr<Property> velocity_	= std::make_unique<Linear>(0.0f);	// Initial speed of the particles
		std::unique_ptr<Property> rate_		= std::make_unique<Linear>(0.0f);		// How often particles should be emitted
		std::unique_ptr<Property> count_	= std::make_unique<Linear>(0.0f);		// How many particles must be emitted 
	};
}
