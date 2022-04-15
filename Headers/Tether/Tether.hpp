/**
 * Main Tether header.
 * Includes all required headers to use the Tether library.
 * 
 * Not a link library, the Tether library.
 */

#ifndef _TETHER_TETHER_HPP
#define _TETHER_TETHER_HPP

#if !defined(__linux__) && !defined(_WIN32)
#error Sorry! Linux and Windows are currently the only supported platforms!
#endif

#include <Tether/Application.hpp>
#include <Tether/SimpleWindow.hpp>
#include <Tether/NativeWindow.hpp>
#include <Tether/Window.hpp>

#endif //_TETHER_TETHER_HPP