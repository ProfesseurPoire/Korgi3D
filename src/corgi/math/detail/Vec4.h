#pragma once

namespace corgi
{
	namespace detail
	{
		template<class ValueType>
		class Vec4
		{
			//static_assert(std::is_arithmetic<ValueType>(),
				//"Template argument is not an arithmetic type");

		public:

			constexpr Vec4() = default;

			constexpr Vec4(ValueType x, ValueType y, ValueType z, ValueType w)
				:x(x), y(y), z(z), w(w) {}

			bool operator==(const Vec4& v)const
			{
				return (x == v.x && y == v.y && z == v.z && w == v.w);
			}

			ValueType x = 0;
			ValueType y = 0;
			ValueType z = 0;
			ValueType w = 0;
		};
	}
}