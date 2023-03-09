#pragma once

#include <Tether/Common/Defs.hpp>

#include <iostream>

namespace Tether::Events
{
    class TETHER_EXPORT WindowResizeEvent
    {
    public:
        WindowResizeEvent() = default;
        WindowResizeEvent(
            uint64_t newWidth,
            uint64_t newHeight
        );
        
        const uint64_t GetNewWidth() const;
        const uint64_t GetNewHeight() const;
    private:
        uint64_t newWidth = 0;
        uint64_t newHeight = 0;
    };
}
