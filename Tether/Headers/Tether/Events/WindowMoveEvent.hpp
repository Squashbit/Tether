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
        
        const int64_t GetX() const;
        const int64_t GetY() const;
    private:
        int64_t x = 0;
        int64_t y = 0;
    };
}
