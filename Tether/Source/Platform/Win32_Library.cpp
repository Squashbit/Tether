#include <Tether/Common/Library.hpp>

#include <Windows.h>

namespace Tether
{
	Library::Library(std::string_view path)
	{
		m_LibraryHandle = LoadLibrary(path.data());
	}

	Library::~Library()
	{
		if (!library)
			return;

		FreeLibrary(library);
	}

	Library::Library(Library&& other)
	{
		m_LibraryHandle = other.m_LibraryHandle;
		other.m_LibraryHandle = nullptr;
	}

	void* Library::LoadFunction(std::string_view functionName)
	{
		return GetProcAddress((HMODULE)handle, functionName.data());
	}

	void* Library::GetHandle() const
	{
		return m_LibraryHandle;
	}
}
