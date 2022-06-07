#ifndef _TETHER_DEFS_HPP
#define _TETHER_DEFS_HPP

#include <Tether/Common/VulkanDefs.hpp>

#define TETHER_NO_COPY(typename) \
    typename(const typename&) = delete; \
	typename(typename&&) = delete; \
	typename& operator=(const typename&) = delete; \
	typename& operator=(typename&&) = delete;

#endif //_TETHER_DEFS_HPP