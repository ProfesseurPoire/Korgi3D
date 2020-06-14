#include "Settings.h"

#include <corgi/main/Game.h>

static std::string setting_file_;

// document where the settings are stored
static ini::Document document_;

void Settings::initialize(const std::string& path)
{
	setting_file_ = path;
	document_.read(path);
}

void Settings::refresh()
{
	// Refresh actually doesn't "clear" the current setting thing, which I think is a mistake here
	// cause I'd end up with tons of stuff. I can try to make it smart by saving every new thing and cleaning
	// afterward what's actually not used 
	document_.read(setting_file_);
}

const int& Settings::get_int(const std::string& name, const std::string& group)
{
	return document_.get_int(name, group);
}

const float& Settings::get_float(const std::string& name, const std::string& group)
{
	return document_.get_float(name, group);
}

const std::string& Settings::get_string(const std::string& name, const std::string& group)
{
	return document_.get_string(name, group);
}

