#pragma once

#include <Tether/Common/Defs.hpp>
#include <string>

namespace Tether
{
	class TETHER_EXPORT HashedString
	{
	public:
		HashedString(const std::string& input);
		
		bool operator==(const HashedString& hash) const noexcept;
		size_t operator()() const;

		size_t Get() const;
	private:
		size_t hashedString;
	};
}