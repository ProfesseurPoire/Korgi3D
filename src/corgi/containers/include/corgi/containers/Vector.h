#pragma once

#include <vector>

namespace corgi
{
template<class T>
using Vector = std::vector<T>;

namespace containers
{
template<class T>
inline void remove(std::vector<T>& v, int pos) noexcept
{
    v.erase(v.begin() +pos);
}

}
}

// I consider making my own vector class  that wraps
// the std::vector  to add some shortcut functions, because some of the stl
