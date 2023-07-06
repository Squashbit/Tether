#pragma once

#include <Tether/Platform/PlatformDefs.hpp>

#include <cassert>

#define TETHER_NO_COPY(typename) \
    typename(const typename&) = delete; \
	typename(typename&&) = delete; \
	typename& operator=(const typename&) = delete; \
	typename& operator=(typename&&) = delete;

#ifdef TETHER_PLATFORM_WINDOWS
#define TETHER_DEBUG_BREAK() __debugbreak()
#elif defined(TETHER_PLATFORM_LINUX) || defined(TETHER_PLATFORM_ANDROID)
#define TETHER_DEBUG_BREAK() raise(SIGTRAP)
#else
#define TETHER_DEBUG_BREAK()
#endif

#ifndef NDEBUG
#define TETHER_ASSERT(check) assert(check)
#else
#define TETHER_ASSERT(check)
#endif

#if defined(TETHER_PLATFORM_WINDOWS) && defined(_TETHER_BUILD_DLL)
// Ignore the 4251 warning.
#pragma warning(disable : 4251)
// MSVC requires you to put __declspec(dllexport) before every function if you are
// building a DLL.
#define TETHER_EXPORT __declspec(dllexport)
#elif defined(TETHER_PLATFORM_WINDOWS) && (!defined(_TETHER_STATIC) \
	&& !defined(_TETHER_BUILD_STATIC))
// MSVC also requires you to put this before every function if you are calling it from
// a DLL.
#define TETHER_EXPORT __declspec(dllimport)
#else
#define TETHER_EXPORT
#endif
