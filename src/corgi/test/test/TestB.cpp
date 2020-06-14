#include <corgi/test/test.h>

TEST(TestB, Checka)
{
    ASSERT_EQ(true,true);
}

TEST(TestB, CheckEquals)
{
    ASSERT_EQ(10, 10);
}

TEST(TestB, CheckNonEquals)
{
    ASSERT_NE(10, 5);
}
