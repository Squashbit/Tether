#pragma once

#include <Tether/Common/Defs.hpp>
#include <string>

namespace Tether
{
	class TETHER_EXPORT Library
	{
	public:
		Library(std::string_view path);
		~Library();

		Library(Library&& other);

		void* LoadFunction(std::string_view functionName);

		void* GetHandle() const;
	private:
		void* m_LibraryHandle = nullptr;
	};
}
