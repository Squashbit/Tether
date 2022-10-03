#ifndef _TETHER_WINDOWRESIZEEVENT_HPP
#define _TETHER_WINDOWRESIZEEVENT_HPP

#include <Tether/Common/Defs.hpp>

#include <iostream>

namespace Tether::Events
{
    class TETHER_EXPORT WindowResizeEvent
    {
    public:
        WindowResizeEvent() {}
        WindowResizeEvent(
            uint64_t newWidth,
            uint64_t newHeight
        );
        
        WindowResizeEvent(const WindowResizeEvent& ref);
		
        uint64_t GetNewWidth();
        uint64_t GetNewHeight();
    private:
        uint64_t newWidth = 0;
        uint64_t newHeight = 0;
    };
}

#endif //_TETHER_WINDOWRESIZEEVENT_HPP