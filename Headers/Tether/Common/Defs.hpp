#ifndef _TETHER_DEFS_HPP
#define _TETHER_DEFS_HPP

#define TETHER_NO_COPY(typename) \
    typename(const typename&) = delete; \
	typename(typename&&) = delete; \
	typename& operator=(const typename&) = delete; \
	typename& operator=(typename&&) = delete;

#if defined(__INTELLISENSE__)

#if !defined(TETHER_PREVIEW_FEATURES)
#define TETHER_PREVIEW_FEATURES
#endif

#if !defined(TETHER_INCLUDE_VULKAN)
#define TETHER_INCLUDE_VULKAN
#endif

#endif // __INTELLISENSE__

#endif //_TETHER_DEFS_HPP