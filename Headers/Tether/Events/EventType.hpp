#ifndef _TETHER_EVENTS_HPP
#define _TETHER_EVENTS_HPP

namespace Tether::Events
{
    enum class EventType
    {
        WINDOW_CLOSING,
        WINDOW_REPAINT,
        WINDOW_RESIZE,
        WINDOW_ERROR,
        WINDOW_MOVE,
    };
}

#endif //_TETHER_EVENTS_HPP