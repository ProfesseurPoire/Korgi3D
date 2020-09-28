#include "StateMachine.h"

namespace corgi
{
    void BlendTree::set_animator(Animator* animator)
    {
        //_animator = animator;
    }

	void BlendTree::add_point(Vec2 point, const std::string& animation)
	{
		_points.emplace_back(point, animation);
	}

	void BlendTree::add_point(float x, float y, const std::string& animation)
	{
		_points.emplace_back(Vec2(x, y), animation);
	}

    BlendTree::operator bool()
    {
		return true;
        //return !(_points.empty() || !_animator || !_input);
    }

	void BlendTree::set_input(Vec2& input)
	{
		_input = &input;
	}

    State::State(unsigned id)
		:  id_(id)
    {

    }

    Transition& State::new_transition(unsigned int name)
	{
		return transitions_.emplace_back(name);
	}


    void BlendTree::play()
	{
		float sum = 100.0f;
		std::string selected;

		for (auto& point : _points)
		{
			Vec2 v = (point.first - *_input);
			if (v.length() < sum)
			{
				sum = v.length();
				selected = point.second;
			}
		}

		// if (_animator->current_animation().name() != selected)
		// {
		// 	_animator->play(selected);
		// }
	}

    void BlendTree::update()
    {
		if(no_repeat_mode)
		{
			return;
		}

        float sum = 100.0f;
        std::string selected;

        for (auto& point : _points)
        {
            Vec2 v = (point.first - *_input);
            if (v.length() < sum)
            {
                sum = v.length();
                selected = point.second;
            }
        }

		// if (_animator->current_animation().name() != selected)
		// {
		// 	_animator->play(selected, _animator->current_frame_index, _animator->current_time);
		// }
    }

    bool Transition::process(Entity& entity)
    {
        bool v = true;

		for (auto& lambda : _lambdas)
		{
			v = v && lambda(entity);
		}

		for (auto& boolean : booleans_)
		{
			v = v && *boolean;
		}

		for(auto* timer : timers_)
		{
			v = v && (*timer).operator bool();
		}

		for(auto& comparison : comparisons_)
		{
			v = v && (*comparison).operator bool();
		}

        return v;
    }

	Transition& Transition::add_condition(std::function<bool(Entity&)> condition)
	{
		_lambdas.push_back(condition);
		return *this;
	}

	Transition& Transition::add_condition(const Timer& timer)
	{
		timers_.push_back(&timer);
		return *this;
	}

    Transition& Transition::add_condition(
	    const bool& condition)
    {
		booleans_.push_back(&condition);
		return *this;
    }


    State& StateMachine::current_state()
    {
        return states_[current_state_];
    }

    void StateMachine::current_state(int name)
    {
		current_state_ = name;
    }

    void StateMachine::set_animator(Animator* animator)
	{
		default_animator_ = animator;
	}

    void StateMachine::enable()
    {
		is_enabled_ = true;
    }

    int StateMachine::transitioning_to() const noexcept
    {
		return transitioning_to_;
    }

    State& StateMachine::new_state(int id)
    {
		states_.try_emplace(id, id);
		current_state_ = id;

		if(input_)
		{
			//current_state_->blend_tree.set_input(*input_);
		}

		if (default_animator_)
		{
			//current_state_->blend_tree.set_animator(default_animator_);
		}
        return states_[id];
    }

    unsigned int State::id() const
    {
		return id_;
    }

	void StateMachine::disable()
	{
		is_enabled_=false;
	}

    void StateMachine::set_input(Vec2& vec2)
	{
		input_ = &vec2;
	}

	void StateMachine::transition_to(int state_id, Entity& entity)
	{
		transitioning_to_ = state_id;

    	if(states_[current_state_].on_exit)
			states_[current_state_].on_exit(entity);
		current_state_ = state_id;
    	if(states_[current_state_].on_enter)
			states_[current_state_].on_enter(entity);
	}

	bool StateMachine::is_enabled()const
	{
		return is_enabled_;
	}
}