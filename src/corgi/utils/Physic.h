#pragma once

#include <corgi/math/Vec3.h>
#include <corgi/math/Ray.h>

#include <corgi/utils/types.h>
#include <array>

namespace corgi
{
	class ColliderComponent;

	class RaycastResult
	{
		friend class Physic;

	public:

		RaycastResult() = default;

		operator bool()const;

		bool collision_occured = false;
		Vec3 intersection_point;
		Vec3 intersection_normal;
		Ray	 ray;
		ColliderComponent* collider = nullptr;
		Entity* entity  {nullptr};
	};

	class Physic
	{
	public:

		static inline std::array<int_64, 64> layers;

		static void set_collision(int layer1, int layer2);
		static bool layer_colliding(int layer1, int layer2);

		[[nodiscard]] static  const RaycastResult raycast(const Vec3& start, const Vec3& direction, float distance, int_64 layer_flag);
		[[nodiscard]] static  const RaycastResult raycast(const Ray& ray, int_64 layer_flag);
	};
}