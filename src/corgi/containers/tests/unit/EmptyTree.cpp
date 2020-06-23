#include <corgi/test/test.h>
#include <corgi/containers/Tree.h>

#include <Component.h>

using namespace corgi::test;
using namespace corgi;

class TestEmptyTree : public Test
{
public:

//Lifecycle 

    TestEmptyTree()=default;
    TestEmptyTree(const TestEmptyTree& other)=default;
    TestEmptyTree(TestEmptyTree&& other)=default;
    virtual ~TestEmptyTree()=default;

    TestEmptyTree& operator=(const TestEmptyTree& other)=default;
    TestEmptyTree& operator=(TestEmptyTree&& other)=default;

protected:

    Tree<Component> tree;
};

TEST_F(TestEmptyTree, Size_EqualZero)
{
    assert_that(tree.children().size(), equals(0));
}

TEST_F(TestEmptyTree, Empty_EqualsTrue)
{
    assert_that(tree.children().empty(), equals(true));
}

// TEST_F(TestEmptyTree, Emplace_ConstructOneComponentInPlace)
// {
//     // Action
//     auto& a = tree.children().emplace_back();

//     // Assert
//     assert_that(a.get().number, equals(3));
//     assert_that(tree.children().size(), equals(1));
//     assert_that(tree.children().empty(), equals(false));
// }

// TEST_F(TestEmptyTree, children_emplace_parameter_pack)
// {
//     // Action
//     auto& node = tree.children().emplace_back(14);

//     // Assert
//     assert_that(node.get().number, equals(14));
//     assert_that(tree.children().size(), equals(1));
//     assert_that(tree.children().empty(), equals(false));
// }