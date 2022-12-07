#pragma once

#include <Tether/Common/Defs.hpp>
#include <string>

namespace Tether::StringTools
{
	TETHER_EXPORT 
	std::string RandomString(size_t length);
	TETHER_EXPORT 
	size_t FindFirstOccurance(const char* str, size_t strLength, char c);
	TETHER_EXPORT 
	size_t FindLastOccurance(const char* str, size_t strLength, char c);
}
