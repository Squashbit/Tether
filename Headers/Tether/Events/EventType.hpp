#ifndef _TETHER_EVENTS_HPP
#define _TETHER_EVENTS_HPP

namespace Tether::Events
{
    enum class EventType
    {
        MOUSE_MOVE,
        WINDOW_CLOSING,
        WINDOW_REPAINT,
        WINDOW_RESIZE,
        WINDOW_ERROR,
    };
}

#endif //_TETHER_EVENTS_HPP