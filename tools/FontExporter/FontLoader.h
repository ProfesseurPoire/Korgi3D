#pragma once

#include <string>

class FontLoader
{
public:

	static bool initialize();
	static bool load(const std::string& path, const std::string& output);
};