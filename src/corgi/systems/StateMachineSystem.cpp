#include "StateMachineSystem.h"

#include <corgi/ecs/ComponentPools.h>
#include <corgi/ecs/Scene.h>

#include <corgi/components/StateMachine.h>
#include <corgi/ecs/Entity.h>

using namespace corgi;

StateMachineSystem::StateMachineSystem(Scene& scene):
     AbstractSystem(scene)
{

}

void StateMachineSystem::before_update()
{

}

void StateMachineSystem::after_update()
{

}

void StateMachineSystem::update()
{
	auto opt_pool = scene_.pools().get<StateMachine>();

	if(!opt_pool)
	{
		log_error("Could not find State Machine Component Pool");
		return;
	}

    for(auto [entity_id, state_machine]  : opt_pool->get())
    {
		auto opt_entity = scene_.entities()[entity_id];

    	if(opt_entity->get().name()=="character")
    	{
			//std::cout << "lol";
    	}
    	if(!opt_entity)
    	{
			log_error("Could not find the entity attached to the component");
			continue;
    	}
    	
        auto& entity = opt_entity->get();

        // First we handle the transitions
		for (auto& transition : state_machine.current_state().transitions_)
		{
			if (transition.process(entity)) // Checks if the transition's conditions are true
			{
				state_machine.transition_to(transition._new_state, entity);
				break;
			}
		}

		if (state_machine.current_state_ != -1)
		{
			if(state_machine.current_state().on_update)
				state_machine.current_state().on_update(entity);
		}
    }
}

//#include <corgi/ecs/World.h>
//#include <corgi/components/StateMachine.h>
//
//namespace corgi
//{
//    void StateMachineSystem::update(World& g, float elapsed_time)
//    {
//        auto pool = g.get_component_pool<StateMachine>();
//
//        for (auto& entity_index : pool->registered_components())
//        {
//            auto stateMachine = pool->get(entity_index);
//            
//            if (stateMachine->currentState != nullptr)
//            {
//                if (stateMachine->currentState->blend_tree)
//                    stateMachine->currentState->blend_tree.update();
//
//                if (stateMachine->currentState->on_update_)
//                    stateMachine->currentState->on_update_(elapsed_time);
//
//                auto* cs = stateMachine->currentState;
//
//                for (auto& transition : stateMachine->currentState->transitions_)
//                {
//                    if (*transition) // Checks if the transition's conditions are true
//                    {
//                        if (stateMachine->currentState->on_exit_)
//                            stateMachine->currentState->on_exit_(elapsed_time);
//
//                        stateMachine->currentState = &transition->newState;
//
//                        if (stateMachine->currentState->on_enter_)
//                            stateMachine->currentState->on_enter_(elapsed_time);
//                    }
//                }
//            }
//        }
//    }
//}