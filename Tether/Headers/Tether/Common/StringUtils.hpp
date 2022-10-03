#ifndef _TETHER_STRINGUTILS_HPP
#define _TETHER_STRINGUTILS_HPP

#include <Tether/Common/Defs.hpp>
#include <string>

namespace Tether::StringUtils
{
	TETHER_EXPORT std::string RandomString(uint64_t length);
}

#endif //_TETHER_STRINGUTILS_HPP