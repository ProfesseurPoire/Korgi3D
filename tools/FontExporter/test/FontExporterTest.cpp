#include <corgi/test/test.h>
#include "config.h"

#include "FontLoader.h"

using namespace corgi::test;

class TestFontLoader : public Test
{
protected:
	
	std::string font_file_;
	std::string output_file_;
	
public:

	void set_up() override
	{
		font_file_ = RESOURCE_DIRECTORY;
		font_file_ += "/coders_crux.ttf";
		output_file_ = RESOURCE_DIRECTORY;
		output_file_ += "/coders_crux.bin";
	}

	void tear_down() override
	{
		
	}
};

TEST_F(TestFontLoader, initialization)
{
	assert_that(FontLoader::initialize(), equals(true));
}

TEST_F(TestFontLoader, try_load)
{
	assert_that(FontLoader::load(font_file_, output_file_), equals(true));
}

int main()
{
	run_all();
}