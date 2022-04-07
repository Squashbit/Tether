#include <Tether/Common/StringUtils.hpp>

#include <random>

using namespace Tether;

std::string StringUtils::RandomString(uint64_t length)
{
	const std::string nums = "0123456789";
	const std::string caps = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const std::string lowers = "abcdefghijklmnopqrstuvwxyz";
	const std::string allChars = nums + caps + lowers;

	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_int_distribution<uint64_t> dist(0, allChars.size() - 1);

	std::string str;
	for (uint64_t i = 0; i < length; ++i)
		str += allChars[dist(generator)];

	return str;
}