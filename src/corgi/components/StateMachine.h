#pragma once

#include <corgi/ecs/Component.h>

#include <corgi/math/Vec2.h>

#include <corgi/utils/Event.h>
#include <corgi/utils/TimeHelper.h>

#include <corgi/containers/Map.h>
#include <corgi/containers/Vector.h>

#include <corgi/memory/UniquePtr.h>

#include <corgi/string/String.h>

namespace corgi
{
class Animator;
class Scene;

class BlendTree
{
public:

	operator bool();

	void set_animator(Animator* animator);
	void set_input(Vec2& input);
	void add_point(float x, float y, const String& animation);
	void add_point(Vec2 point, const String& animation);

	// Only chose the animation to play once
	void play();
	void update();

	bool no_repeat_mode = false;

private:

	Vec2*                           _input   = nullptr;
	Animator*                       _animaor = nullptr;
	Vector<std::pair<Vec2, String>> _points;
};

enum class Operator
{
	Equals,
	NonEquals
};

class Transition
{
	friend class StateMachineSystem;
	friend class StateMachine;

public:

	Transition(unsigned int state)
		: _new_state(state)
	{
	}

	Transition(Transition& tr)
	{
		_lambdas   = std::move(tr._lambdas);
		booleans_  = std::move(tr.booleans_);
		timers_    = std::move(tr.timers_);
		_new_state = std::move(tr._new_state);

		comparisons_ = std::move(tr.comparisons_);
	}

	Transition& operator=(Transition& tr)
	{
		_lambdas   = tr._lambdas;
		booleans_  = tr.booleans_;
		timers_    = tr.timers_;
		_new_state = tr._new_state;

		comparisons_ = std::move(tr.comparisons_);
		return *this;
	}

	[[nodiscard]] bool process(Entity& entity);

	/*!
		* @brief	It's possible to add a condition as a lambda
		*/
	Transition& add_condition(std::function<bool(Entity& entity)> condition);

	/*!
		* @brief	You can add a condition that is simply a boolean value
		*/
	Transition& add_condition(const bool& condition);

	/*!
		* @brief	You can use a timer as a condition. Once it returns true it will triggers the transition
		*/
	Transition& add_condition(const Timer& timer);

	class AbstractComparison
	{
	public:

		virtual explicit operator bool() = 0;

		virtual ~AbstractComparison()
		{
		}
	};

	template <class T>
	class VariableComparison : public AbstractComparison
	{
	public:

		VariableComparison(const T& value, Operator comparison_operator,
		                   const T& compaired_to)
			: value_(value), comparison_operator_(comparison_operator),
			  compaired_to_(compaired_to)
		{
		}

		explicit operator bool() override
		{
			switch (comparison_operator_)
			{
			case Operator::Equals:
				return value_ == compaired_to_;
			case Operator::NonEquals:
				return value_ != compaired_to_;
			}
			return false;
		}

	private:

		const T& value_;
		T        compaired_to_;
		Operator comparison_operator_;
	};

	Vector<UniquePtr<AbstractComparison>> comparisons_;

	template <class T>
	Transition& add_condition(const T& value, Operator op, const T& comparison)
	{
		std::tuple<T, Operator, T> tuple;

		comparisons_.emplace_back(
			new VariableComparison<T>(value, op, comparison));
		return *this;
	}

protected:

	Vector<std::function<bool(Entity&)>> _lambdas;
	Vector<const bool*>                  booleans_;
	Vector<const Timer*>                 timers_;

	unsigned int _new_state;
};

class State
{
public:

	friend class StateMachineSystem;
	friend class StateMachine;

	State() = default;
	State(unsigned int id);

	Transition& new_transition(unsigned int v);


	std::function<void(Entity&)> on_exit;
	std::function<void(Entity&)> on_enter;
	std::function<void(Entity&)> on_update;
	
	[[nodiscard]] unsigned int id() const;

	BlendTree blend_tree;

protected:

	// could probably just use a std::string here
	unsigned int id_;


	std::vector<Transition> transitions_;
};

class StateMachine : public Component
{
	friend class StateMachineSystem;

public:

	//  Lifecycle

	// Functions

	State& new_state(int id);

	[[nodiscard]] State& current_state();

	[[nodiscard]] int  transitioning_to() const noexcept;
	[[nodiscard]] bool is_enabled() const;

	void current_state(int name);
	void transition_to(int name, Entity& entity);

	void set_input(Vec2& vec2);
	void set_animator(Animator* animator);

	void enable();
	void disable();

protected:

	Vec2*     input_            = nullptr;
	Animator* default_animator_ = nullptr;
	int       current_state_    = -1;
	int       transitioning_to_ = -1;

	bool is_enabled_ = true;

	Map<int, State> states_;
};
}
