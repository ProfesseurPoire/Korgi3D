#include <corgi/ecs/ComponentPools.h>

namespace corgi
{
	void ComponentPools::remove(const std::type_info& component_type)
	{
		pools_.erase(component_type);
	}

	bool ComponentPools::exists(const std::type_info& component_type) const
	{
		return (pools_.count(component_type) != 0);
	}

	int ComponentPools::size() const noexcept
	{
		return pools_.size();
	}

	[[nodiscard]] AbstractComponentPool* ComponentPools::get(const std::type_info& component_type)
	{
		if (!exists(component_type))
		{
			return nullptr;
		}
		return pools_.at(component_type).get();
	}
}
