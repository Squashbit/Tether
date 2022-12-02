#include <Tether/Common/HashedString.hpp>
#include <string>

using namespace Tether;

HashedString::HashedString(const std::string& input)
{
	std::hash<std::string> hasher;
	hashedString = hasher(input);
}

bool HashedString::operator==(const HashedString& hash) const noexcept
{
	return hashedString == hash.hashedString;
}

size_t HashedString::operator()() const
{
	return Get();
}

size_t HashedString::Get() const
{
	return hashedString;
}