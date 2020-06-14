#include <corgi/test/test.h>

#include <corgi/ecs/Scene.h>
#include <corgi/ecs/Entity.h>

using namespace corgi;
using namespace corgi::test;

class TestEntity : public Test
{
public:

    void set_up()override
    {
        // auto& first     = scene.new_entity("First");
        // first.tags().push_back("stuff");

        // auto& second    = first.add_child("Second");
        // auto& third     = second.add_child("Third");
    }

    void tear_down()override
    {

    }

protected:

    Scene scene;
};

TEST_F(TestEntity, emplace)
{
    scene.entities().emplace("Name");
}


// TEST_F(TestEntity, Find)
// {
//     auto result = scene.find("First");
//     assert_that(result, equals(true));
// }