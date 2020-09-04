#pragma once

namespace corgi
{
	class Entity;
	class Scene;

	/*!
	 * @brief Components are data object attached to an entity and updated by the systems
	 */
	class Component
	{
		friend class Entity;

	public:

	// Lifecycle

		Component(Entity* entity = nullptr);
		virtual ~Component();

	// Functions
		
		virtual Component* clone(Entity& e) { return nullptr; };

		const Entity& entity()const;

		[[nodiscard]] bool is_enabled() const;

		void is_enabled(bool value);

		[[nodiscard]]int id() const;

		Entity& entity();
		Entity* _entity = nullptr;	// 4 bytes

	private:

		bool is_enabled_ = true;
	};

}