#include <corgi/test/test.h>

#include "ResourcesPackager.h"
#include "config.h"

using namespace corgi::test;
using namespace corgi;

int main()
{
    corgi::test::run_all();
}

class TestResourcesPackager : public Test
{
    public:

    protected:

    // just no exactly sure how to access this actually, maybe with a config file
    // it'd be more robust
    ResourcePackager resource_packager_ {PROJECT_RESOURCE_DIRECTORY};
};

TEST_F(TestResourcesPackager, check_if_file_exist)
{
    assert_that(resource_packager_.file_exist("images/explosion.png"), equals(true));
    assert_that(resource_packager_.file_exist("images/boom.png"), equals(false));
}

TEST_F(TestResourcesPackager, convert_png_to_image)
{
    resource_packager_.convert("images/explosion.png");

    // I probably could put everything into the thing
    assert_that(resource_packager_.file_exist("bin/images/explosion.img"), equals(true));
    assert_that(resource_packager_.file_exist("bin/images/explosion.tex"), equals(true));
}