#ifndef _TETHER_XDEFS_HPP
#define _TETHER_XDEFS_HPP
#ifdef __linux__

#include <iostream>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace Tether::XDefs
{
    bool QueryPointer(
        Display*		/* display */,
        uint32_t		/* w */,
        uint32_t*		/* root_return */,
        uint32_t*		/* child_return */,
        int*		/* root_x_return */,
        int*		/* root_y_return */,
        int*		/* win_x_return */,
        int*		/* win_y_return */,
        uint32_t*       /* mask_return */
    );
}

#endif //__linux__
#endif //_TETHER_XDEFS_HPP