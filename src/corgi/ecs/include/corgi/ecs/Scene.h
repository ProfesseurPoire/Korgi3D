#pragma once

#include <corgi/ecs/Entities.h>
#include <corgi/ecs/Systems.h>
#include <corgi/ecs/ComponentPools.h>

namespace corgi
{
	class Entity;
	
	class Scene
	{
	public:

	// Static Functions

		static Scene& main();

	// Constructors

		Scene()		= default;
		~Scene()	= default;
		
	// Functions

		/*
		 * @brief	This enum can be used to specify additional information on how to create
		 *			a new entity 
		 */
		enum class EntityCreationFlag
		{
			Default,
			NoTransform
		};

		ComponentPools& pools(){return pools_;}

		/*!
		 * @brief	Removes every entity attached to the scene
		 */
		static void clear();

		float elapsed_time(){ return _elapsed_time;}

		[[nodiscard]] Systems& systems() noexcept;
		[[nodiscard]] const Systems& systems()const noexcept;

		[[nodiscard]] Entities& entities() noexcept { return entities_; }
		[[nodiscard]] const Entities& entities() const noexcept{ return entities_; }

		void before_update(float elapsed_time);
		void update(float elapsed_time);
		void after_update(float elapsed_time);

		float _time_step;
		float _elapsed_time = 0.0f;

	private:

		Systems systems_{ *this };
		ComponentPools pools_ {systems_};
		Entities entities_{*this};
	};
}
