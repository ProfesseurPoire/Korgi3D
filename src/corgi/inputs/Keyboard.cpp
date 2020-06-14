#include "Keyboard.h"

namespace corgi
{
void Keyboard::update()
{
	// Could probably be handled with a std::vector with a fixed size
	// it's unlikely the player will press more that a few keys at the same time
    for (int i = 0; i < 349; ++i)
    {
        _keys_down[i]    = false;
        _keys_up[i]      = false;
    }
}

bool Keyboard::key(Key k)const
{
    return _keys[static_cast<int>(k)];
}

bool Keyboard::key_down(Key k)const
{
    return _keys_down[static_cast<int>(k)];
}

bool Keyboard::key_up(Key k)const
{
    return _keys_up[static_cast<int>(k)];
}

Keyboard::Keyboard()
{
    for (int i = 0; i < 349; ++i)
    {
        _keys_down[i]    = false;
        _keys_up[i]      = false;
        _keys[i]         = false;
    }
}
}