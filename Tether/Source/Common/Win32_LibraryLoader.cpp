#ifdef _WIN32

#include <Tether/Common/LibraryLoader.hpp>

#include <Windows.h>

using namespace Tether;

#undef LoadLibrary

static void* LoadLib(const char* path)
{
	return LoadLibraryA(path);
}

static bool FreeLib(void* library)
{
	return FreeLibrary((HMODULE)library);
}

void* LibraryLoader::LoadLibrary(const char* path)
{
	return LoadLib(path);
}

void* LibraryLoader::LoadFunction(void* handle, const char* funcName)
{
	return GetProcAddress((HMODULE)handle, funcName);
}

bool LibraryLoader::FreeLibrary(void* library)
{
	if (!library)
		return false;

	return FreeLib(library);
}

#endif