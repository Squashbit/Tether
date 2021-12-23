#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

namespace Link::FileLoader
{
	bool LoadFromFile(std::string path, std::string* content);
}
