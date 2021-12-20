#ifndef _LINK_WINDOWRESIZEEVENT_HPP
#define _LINK_WINDOWRESIZEEVENT_HPP

#include <iostream>

namespace Link::Events
{
    class WindowResizeEvent
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

#endif //_LINK_WINDOWRESIZEEVENT_HPP