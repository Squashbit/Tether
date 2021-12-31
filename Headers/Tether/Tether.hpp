/**
 * Main link header.
 * Includes all required headers to use the Tether library.
 * Not a link library, the Tether library.
 */

#ifndef _TETHER_TETHER_HPP
#define _TETHER_TETHER_HPP

#ifndef __linux__
#error Sorry! Linux is currently the only supported platform!
#endif //__linux__

#include <Tether/Application.hpp>
#include <Tether/Window.hpp>
#include <Tether/Controls/Button.hpp>
#include <Tether/Controls/Panel.hpp>

#endif //_TETHER_TETHER_HPP