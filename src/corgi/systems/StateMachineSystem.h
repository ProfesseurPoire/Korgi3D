#pragma once

#include <corgi/ecs/system.h>

namespace corgi
{

class Scene;

class StateMachineSystem : public AbstractSystem
{
public:

    StateMachineSystem(Scene& scene);

protected:

    void before_update()override;
    void update()override;
    void after_update()override;
};

}