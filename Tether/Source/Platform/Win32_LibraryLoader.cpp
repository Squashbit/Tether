#ifdef _WIN32

#include <Tether/Common/LibraryLoader.hpp>

#include <Windows.h>

#undef LoadLibrary

namespace Tether
{
	static void* LoadLib(const char* path)
	{
		return LoadLibraryA(path);
	}

	static void FreeLib(void* library)
	{
		FreeLibrary((HMODULE)library);
	}

	void* LibraryLoader::LoadLibrary(const char* path)
	{
		return LoadLib(path);
	}

	void* LibraryLoader::LoadFunction(void* handle, const char* funcName)
	{
		return GetProcAddress((HMODULE)handle, funcName);
	}

	void LibraryLoader::FreeLibrary(void* library)
	{
		if (!library)
			return;

		FreeLib(library);
	}
}

#endif