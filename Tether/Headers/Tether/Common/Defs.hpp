#pragma once

#define TETHER_NO_COPY(typename) \
    typename(const typename&) = delete; \
	typename(typename&&) = delete; \
	typename& operator=(const typename&) = delete; \
	typename& operator=(typename&&) = delete;

#define TETHER_TRY_INIT_APP_RETURNVAL(returnValue) \
	Application& app = Application::Get(); \
	if (!app.IsInitialized() && !app.Init()) \
		return returnValue;

#define TETHER_TRY_INIT_APP() \
	Application& app = Application::Get(); \
	if (!app.IsInitialized() && !app.Init()) \
		return;

#if defined(_WIN32) && defined(_TETHER_BUILD_DLL)
// Ignore the 4251 warning.
#pragma warning(disable : 4251)
// MSVC requires you to put __declspec(dllexport) before every function if you are
// building a DLL.
#define TETHER_EXPORT __declspec(dllexport)
#elif defined(_WIN32) && (!defined(_TETHER_STATIC) && !defined(_TETHER_BUILD_STATIC))
// MSVC also requires you to put this before every function if you are calling it from
// a DLL.
#define TETHER_EXPORT __declspec(dllimport)
#else
#define TETHER_EXPORT
#endif
