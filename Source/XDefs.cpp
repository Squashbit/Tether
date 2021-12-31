#include <Tether/XDefs.hpp>
#ifdef __linux__

using namespace Tether;

bool XDefs::QueryPointer(
    Display* display,
    uint32_t w,
    uint32_t* root_return,
    uint32_t* child_return,
    int* root_x_return,
    int* root_y_return,
    int* win_x_return,
    int* win_y_return,
    uint32_t* mask_return
)
{
    return XQueryPointer(display, w, (Window*)root_return, 
        (Window*)child_return, root_x_return, root_y_return, 
        win_x_return, win_y_return, mask_return);
}

#endif //__linux__