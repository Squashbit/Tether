#include <Tether/Common/Library.hpp>

#include <dlfcn.h>

namespace Tether
{
	Library::Library(std::string_view path)
	{
		m_LibraryHandle = dlopen(path.data(), RTLD_LAZY | RTLD_LOCAL);
	}

	Library::~Library()
	{
		if (m_LibraryHandle)
			dlclose(m_LibraryHandle);
	}

	Library::Library(Library&& other)
	{
		m_LibraryHandle = other.m_LibraryHandle;
		other.m_LibraryHandle = nullptr;
	}

	void* Library::LoadFunction(std::string_view functionName)
	{
		return dlsym(m_LibraryHandle, functionName.data());
	}

	void* Library::GetHandle() const
	{
		return m_LibraryHandle;
	}
}
