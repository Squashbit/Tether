#pragma once

#include <Tether/Common/Defs.hpp>
#include <string>

namespace Tether::LibraryLoader
{
	TETHER_EXPORT void* LoadLibrary(const char* path);
	TETHER_EXPORT void* LoadFunction(void* handle, const char* funcName);
	TETHER_EXPORT void FreeLibrary(void* library);
}
