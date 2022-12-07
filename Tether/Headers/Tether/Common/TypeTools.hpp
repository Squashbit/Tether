#pragma once

#include <Tether/Common/StringTools.hpp>

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
	template<typename T>
	const char* GetCleanFuncSig()
	{
		// Getting it here instead of the other function gives a cleaner 
		// function signature now that the messy std::string isn't being returned
		return TETHER_PRETTY_FUNCTION;
	}

	template<typename T>
	std::string GetTypeName()
	{
		const char* funcSig = GetCleanFuncSig<T>();
		size_t funcSigLength = strlen(funcSig);

		size_t startIndex = StringTools::FindFirstOccurance(
			funcSig,
			funcSigLength,
			TETHER_TYPENAME_START
		);

#if defined(__clang__) || defined(__GNUC__)
		// Cut off the "= "
		startIndex += 2;
#elif defined(_MSC_VER)
		startIndex++;
#endif

		size_t lastIndex = StringTools::FindLastOccurance(
			funcSig,
			funcSigLength,
			TETHER_TYPENAME_END
		);

		return std::string(funcSig + startIndex, lastIndex - startIndex);
	}
}
