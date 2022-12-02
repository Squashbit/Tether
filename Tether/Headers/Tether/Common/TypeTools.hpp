#pragma once

#include <Tether/Common/Defs.hpp>
#include <string>

#if defined(__clang__) || defined(__GNUC__)
#define TETHER_PRETTY_FUNCTION __PRETTY_FUNCTION__
#define TETHER_TYPENAME_START '='
#define TETHER_TYPENAME_END ';'
#elif defined(_MSC_VER)
#define TETHER_PRETTY_FUNCTION __FUNCSIG__
#define TETHER_TYPENAME_START '<'
#define TETHER_TYPENAME_END '>'
#else
#error "The current compiler doesn't support function signature macros."
#endif

namespace Tether::TypeTools
{
	TETHER_EXPORT std::string GetTypeFromFuncsig(const char* funcsig);

	template<typename T>
	std::string GetTypeName()
	{
		return GetTypeFromFuncsig(TETHER_PRETTY_FUNCTION);
	}
}
