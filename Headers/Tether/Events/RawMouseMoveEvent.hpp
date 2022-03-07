#ifndef _TETHER_RAWMOUSEMOVEEVENT_HPP
#define _TETHER_RAWMOUSEMOVEEVENT_HPP

#include <iostream>

namespace Tether::Events
{
    class RawMouseMoveEvent
    {
    public:
        RawMouseMoveEvent() {}
        RawMouseMoveEvent(
            uint64_t rawX,
            uint64_t rawY
        );
        
        RawMouseMoveEvent(const RawMouseMoveEvent& ref);
		
        int64_t GetRawX();
        int64_t GetRawY();
    private:
        int64_t rawX = 0;
        int64_t rawY = 0;
    };
}

#endif //_TETHER_RAWMOUSEMOVEEVENT_HPP