#pragma once

#ifdef _WIN32
#define TETHER_PLATFORM_WINDOWS
#elif defined(__ANDROID__)
#define TETHER_PLATFORM_ANDROID
#error "Tether doesn't support android"
#elif defined(__linux__)
#define TETHER_PLATFORM_LINUX
#endif
