#pragma once

#ifdef _DEBUG
#define TETHER_LOG_INFO(text) \
    Application::Get().GetLogger().Log(Logger::Type::INFO, text)
#define TETHER_LOG_WARN(text) \
    Application::Get().GetLogger().Log(Logger::Type::WARN, text)
#define TETHER_LOG_ERROR(text) \
    Application::Get().GetLogger().Log(Logger::Type::ERROR, text)
#else
#define TETHER_LOG_INFO(text)
#define TETHER_LOG_WARN(text)
#define TETHER_LOG_ERROR(text)
#endif

#define TETHER_THROW_ERROR(text) TETHER_LOG_ERROR(text); throw text

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
#elif defined(_WIN32) && (!defined(_TETHER_STATIC) && !defined(_TETHER_BUILD_STATIC))
// MSVC also requires you to put this before every function if you are calling it from
// a DLL.
#define TETHER_EXPORT __declspec(dllimport)
#else
#define TETHER_EXPORT
#endif
