#include <corgi/test/test.h>

TEST(TestA,Check)
{
    ASSERT_EQ(true,true);
}

TEST(TestA,CheckEquals)
{
    int val = 10;

    ASSERT_EQ(val , 10);
}

TEST(TestA,CheckNonEquals)
{
    ASSERT_NE(10, 5);
}