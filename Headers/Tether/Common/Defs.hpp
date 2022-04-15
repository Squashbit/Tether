#ifndef _TETHER_DEFS_HPP
#define _TETHER_DEFS_HPP

#define TETHER_NO_COPY(typename) \
    typename(const typename&) = delete; \
	typename(typename&&) = delete; \
	typename& operator=(const typename&) = delete; \
	typename& operator=(typename&&) = delete;

#if !defined(TETHER_PREVIEW_FEATURES) && defined(__INTELLISENSE__)
#define TETHER_PREVIEW_FEATURES
#endif

#endif //_TETHER_DEFS_HPP