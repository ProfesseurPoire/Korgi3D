#include <corgi/test/test.h>

using namespace corgi::test;

class TestFixture : public corgi::test::Test
{
public:

    void set_up() override
    {
        x = 45;
        y = 10;
    }

    void tear_down() override
    {
        x = 0;
    }

    int x = 0;
    int y = 0;
};


TEST_F(TestFixture, if_x_equals_to_45)
{
    ASSERT_EQ(x, 45);
}

TEST_F(TestFixture, if_y_is_equals_to_10)
{
    ASSERT_EQ(y, 10);
}

TEST_F(TestFixture, use_assert_that)
{
    assert_that(y, equals(10));
    assert_that(y, non_equals(2));
    assert_that(0.1f, almost_equals(0.09f, 0.0200f));
}