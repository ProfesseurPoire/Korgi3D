#pragma once

#include <functional>
#include <vector>

namespace corgi
{
	template<class ... Args>
	class Event
	{
	public:
		
		template<class U>
		void operator+=(U callback)
		{
			callbacks_.emplace_back(callback);
		}
		
		void operator()(Args ... args)
		{
			for (auto& callback : callbacks_)
			{
				callback(std::forward<Args>(args)...);
			}
		}
		
		std::vector<std::function<void(Args ...)>> callbacks_;
	};
}