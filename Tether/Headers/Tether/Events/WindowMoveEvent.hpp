#pragma once

#include <Tether/Common/Defs.hpp>

#include <iostream>

namespace Tether::Events
{
    class TETHER_EXPORT WindowMoveEvent
    {
    public:
        WindowMoveEvent() = default;
        WindowMoveEvent(
            int64_t x,
            int64_t y
        );
        
        int64_t GetX();
        int64_t GetY();
    private:
        int64_t x = 0;
        int64_t y = 0;
    };
}
