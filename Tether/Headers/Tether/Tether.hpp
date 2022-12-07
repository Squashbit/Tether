/**
 * Main Tether header.
 * Includes all required headers to use the Tether library.
 * 
 * Not a link library, the Tether library.
 */

#pragma once

#if !defined(__linux__) && !defined(_WIN32)
#error Sorry! Linux and Windows are currently the only supported platforms!
#endif

#include <Tether/Common/Ref.hpp>

#include <Tether/Application.hpp>
#include <Tether/SimpleWindow.hpp>
#include <Tether/NativeWindow.hpp>
