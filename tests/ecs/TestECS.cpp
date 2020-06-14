#include <corgi/test/test.h>

#include <corgi/ecs/Entity.h>
#include <corgi/ecs/Scene.h>

#include <set>

using namespace corgi;
using namespace test;

class OtherComponent : public Component
{
public:

	float y;
};

class TestComponent : public Component
{
public:

	// Lifecycle

	TestComponent() = default;

	TestComponent(int val) :
		x(val)
	{
	}

	TestComponent(const TestComponent& t)
		: x(t.x)
	{
	}

	TestComponent(TestComponent&& t):
		x(t.x)
	{
		t.x = -1;
	}

	TestComponent& operator=(TestComponent&& moved) noexcept
	{
		x = moved.x;
		moved.x = -1;
		return *this;
	}

	TestComponent& operator=(const TestComponent& copied) noexcept
	{
		x = copied.x;
		return *this;
	}

	// Functions

	// Member Variables

	int x = 0;
};

int main()
{
	test::run_all();
}

class ComponentPoolsTest : public test::Test
{
public:

	Scene scene;

	ComponentPools& pools()
	{
		//std::cout << corgi::test::detail::stuffstuff.size() << std::endl;
		return scene.pools();
	}

	void set_up() override
	{
	}

	void tear_down() override
	{
	}
};

TEST_F(ComponentPoolsTest, PoolInitialization)
{
	// We first make sure that the scene is correctly initialized with zero pools
	assert_that(pools().size(), equals(0));
}

TEST_F(ComponentPoolsTest, AddPool)
{
	pools().add<TestComponent>();
	assert_that(pools().contains<TestComponent>(), equals(true));
	assert_that(pools().size(), equals(1));
}

TEST_F(ComponentPoolsTest, CheckIfPoolDoesntExist)
{
	assert_that(pools().contains<TestComponent>(), equals(false));
}

TEST_F(ComponentPoolsTest, RemovingPool)
{
	scene.pools().add<TestComponent>();
	assert_that(pools().contains<TestComponent>(), equals(true));
	assert_that(pools().size(), equals(1));

	scene.pools().remove<TestComponent>();
	assert_that(pools().contains<TestComponent>(), equals(false));
	assert_that(pools().size(), equals(0));
}

TEST_F(ComponentPoolsTest, AccessNonExistingPool)
{
	auto* pool = pools().get<TestComponent>();
	assert_that(pool, equals(nullptr));
}

TEST_F(ComponentPoolsTest, AccessPool)
{
	pools().add<TestComponent>();
	auto* pool = pools().get<TestComponent>();
	assert_that(pool, non_equals(nullptr));
}

TEST_F(ComponentPoolsTest, AddOverExistingComponentPool)
{
	pools().add<TestComponent>();

	auto* pool = pools().get<TestComponent>();

	pool->add(0);
	pool->add(1);

	assert_that(pools().get<TestComponent>()->size(), equals(2));

	auto* first_pool = pools().get<TestComponent>();

	// I want this to be ignored when we already have a component pool of the same
	// type
	pools().add<TestComponent>();

	auto* second_pool = pools().get<TestComponent>();

	assert_that(pools().get<TestComponent>()->size(), equals(2));
	assert_that(first_pool == second_pool, equals(true));
}

TEST_F(ComponentPoolsTest, IterateOver)
{
	pools().add<TestComponent>();
	pools().add<OtherComponent>();

	std::set<std::type_index> indexes{
		typeid(TestComponent), typeid(OtherComponent)
	};

	for (auto& pool : pools())
	{
		indexes.erase(pool.first);
	}

	assert_that(indexes.size(), equals(0));
	assert_that(pools().size(), equals(2));
}

class ComponentPoolTest : public test::Test
{
public:

	Scene scene;

	ComponentPool<TestComponent>* pool()
	{
		return scene.pools().get<TestComponent>();
	}

	void set_up() override
	{
		scene.pools().add<TestComponent>();
	}

	void tear_down() override
	{
	}
};

TEST_F(ComponentPoolTest, Initialization)
{
	assert_that(pool()->size(), equals(0));
}

TEST_F(ComponentPoolTest, Add)
{
	auto& e = scene.root()->add_child("Test1");

	e.add_component<TestComponent>();

	assert_that(pool()->size(), equals(1));
	assert_that(e.has_component<TestComponent>(), equals(true));
}

TEST_F(ComponentPoolTest, AddWithParameters)
{
	auto& e1 = scene.root()->add_child("Test1");
	e1.add_component<TestComponent>(1);

	assert_that(pool()->size(), equals(1));
	assert_that(e1.get_component<TestComponent>().x, equals(1));
}

TEST_F(ComponentPoolTest, AddMoreThanOne)
{
	auto& e1 = scene.root()->add_child("Test1");
	auto& e2 = scene.root()->add_child("Test2");
	auto& e3 = scene.root()->add_child("Test3");

	e1.add_component<TestComponent>();
	e2.add_component<TestComponent>();
	e3.add_component<TestComponent>();

	assert_that(pool()->size(), equals(3));
}

TEST_F(ComponentPoolTest, Remove)
{
	auto& e1 = scene.root()->add_child("Test1");
	auto& e2 = scene.root()->add_child("Test2");
	auto& e3 = scene.root()->add_child("Test3");

	e1.add_component<TestComponent>();
	e2.add_component<TestComponent>();
	e3.add_component<TestComponent>();

	e2.remove_component<TestComponent>() ;

	assert_that(e1.has_component<TestComponent>(), equals(true));

	assert_that(pool()->size(), equals(2));
	assert_that(e1.has_component<TestComponent>(), equals(true));
	assert_that(e2.has_component<TestComponent>(), equals(false));
	assert_that(e3.has_component<TestComponent>(), equals(true));
}

TEST_F(ComponentPoolTest, MoveOperator)
{
	auto& e1 = scene.root()->add_child("Test1");
	auto& e2 = scene.root()->add_child("Test2");
	auto& e3 = scene.root()->add_child("Test3");

	e1.add_component<TestComponent>(1);
	e2.add_component<TestComponent>(2);
	e3.add_component<TestComponent>(3);

	//e1.move_component(std::move(e2.get_component<TestComponent>()));

	//e1.get_component<TestComponent>() = std::move(e2.get_component<TestComponent>());

	// After moving a component, we actually remove the moved component from the
	// list
	// assert_that(pool()->size(), equals(2));

	// assert_that(e1.get_component<TestComponent>().x, equals(2));
	// assert_that(e3.get_component<TestComponent>().x, equals(3));

	// // I'm making sure the entity reference by the components are correct

	// assert_that(e1.has_component<TestComponent>(), equals(true));
	// assert_that(e2.has_component<TestComponent>(), equals(false));
	// assert_that(e3.has_component<TestComponent>(), equals(true));

	// // I'm also checking if I didn't fucked up something inside the map that links
	// // entity id to the component's position inside the vector

	// assert_that(pool()->entity_id_to_components().size(), equals(2));
	// assert_that(pool()->entity_id_to_components().at(e1.id()), equals(0));
	// assert_that(pool()->entity_id_to_components().at(e3.id()), equals(1));
}

TEST_F(ComponentPoolTest, MoveConstructor)
{
	auto& e1 = scene.root()->add_child("Test1");
	auto& e2 = scene.root()->add_child("Test2");
	auto& e3 = scene.root()->add_child("Test3");
	auto& e4 = scene.root()->add_child("Test4");

	e1.add_component<TestComponent>(1);
	e2.add_component<TestComponent>(2);
	e3.add_component<TestComponent>(3);
	e4.add_component<TestComponent>(std::move(e2.get_component<TestComponent>()));

	assert_that(e1.get_component<TestComponent>().x, equals(1));
	assert_that(e2.get_component<TestComponent>().x, equals(-1));
	assert_that(e3.get_component<TestComponent>().x, equals(3));
	assert_that(e4.get_component<TestComponent>().x, equals(2));
}

TEST_F(ComponentPoolTest, CopyOperator)
{
	// auto& e1 = scene.root()->add_child("Test1");
	// auto& e2 = scene.root()->add_child("Test2");
	// auto& e3 = scene.root()->add_child("Test3");

	// e1.add_component<TestComponent>(1);
	// e2.add_component<TestComponent>(2);
	// e3.add_component<TestComponent>(3);

	// assert_that(e1.get_component<TestComponent>().x, equals(1));

	// e1.copy_component(e2.get_component<TestComponent>());

	// // After moving a component, we actually remove the moved component from the
	// // list
	// assert_that(pool()->size(), equals(3));

	// assert_that(e1.get_component<TestComponent>().x, equals(2));
	// assert_that(e2.get_component<TestComponent>().x, equals(2))
	// assert_that(e3.get_component<TestComponent>().x, equals(3));

	// // I'm making sure the entity reference by the components are correct

	// // assert_that(e1.has_component<TestComponent>().id(), equals(true));
	// // assert_that(e2.has_component<TestComponent>().id(), equals(e2.id()));
	// // assert_that(e3.has_component<TestComponent>().id(), equals(e3.id()));

	// // I'm also checking if I didn't fucked up something inside the map that links
	// // entity id to the component's position inside the vector

	// assert_that(pool()->entity_id_to_components().size(), equals(3));
	// assert_that(pool()->entity_id_to_components().at(e1.id()), equals(0));
	// assert_that(pool()->entity_id_to_components().at(e2.id()), equals(1));
	// assert_that(pool()->entity_id_to_components().at(e3.id()), equals(2));
}

TEST_F(ComponentPoolTest, CopyConstructor)
{
	auto& e1 = scene.root()->add_child("Test1");
	auto& e2 = scene.root()->add_child("Test2");
	auto& e3 = scene.root()->add_child("Test3");
	auto& e4 = scene.root()->add_child("Test4");

	e1.add_component<TestComponent>(1);
	e2.add_component<TestComponent>(2);
	e3.add_component<TestComponent>(3);
	e4.add_component<TestComponent>(e2.get_component<TestComponent>());

	assert_that(pool()->size(), equals(4));

	// assert_that(e1.id(), equals(e1.get_component<TestComponent>().id()));
	// assert_that(e2.id(), equals(e2.get_component<TestComponent>().id()));
	// assert_that(e3.id(), equals(e3.get_component<TestComponent>().id()));
	// assert_that(e4.id(), equals(e4.get_component<TestComponent>().id()));

	assert_that(pool()->entity_id_to_components().size(), equals(4));

	assert_that(pool()->entity_id_to_components().at(e1.id()), equals(0));
	assert_that(pool()->entity_id_to_components().at(e2.id()), equals(1));
	assert_that(pool()->entity_id_to_components().at(e3.id()), equals(2));
	assert_that(pool()->entity_id_to_components().at(e4.id()), equals(3));

	assert_that(e1.get_component<TestComponent>().x, equals(1));
	assert_that(e2.get_component<TestComponent>().x, equals(2));
	assert_that(e3.get_component<TestComponent>().x, equals(3));
	assert_that(e4.get_component<TestComponent>().x, equals(2));
}

TEST_F(ComponentPoolTest, RangedBasedForLoop)
{
	auto& e1 = scene.root()->add_child("Test1");
	auto& e2 = scene.root()->add_child("Test2");
	auto& e3 = scene.root()->add_child("Test3");
	auto& e4 = scene.root()->add_child("Test4");

	e1.add_component<TestComponent>(1);
	e2.add_component<TestComponent>(2);
	e3.add_component<TestComponent>(3);
	e4.add_component<TestComponent>(4);

	int i = 0;
	for (auto& pair : *pool())
	{
		switch (i)
		{
		case 0:
			assert_that(pair.second.x, equals(1));
			break;
		case 1:
			assert_that(pair.second.x, equals(2));
			break;
		case 2:
			assert_that(pair.second.x, equals(3));
			break;
		case 3:
			assert_that(pair.second.x, equals(4));
			break;
		}
		i++;
	}
}