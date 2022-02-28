#ifndef _TETHER_WINDOWMOVEEVENT_HPP
#define _TETHER_WINDOWMOVEEVENT_HPP

#include <iostream>

namespace Tether::Events
{
    class WindowMoveEvent
    {
    public:
        WindowMoveEvent() {}
        WindowMoveEvent(
            int64_t x,
            int64_t y
        );
        
        WindowMoveEvent(const WindowMoveEvent& ref);
		
        int64_t GetX();
        int64_t GetY();
    private:
        int64_t x = 0;
        int64_t y = 0;
    };
}

#endif //_TETHER_WINDOWMOVEEVENT_HPP