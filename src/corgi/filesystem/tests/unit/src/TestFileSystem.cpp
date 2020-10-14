#include <corgi/test/test.h>
#include <corgi/filesystem/FileSystem.h>
#include <filesystem>

using namespace corgi::test;

class TestFileSystem : public Test
{
public:

private:

};

int main()
{
    corgi::test::run_all();
}

TEST_F(TestFileSystem, extension_default_behavior)
{
    auto ex = corgi::filesystem::extension("stuff/truc/bidule.machin");

    assert_that(ex, equals("machin"));
}

TEST_F(TestFileSystem, extension_without_extension_after_dot)
{
    try
    {
        auto ex = corgi::filesystem::extension("stuff/truc/bidule.");
    }
    catch(std::exception e)
    {
        assert_that(e.what(), equals(std::string("No extension found after the dot character : stuff/truc/bidule.")));
    }
}

TEST_F(TestFileSystem, extension_without_dot)
{
    try
    {
        auto ex = corgi::filesystem::extension("stuff/truc/bidule");
    }
    catch(std::exception e)
    {
        assert_that(e.what(), equals(std::string("No extension found in filepath : stuff/truc/bidule")));
    }
}