#include <Tether/Common/StringTools.hpp>
#include <random>

using namespace Tether;

std::string StringTools::RandomString(size_t length)
{
	const std::string nums = "0123456789";
	const std::string caps = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const std::string lowers = "abcdefghijklmnopqrstuvwxyz";
	const std::string allChars = nums + caps + lowers;

	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_int_distribution<size_t> dist(0, allChars.size() - 1);

	std::string str;
	for (size_t i = 0; i < length; ++i)
		str += allChars[dist(generator)];

	return str;
}

size_t StringTools::FindFirstOccurance(const char* str, size_t strLength, char c)
{
	for (size_t i = 0; i < strLength; i++)
		if (str[i] == c)
			return i;

	return 0;
}

size_t StringTools::FindLastOccurance(const char* str, size_t strLength, char c)
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