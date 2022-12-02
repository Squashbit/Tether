#include <Tether/Common/TypeTools.hpp>

using namespace Tether;

static size_t Find(const char* str, size_t strLength, const char* toFind,
	size_t toFindLength)
{
	if (strLength < toFindLength)
		return 0;

	size_t toFindOffset = toFindLength - 1;
	for (size_t i = 0; i < strLength; i++)
	{
		if (i + toFindOffset >= strLength)
			return 0;

		bool isEqual = true;
		for (size_t i2 = 0; i2 < toFindLength; i2++)
			if (str[i + i2] != toFind[i2])
				isEqual = false;

		if (isEqual)
			return i;
	}

	return 0;
}

static size_t FindFirstOccurance(const char* str, size_t strLength, char c)
{
	for (size_t i = 0; i < strLength; i++)
		if (str[i] == c)
			return i;

	return 0;
}

static size_t FindLastOccurance(const char* str, size_t strLength, char c)
{
	if (strLength < 1)
		return 0;

	size_t i = strLength - 1;
	do
	{
		if (str[i] == c)
			return i;

		i--;
	} while (i > 0);

	return strLength - 1;
}

std::string TypeTools::GetTypeFromFuncsig(const char* funcsig)
{
	const char* funcSig = TETHER_PRETTY_FUNCTION;
	size_t funcSigLength = strlen(funcSig);

	size_t firstIndex;
	firstIndex = FindFirstOccurance(
		funcSig,
		funcSigLength,
		TETHER_TYPENAME_START
	);

#if defined(__clang__) || defined(__GNUC__)
	// Cut off the "= "
	firstIndex += 2;
#elif defined(_MSC_VER)
	// Cut off the space
	firstIndex++;
#endif

	size_t lastIndex = FindLastOccurance(
		funcSig,
		funcSigLength,
		TETHER_TYPENAME_END
	);

	return std::string(funcSig + firstIndex, lastIndex - firstIndex);
}