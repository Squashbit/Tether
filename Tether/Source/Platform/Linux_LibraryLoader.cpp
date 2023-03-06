#include <Tether/Common/LibraryLoader.hpp>

#include <dlfcn.h>

namespace Tether
{
	void* LibraryLoader::LoadLibrary(const char* path)
	{
		return dlopen(path, RTLD_LAZY | RTLD_LOCAL);
	}

	void* LibraryLoader::LoadFunction(void* handle, const char* funcName)
	{
		return dlsym(handle, funcName);
	}

	void LibraryLoader::FreeLibrary(void* library)
	{
		if (library)
			dlclose(library);
	}
}
