#include <corgi/test/test.h>
#include <corgi/CString.h>

using namespace corgi::test;
using namespace corgi;

TEST(CStringTesting , size)
{
    assert_that(CString::size("corgi"), equals(8));
}

TEST(CStringTesting, copy)
{
    char out[9];

    CString::copy("corgi", out);

    assert_that(CString::size(out), equals(8));

    assert_that(out[0], equals('C'));
    assert_that(out[1], equals('a'));
    assert_that(out[2], equals('b'));
    assert_that(out[3], equals('b'));
    assert_that(out[4], equals('a'));
    assert_that(out[5], equals('g'));
    assert_that(out[6], equals('e'));
    assert_that(out[7], equals('s'));
    assert_that(out[8], equals('\0'));
}

TEST(CStringTest, compare)
{
    assert_that(CString::compare("corgi", "corgi"), equals(true));
    assert_that(CString::compare("corgi", "Notcorgi"), equals(false));
}

TEST(CStringTest, text_match)
{
    assert_that(CString::text_match("corgi", "corgi"),       equals(true));
    assert_that(CString::text_match("stuff", "corgi"),       equals(false));
    assert_that(CString::text_match("corgi", "corgi"),       equals(false));
    assert_that(CString::text_match("corgi", "corgi", false), equals(true));
}