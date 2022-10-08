#ifndef _TETHER_DEFS_HPP
#define _TETHER_DEFS_HPP

#define TETHER_NO_COPY(typename) \
    typename(const typename&) = delete; \
	typename(typename&&) = delete; \
	typename& operator=(const typename&) = delete; \
	typename& operator=(typename&&) = delete;

#if defined(_WIN32) && defined(_TETHER_BUILD_DLL)
// Ignore the 4251 warning.
#pragma warning(disable : 4251)
// MSVC requires you to put __declspec(dllexport) before every function if you are
// building a DLL.
#define TETHER_EXPORT __declspec(dllexport)
#elif defined(_WIN32) && !defined(TETHER_STATIC)
// MSVC also requires you to put this before every function if you are calling it from
// a DLL.
#define TETHER_EXPORT __declspec(dllimport)
#else
#define TETHER_EXPORT
#endif

#endif //_TETHER_DEFS_HPP